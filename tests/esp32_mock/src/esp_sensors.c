#include "esp_sensors.h"
#include <string.h>
#include <stdio.h>

/* Mock state cho Sensor 1 */
static struct {
    bool initialized;
    bool has_fault;
    bool is_timeout;
    float mock_distance;
    bool use_explicit_distance;
    uint16_t raw_adc;
} s_mock_sensor1;

/* Mock state cho Sensor 2 */
static struct {
    bool initialized;
    bool has_fault;
    int gpio_level;
    bool tamper_detected;
    int noisy_samples[32];
    int noisy_sample_count;
    int current_sample_index;
} s_mock_sensor2;

/* ====================================================================
 * IMPLEMENTATION: CẢM BIẾN 1 (Khoảng cách / Vật cản)
 * ==================================================================== */

void mock_sensor1_reset(void) {
    s_mock_sensor1.initialized = false;
    s_mock_sensor1.has_fault = false;
    s_mock_sensor1.is_timeout = false;
    s_mock_sensor1.mock_distance = 100.0f;
    s_mock_sensor1.use_explicit_distance = true;
    s_mock_sensor1.raw_adc = 2048;
}

void mock_sensor1_set_raw_adc(uint16_t raw_adc) {
    s_mock_sensor1.raw_adc = raw_adc;
    s_mock_sensor1.use_explicit_distance = false;
}

void mock_sensor1_set_distance(float distance_cm) {
    s_mock_sensor1.mock_distance = distance_cm;
    s_mock_sensor1.use_explicit_distance = true;
}

void mock_sensor1_set_fault(bool has_fault) {
    s_mock_sensor1.has_fault = has_fault;
}

void mock_sensor1_set_timeout(bool is_timeout) {
    s_mock_sensor1.is_timeout = is_timeout;
}

esp_sensor_status_t sensor1_init(void) {
    if (s_mock_sensor1.has_fault) {
        return SENSOR_ERR_HARDWARE_FAULT;
    }
    s_mock_sensor1.initialized = true;
    return SENSOR_OK;
}

esp_sensor_status_t sensor1_read_distance(float *out_distance_cm) {
    if (!out_distance_cm) {
        return SENSOR_ERR_INVALID_PARAM;
    }
    if (!s_mock_sensor1.initialized) {
        return SENSOR_ERR_HARDWARE_FAULT;
    }
    if (s_mock_sensor1.has_fault) {
        return SENSOR_ERR_HARDWARE_FAULT;
    }
    if (s_mock_sensor1.is_timeout) {
        return SENSOR_ERR_TIMEOUT;
    }

    float distance = 0.0f;
    if (s_mock_sensor1.use_explicit_distance) {
        distance = s_mock_sensor1.mock_distance;
    } else {
        /* Chuyển đổi ADC 12-bit (0-4095) sang khoảng cách cm (0 - 400cm) */
        distance = ((float)s_mock_sensor1.raw_adc / 4095.0f) * 400.0f;
    }

    if (distance < SENSOR1_MIN_DISTANCE_CM || distance > SENSOR1_MAX_DISTANCE_CM) {
        *out_distance_cm = distance;
        return SENSOR_ERR_OUT_OF_RANGE;
    }

    *out_distance_cm = distance;
    return SENSOR_OK;
}

bool sensor1_is_object_detected(float threshold_cm, esp_sensor_status_t *out_status) {
    float current_distance = 0.0f;
    esp_sensor_status_t st = sensor1_read_distance(&current_distance);
    if (out_status) {
        *out_status = st;
    }
    if (st == SENSOR_OK) {
        return (current_distance <= threshold_cm);
    }
    return false;
}

/* ====================================================================
 * IMPLEMENTATION: CẢM BIẾN 2 (Trạng thái cửa / Từ tính)
 * ==================================================================== */

void mock_sensor2_reset(void) {
    s_mock_sensor2.initialized = false;
    s_mock_sensor2.has_fault = false;
    s_mock_sensor2.gpio_level = 0; /* 0: Door closed, 1: Door open */
    s_mock_sensor2.tamper_detected = false;
    s_mock_sensor2.noisy_sample_count = 0;
    s_mock_sensor2.current_sample_index = 0;
}

void mock_sensor2_set_gpio_level(int level) {
    s_mock_sensor2.gpio_level = level;
    s_mock_sensor2.noisy_sample_count = 0;
}

void mock_sensor2_set_tamper_detected(bool tamper) {
    s_mock_sensor2.tamper_detected = tamper;
}

void mock_sensor2_set_fault(bool has_fault) {
    s_mock_sensor2.has_fault = has_fault;
}

void mock_sensor2_set_noisy_samples(const int *samples, int count) {
    if (samples && count > 0 && count <= 32) {
        memcpy(s_mock_sensor2.noisy_samples, samples, count * sizeof(int));
        s_mock_sensor2.noisy_sample_count = count;
        s_mock_sensor2.current_sample_index = 0;
    }
}

esp_sensor_status_t sensor2_init(void) {
    if (s_mock_sensor2.has_fault) {
        return SENSOR_ERR_HARDWARE_FAULT;
    }
    s_mock_sensor2.initialized = true;
    return SENSOR_OK;
}

esp_sensor_status_t sensor2_read_door_state(door_state_t *out_state) {
    if (!out_state) {
        return SENSOR_ERR_INVALID_PARAM;
    }
    if (!s_mock_sensor2.initialized || s_mock_sensor2.has_fault) {
        *out_state = DOOR_STATE_UNKNOWN;
        return SENSOR_ERR_HARDWARE_FAULT;
    }

    if (s_mock_sensor2.tamper_detected) {
        *out_state = DOOR_STATE_TAMPER;
        return SENSOR_OK;
    }

    int raw_level = s_mock_sensor2.gpio_level;
    if (s_mock_sensor2.noisy_sample_count > 0) {
        raw_level = s_mock_sensor2.noisy_samples[s_mock_sensor2.current_sample_index];
        s_mock_sensor2.current_sample_index = (s_mock_sensor2.current_sample_index + 1) % s_mock_sensor2.noisy_sample_count;
    }

    if (raw_level == 0) {
        *out_state = DOOR_STATE_CLOSED;
    } else {
        *out_state = DOOR_STATE_OPEN;
    }

    return SENSOR_OK;
}

bool sensor2_is_door_secure(void) {
    door_state_t state;
    esp_sensor_status_t st = sensor2_read_door_state(&state);
    if (st == SENSOR_OK && state == DOOR_STATE_CLOSED) {
        return true;
    }
    return false;
}

esp_sensor_status_t sensor2_read_debounced(door_state_t *out_state, int sample_count) {
    if (!out_state || sample_count <= 0) {
        return SENSOR_ERR_INVALID_PARAM;
    }

    int open_votes = 0;
    int closed_votes = 0;

    for (int i = 0; i < sample_count; i++) {
        door_state_t st;
        esp_sensor_status_t err = sensor2_read_door_state(&st);
        if (err != SENSOR_OK) {
            *out_state = DOOR_STATE_UNKNOWN;
            return err;
        }
        if (st == DOOR_STATE_TAMPER) {
            *out_state = DOOR_STATE_TAMPER;
            return SENSOR_OK;
        }
        if (st == DOOR_STATE_OPEN) {
            open_votes++;
        } else {
            closed_votes++;
        }
    }

    /* Majority voting debouncing algorithm */
    if (open_votes > closed_votes) {
        *out_state = DOOR_STATE_OPEN;
    } else {
        *out_state = DOOR_STATE_CLOSED;
    }

    return SENSOR_OK;
}
