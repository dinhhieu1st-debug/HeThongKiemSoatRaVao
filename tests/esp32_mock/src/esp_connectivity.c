#include "esp_connectivity.h"
#include <string.h>
#include <strings.h>
#include <stdio.h>

/* Mock state cho BLE */
static struct {
    bool initialized;
    bool connected;
    bool peer_available;
    char target_mac[18];
    char device_name[32];
    int rssi;
    esp_conn_status_t sim_error;
    char last_sent_topic[64];
    char last_sent_payload[256];
    char rx_injected_topic[64];
    char rx_injected_payload[256];
    bool has_injected_rx;
} s_mock_ble;

/* Mock state cho WiFi */
static struct {
    bool initialized;
    bool connected;
    bool ap_available;
    char valid_ssid[32];
    char valid_password[32];
    int rssi;
    esp_conn_status_t sim_error;
} s_mock_wifi;

/* ====================================================================
 * IMPLEMENTATION: BLE
 * ==================================================================== */

void mock_ble_reset(void) {
    memset(&s_mock_ble, 0, sizeof(s_mock_ble));
    s_mock_ble.peer_available = true;
    s_mock_ble.rssi = -60;
    s_mock_ble.sim_error = CONN_STATUS_DISCONNECTED;
    strcpy(s_mock_ble.target_mac, "AA:BB:CC:11:22:33");
}

void mock_ble_set_peer_available(bool available, const char *valid_mac) {
    s_mock_ble.peer_available = available;
    if (valid_mac) {
        strncpy(s_mock_ble.target_mac, valid_mac, sizeof(s_mock_ble.target_mac) - 1);
    }
}

void mock_ble_set_rssi(int rssi) {
    s_mock_ble.rssi = rssi;
}

void mock_ble_set_simulated_error(esp_conn_status_t err) {
    s_mock_ble.sim_error = err;
}

void mock_ble_inject_packet(const char *topic, const char *payload) {
    if (topic) strncpy(s_mock_ble.rx_injected_topic, topic, sizeof(s_mock_ble.rx_injected_topic) - 1);
    if (payload) strncpy(s_mock_ble.rx_injected_payload, payload, sizeof(s_mock_ble.rx_injected_payload) - 1);
    s_mock_ble.has_injected_rx = true;
}

const char* mock_ble_get_last_sent_payload(void) {
    return s_mock_ble.last_sent_payload;
}

const char* mock_ble_get_last_sent_topic(void) {
    return s_mock_ble.last_sent_topic;
}

esp_conn_status_t esp_ble_init(const char *device_name) {
    if (!device_name || strlen(device_name) == 0) {
        return CONN_STATUS_ERROR;
    }
    s_mock_ble.initialized = true;
    strncpy(s_mock_ble.device_name, device_name, sizeof(s_mock_ble.device_name) - 1);
    return CONN_STATUS_DISCONNECTED;
}

esp_conn_status_t esp_ble_connect(const char *target_mac_address) {
    if (!s_mock_ble.initialized) {
        return CONN_STATUS_NOT_INITIALIZED;
    }
    if (!target_mac_address || strlen(target_mac_address) == 0) {
        return CONN_STATUS_ERROR;
    }

    if (s_mock_ble.sim_error == CONN_STATUS_TIMEOUT || s_mock_ble.sim_error == CONN_STATUS_ERROR) {
        return s_mock_ble.sim_error;
    }

    if (!s_mock_ble.peer_available) {
        return CONN_STATUS_TIMEOUT;
    }

    if (strcasecmp(target_mac_address, s_mock_ble.target_mac) != 0) {
        return CONN_STATUS_AUTH_FAIL;
    }

    s_mock_ble.connected = true;
    return CONN_STATUS_CONNECTED;
}

esp_conn_status_t esp_ble_disconnect(void) {
    if (!s_mock_ble.initialized) {
        return CONN_STATUS_NOT_INITIALIZED;
    }
    s_mock_ble.connected = false;
    return CONN_STATUS_DISCONNECTED;
}

bool esp_ble_is_connected(void) {
    return (s_mock_ble.initialized && s_mock_ble.connected);
}

esp_conn_status_t esp_ble_send_data(const char *topic, const char *payload) {
    if (!s_mock_ble.initialized) return CONN_STATUS_NOT_INITIALIZED;
    if (!s_mock_ble.connected) return CONN_STATUS_DISCONNECTED;
    if (!topic || !payload) return CONN_STATUS_ERROR;

    strncpy(s_mock_ble.last_sent_topic, topic, sizeof(s_mock_ble.last_sent_topic) - 1);
    strncpy(s_mock_ble.last_sent_payload, payload, sizeof(s_mock_ble.last_sent_payload) - 1);
    return CONN_STATUS_CONNECTED;
}

