#ifndef ESP_CONNECTIVITY_H
#define ESP_CONNECTIVITY_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ====================================================================
 * ĐỊNH NGHĨA TRẠNG THÁI & MÃ LỖI KẾT NỐI WIFI / BLE
 * ==================================================================== */
typedef enum {
    CONN_STATUS_DISCONNECTED = 0,
    CONN_STATUS_CONNECTING = 1,
    CONN_STATUS_CONNECTED = 2,
    CONN_STATUS_AUTH_FAIL = -1,
    CONN_STATUS_TIMEOUT = -2,
    CONN_STATUS_NOT_INITIALIZED = -3,
    CONN_STATUS_ERROR = -4
} esp_conn_status_t;

typedef struct {
    char device_name[32];
    char connected_mac[18];
    int rssi;
    bool is_ble_connected;
    bool is_wifi_connected;
    char wifi_ssid[32];
    char wifi_ip[16];
    uint32_t packets_sent;
    uint32_t packets_received;
} esp_conn_info_t;

/* ====================================================================
 * CHỨC NĂNG 3: KẾT NỐI BLUETOOTH LOW ENERGY (BLE)
 * ==================================================================== */
esp_conn_status_t esp_ble_init(const char *device_name);
esp_conn_status_t esp_ble_connect(const char *target_mac_address);
esp_conn_status_t esp_ble_disconnect(void);
bool esp_ble_is_connected(void);
esp_conn_status_t esp_ble_send_data(const char *topic, const char *payload);
esp_conn_status_t esp_ble_receive_data(char *out_topic, char *out_payload, size_t max_len);
int esp_ble_get_rssi(void);
esp_conn_status_t esp_ble_auto_reconnect(int max_retries);

/* Mock / Giả lập kết nối BLE */
void mock_ble_reset(void);
void mock_ble_set_peer_available(bool available, const char *valid_mac);
void mock_ble_set_rssi(int rssi);
void mock_ble_set_simulated_error(esp_conn_status_t err);
void mock_ble_inject_packet(const char *topic, const char *payload);
const char* mock_ble_get_last_sent_payload(void);
const char* mock_ble_get_last_sent_topic(void);

/* ====================================================================
 * CHỨC NĂNG 4: KẾT NỐI WIFI
 * ==================================================================== */
esp_conn_status_t esp_wifi_init(void);
esp_conn_status_t esp_wifi_connect(const char *ssid, const char *password);
esp_conn_status_t esp_wifi_disconnect(void);
bool esp_wifi_is_connected(void);
esp_conn_status_t esp_wifi_send_data(const char *endpoint, const char *payload);

/* Mock / Giả lập kết nối WiFi */
void mock_wifi_reset(void);
void mock_wifi_set_ap_credentials(const char *valid_ssid, const char *valid_password);
void mock_wifi_set_ap_available(bool available);
void mock_wifi_set_rssi(int rssi);
void mock_wifi_set_simulated_error(esp_conn_status_t err);

#ifdef __cplusplus
}
#endif

#endif /* ESP_CONNECTIVITY_H */
