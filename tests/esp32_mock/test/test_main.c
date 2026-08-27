#include "unity.h"
#include "esp_sensors.h"
#include "esp_connectivity.h"
#include <stdio.h>

void setUp(void) {
    mock_sensor1_reset();
    sensor1_init();
    mock_sensor2_reset();
    sensor2_init();
    mock_ble_reset();
    mock_wifi_reset();
    esp_ble_init("ESP32_AccessControl");
    esp_wifi_init();
}

void tearDown(void) {
    esp_ble_disconnect();
    esp_wifi_disconnect();
}

/* Khai báo các test function của Sensor 1 */
extern void test_sensor1_init_success(void);
extern void test_sensor1_init_hardware_fault(void);
extern void test_sensor1_read_normal_valid_distance(void);
extern void test_sensor1_read_adc_conversion(void);
extern void test_sensor1_read_out_of_range_low(void);
extern void test_sensor1_read_out_of_range_high(void);
extern void test_sensor1_read_timeout_error(void);
extern void test_sensor1_detect_object_within_threshold(void);
extern void test_sensor1_detect_object_outside_threshold(void);
extern void test_sensor1_read_null_pointer_safety(void);

/* Khai báo các test function của Sensor 2 */
extern void test_sensor2_init_success(void);
extern void test_sensor2_read_door_closed(void);
extern void test_sensor2_read_door_open(void);
extern void test_sensor2_read_tamper_alert(void);
extern void test_sensor2_read_hardware_fault(void);
extern void test_sensor2_debounce_filter_noise(void);
extern void test_sensor2_read_null_pointer_safety(void);

/* Khai báo các test function của BLE & WiFi */
extern void test_ble_init_success(void);
extern void test_ble_connect_success(void);
extern void test_ble_connect_auth_fail_wrong_mac(void);
extern void test_ble_connect_peer_unavailable_timeout(void);
extern void test_ble_send_rfid_card_payload(void);
extern void test_ble_send_data_when_disconnected(void);
extern void test_ble_receive_unlock_command(void);
extern void test_ble_auto_reconnect_mechanism(void);
extern void test_wifi_connect_success(void);
extern void test_wifi_connect_auth_fail(void);
extern void test_wifi_connect_ap_not_found(void);
extern void test_wifi_send_telemetry_data(void);

int main(void) {
    UNITY_BEGIN();

    printf("\n======================================================================\n");
    printf("   [1] KIỂM THỬ CẢM BIẾN 1: ĐỌC KHOẢNG CÁCH / VẬT CẢN (10 TEST CASES) \n");
    printf("======================================================================\n");
    RUN_TEST(test_sensor1_init_success);
    RUN_TEST(test_sensor1_init_hardware_fault);
    RUN_TEST(test_sensor1_read_normal_valid_distance);
    RUN_TEST(test_sensor1_read_adc_conversion);
    RUN_TEST(test_sensor1_read_out_of_range_low);
    RUN_TEST(test_sensor1_read_out_of_range_high);
    RUN_TEST(test_sensor1_read_timeout_error);
    RUN_TEST(test_sensor1_detect_object_within_threshold);
    RUN_TEST(test_sensor1_detect_object_outside_threshold);
    RUN_TEST(test_sensor1_read_null_pointer_safety);

    printf("\n======================================================================\n");
    printf("   [2] KIỂM THỬ CẢM BIẾN 2: ĐỌC TRẠNG THÁI CỬA / TỪ TÍNH (7 TEST CASES)\n");
    printf("======================================================================\n");
    RUN_TEST(test_sensor2_init_success);
    RUN_TEST(test_sensor2_read_door_closed);
    RUN_TEST(test_sensor2_read_door_open);
    RUN_TEST(test_sensor2_read_tamper_alert);
    RUN_TEST(test_sensor2_read_hardware_fault);
    RUN_TEST(test_sensor2_debounce_filter_noise);
    RUN_TEST(test_sensor2_read_null_pointer_safety);

    printf("\n======================================================================\n");
    printf("   [3] KIỂM THỬ KẾT NỐI KHÔNG DÂY: BLE & WIFI (12 TEST CASES)         \n");
    printf("======================================================================\n");
    RUN_TEST(test_ble_init_success);
    RUN_TEST(test_ble_connect_success);
    RUN_TEST(test_ble_connect_auth_fail_wrong_mac);
    RUN_TEST(test_ble_connect_peer_unavailable_timeout);
    RUN_TEST(test_ble_send_rfid_card_payload);
    RUN_TEST(test_ble_send_data_when_disconnected);
    RUN_TEST(test_ble_receive_unlock_command);
    RUN_TEST(test_ble_auto_reconnect_mechanism);
    RUN_TEST(test_wifi_connect_success);
    RUN_TEST(test_wifi_connect_auth_fail);
    RUN_TEST(test_wifi_connect_ap_not_found);
    RUN_TEST(test_wifi_send_telemetry_data);

    return UNITY_END();
}
