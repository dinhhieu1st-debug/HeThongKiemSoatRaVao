#ifndef ESP_SENSORS_H
#define ESP_SENSORS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ====================================================================
 * ĐỊNH NGHĨA TRẠNG THÁI & MÃ LỖI CỦA CẢM BIẾN
 * ==================================================================== */
typedef enum {
    SENSOR_OK = 0,
    SENSOR_ERR_TIMEOUT = -1,
    SENSOR_ERR_OUT_OF_RANGE = -2,
    SENSOR_ERR_HARDWARE_FAULT = -3,
    SENSOR_ERR_INVALID_PARAM = -4
} esp_sensor_status_t;

typedef enum {
    DOOR_STATE_CLOSED = 0,
    DOOR_STATE_OPEN = 1,
    DOOR_STATE_TAMPER = 2,
    DOOR_STATE_UNKNOWN = 3
} door_state_t;

/* ====================================================================
 * CHỨC NĂNG 1: ĐỌC GIÁ TRỊ CẢM BIẾN 1 (CẢM BIẾN KHOẢNG CÁCH / VẬT CẢN)
 * ==================================================================== */
#define SENSOR1_MIN_DISTANCE_CM  2.0f
#define SENSOR1_MAX_DISTANCE_CM  400.0f
#define SENSOR1_DEFAULT_THRESHOLD_CM 50.0f

esp_sensor_status_t sensor1_init(void);
esp_sensor_status_t sensor1_read_distance(float *out_distance_cm);
bool sensor1_is_object_detected(float threshold_cm, esp_sensor_status_t *out_status);

/* Mock / Giả lập dữ liệu cho Cảm biến 1 */
void mock_sensor1_reset(void);
void mock_sensor1_set_raw_adc(uint16_t raw_adc);
void mock_sensor1_set_distance(float distance_cm);
void mock_sensor1_set_fault(bool has_fault);
void mock_sensor1_set_timeout(bool is_timeout);


/* ====================================================================
 * CHỨC NĂNG 2: ĐỌC GIÁ TRỊ CẢM BIẾN 2 (CẢM BIẾN TRẠNG THÁI CỬA / CÔNG TẮC TỪ)
 * ==================================================================== */
esp_sensor_status_t sensor2_init(void);
esp_sensor_status_t sensor2_read_door_state(door_state_t *out_state);
bool sensor2_is_door_secure(void);
esp_sensor_status_t sensor2_read_debounced(door_state_t *out_state, int sample_count);

/* Mock / Giả lập dữ liệu cho Cảm biến 2 */
void mock_sensor2_reset(void);
void mock_sensor2_set_gpio_level(int level);
void mock_sensor2_set_tamper_detected(bool tamper);
void mock_sensor2_set_fault(bool has_fault);
void mock_sensor2_set_noisy_samples(const int *samples, int count);

#ifdef __cplusplus
}
#endif

#endif /* ESP_SENSORS_H */