esp_conn_status_t esp_ble_receive_data(char *out_topic, char *out_payload, size_t max_len) {
    if (!s_mock_ble.initialized) return CONN_STATUS_NOT_INITIALIZED;
    if (!s_mock_ble.connected) return CONN_STATUS_DISCONNECTED;
    if (!out_topic || !out_payload || max_len == 0) return CONN_STATUS_ERROR;

    if (!s_mock_ble.has_injected_rx) {
        return CONN_STATUS_TIMEOUT;
    }

    strncpy(out_topic, s_mock_ble.rx_injected_topic, max_len - 1);
    strncpy(out_payload, s_mock_ble.rx_injected_payload, max_len - 1);
    s_mock_ble.has_injected_rx = false;
    return CONN_STATUS_CONNECTED;
}

int esp_ble_get_rssi(void) {
    if (!esp_ble_is_connected()) {
        return -100;
    }
    return s_mock_ble.rssi;
}

esp_conn_status_t esp_ble_auto_reconnect(int max_retries) {
    if (esp_ble_is_connected()) {
        return CONN_STATUS_CONNECTED;
    }
    for (int retry = 1; retry <= max_retries; retry++) {
        esp_conn_status_t st = esp_ble_connect(s_mock_ble.target_mac);
        if (st == CONN_STATUS_CONNECTED) {
            return CONN_STATUS_CONNECTED;
        }
    }
    return CONN_STATUS_TIMEOUT;
}

/* ====================================================================
 * IMPLEMENTATION: WIFI
 * ==================================================================== */

void mock_wifi_reset(void) {
    memset(&s_mock_wifi, 0, sizeof(s_mock_wifi));
    s_mock_wifi.ap_available = true;
    s_mock_wifi.rssi = -55;
    strcpy(s_mock_wifi.valid_ssid, "AccessControl_AP");
    strcpy(s_mock_wifi.valid_password, "12345678");
}

void mock_wifi_set_ap_credentials(const char *valid_ssid, const char *valid_password) {
    if (valid_ssid) strncpy(s_mock_wifi.valid_ssid, valid_ssid, sizeof(s_mock_wifi.valid_ssid) - 1);
    if (valid_password) strncpy(s_mock_wifi.valid_password, valid_password, sizeof(s_mock_wifi.valid_password) - 1);
}

void mock_wifi_set_ap_available(bool available) {
    s_mock_wifi.ap_available = available;
}

void mock_wifi_set_rssi(int rssi) {
    s_mock_wifi.rssi = rssi;
}

void mock_wifi_set_simulated_error(esp_conn_status_t err) {
    s_mock_wifi.sim_error = err;
}

esp_conn_status_t esp_wifi_init(void) {
    s_mock_wifi.initialized = true;
    s_mock_wifi.connected = false;
    return CONN_STATUS_DISCONNECTED;
}

esp_conn_status_t esp_wifi_connect(const char *ssid, const char *password) {
    if (!s_mock_wifi.initialized) {
        return CONN_STATUS_NOT_INITIALIZED;
    }
    if (!ssid || strlen(ssid) == 0) {
        return CONN_STATUS_ERROR;
    }
    if (s_mock_wifi.sim_error == CONN_STATUS_TIMEOUT || s_mock_wifi.sim_error == CONN_STATUS_ERROR) {
        return s_mock_wifi.sim_error;
    }
    if (!s_mock_wifi.ap_available) {
        return CONN_STATUS_TIMEOUT;
    }
    if (strcmp(ssid, s_mock_wifi.valid_ssid) != 0) {
        return CONN_STATUS_TIMEOUT; /* SSID not found */
    }
    if (password && strcmp(password, s_mock_wifi.valid_password) != 0) {
        return CONN_STATUS_AUTH_FAIL; /* Wrong password */
    }

    s_mock_wifi.connected = true;
    return CONN_STATUS_CONNECTED;
}

esp_conn_status_t esp_wifi_disconnect(void) {
    if (!s_mock_wifi.initialized) return CONN_STATUS_NOT_INITIALIZED;
    s_mock_wifi.connected = false;
    return CONN_STATUS_DISCONNECTED;
}

bool esp_wifi_is_connected(void) {
    return (s_mock_wifi.initialized && s_mock_wifi.connected);
}

esp_conn_status_t esp_wifi_send_data(const char *endpoint, const char *payload) {
    if (!esp_wifi_is_connected()) return CONN_STATUS_DISCONNECTED;
    if (!endpoint || !payload) return CONN_STATUS_ERROR;
    return CONN_STATUS_CONNECTED;
}
