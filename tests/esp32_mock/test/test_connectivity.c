#include "unity.h"
#include "esp_connectivity.h"
#include <string.h>
#include <stdio.h>

/* 1. Test khởi tạo BLE */
void test_ble_init_success(void) {
    printf("   [TEST] 1. Khởi tạo BLE Stack...\n");
    printf("          -> Mock: Khởi tạo với Device Name = 'ESP32_AccessControl'\n");
    mock_ble_reset();
    esp_conn_status_t status = esp_ble_init("ESP32_Device");
    printf("          -> Kết quả: Khởi tạo xong, Trạng thái = %d (Chờ kết nối)\n", status);
    TEST_ASSERT_EQUAL_INT(CONN_STATUS_DISCONNECTED, status);
    TEST_ASSERT_FALSE(esp_ble_is_connected());
}

/* 2. Test kết nối BLE tới đúng địa chỉ MAC */
void test_ble_connect_success(void) {
    printf("   [TEST] 2. Kết nối BLE tới địa chỉ MAC của Raspberry Pi...\n");
    printf("          -> Mock: Raspberry Pi đang phát sóng BLE với MAC = 'B8:27:EB:12:34:56'\n");
    mock_ble_set_peer_available(true, "B8:27:EB:12:34:56");

    esp_conn_status_t status = esp_ble_connect("B8:27:EB:12:34:56");
    printf("          -> Kết quả: Kết nối thành công! Trạng thái = %d (CONN_STATUS_CONNECTED)\n", status);

    TEST_ASSERT_EQUAL_INT(CONN_STATUS_CONNECTED, status);
    TEST_ASSERT_TRUE(esp_ble_is_connected());
}

/* 3. Test kết nối BLE thất bại do sai MAC */
void test_ble_connect_auth_fail_wrong_mac(void) {
    printf("   [TEST] 3. Kết nối BLE với địa chỉ MAC không hợp lệ...\n");
    printf("          -> Mock: Cố gắng kết nối tới MAC lạ 'FF:FF:FF:00:00:00'\n");
    mock_ble_set_peer_available(true, "B8:27:EB:12:34:56");

    esp_conn_status_t status = esp_ble_connect("FF:FF:FF:00:00:00");
    printf("          -> Kết quả: Từ chối kết nối, Mã lỗi = %d (CONN_STATUS_AUTH_FAIL)\n", status);

    TEST_ASSERT_EQUAL_INT(CONN_STATUS_AUTH_FAIL, status);
    TEST_ASSERT_FALSE(esp_ble_is_connected());
}

/* 4. Test kết nối BLE bị Timeout */
void test_ble_connect_peer_unavailable_timeout(void) {
    printf("   [TEST] 4. Kết nối BLE khi thiết bị ngoài tầm phủ sóng (Timeout)...\n");
    printf("          -> Mock: Pi bị tắt Bluetooth hoặc nằm ngoài phạm vi sóng (Peer = false)\n");
    mock_ble_set_peer_available(false, "B8:27:EB:12:34:56");

    esp_conn_status_t status = esp_ble_connect("B8:27:EB:12:34:56");
    printf("          -> Kết quả: Hết thời gian chờ, Mã lỗi = %d (CONN_STATUS_TIMEOUT)\n", status);

    TEST_ASSERT_EQUAL_INT(CONN_STATUS_TIMEOUT, status);
    TEST_ASSERT_FALSE(esp_ble_is_connected());
}

/* 5. Test gửi dữ liệu quét thẻ RFID qua BLE */
void test_ble_send_rfid_card_payload(void) {
    printf("   [TEST] 5. Gửi dữ liệu quẹt thẻ RFID từ ESP32 sang Pi qua BLE...\n");
    printf("          -> Mock: Quẹt thẻ UID = 'E200001901080123' qua BLE topic 'rfid/scan'\n");
    mock_ble_set_peer_available(true, "B8:27:EB:12:34:56");
    esp_ble_connect("B8:27:EB:12:34:56");

    esp_conn_status_t status = esp_ble_send_data("rfid/scan", "E200001901080123");
    printf("          -> Kết quả: Đã gửi gói tin lên GATT Characteristic! Topic: '%s', Payload: '%s'\n",
           mock_ble_get_last_sent_topic(), mock_ble_get_last_sent_payload());

    TEST_ASSERT_EQUAL_INT(CONN_STATUS_CONNECTED, status);
    TEST_ASSERT_EQUAL_STRING("rfid/scan", mock_ble_get_last_sent_topic());
    TEST_ASSERT_EQUAL_STRING("E200001901080123", mock_ble_get_last_sent_payload());
}

/* 6. Test gửi dữ liệu khi chưa kết nối BLE */
void test_ble_send_data_when_disconnected(void) {
    printf("   [TEST] 6. Thử gửi dữ liệu khi BLE đang bị ngắt kết nối...\n");
    esp_conn_status_t status = esp_ble_send_data("rfid/scan", "E200001901080123");
    printf("          -> Kết quả: Chặn gửi an toàn, Mã trạng thái = %d (CONN_STATUS_DISCONNECTED)\n", status);
    TEST_ASSERT_EQUAL_INT(CONN_STATUS_DISCONNECTED, status);
}

/* 7. Test nhận lệnh điều khiển mở cửa từ BLE */
void test_ble_receive_unlock_command(void) {
    printf("   [TEST] 7. Nhận lệnh mở cửa từ Raspberry Pi gửi sang ESP32 qua BLE...\n");
    printf("          -> Mock: Pi phát gói tin topic 'door/command' với nội dung 'UNLOCK_DOOR'\n");
    mock_ble_set_peer_available(true, "B8:27:EB:12:34:56");
    esp_ble_connect("B8:27:EB:12:34:56");

    mock_ble_inject_packet("door/command", "UNLOCK_DOOR");

    char topic[64] = {0};
    char payload[128] = {0};
    esp_conn_status_t status = esp_ble_receive_data(topic, payload, sizeof(payload));
    printf("          -> Kết quả: ESP32 giải mã gói tin: Topic='%s', Command='%s'\n", topic, payload);

    TEST_ASSERT_EQUAL_INT(CONN_STATUS_CONNECTED, status);
    TEST_ASSERT_EQUAL_STRING("door/command", topic);
    TEST_ASSERT_EQUAL_STRING("UNLOCK_DOOR", payload);
}

/* 8. Test tự động kết nối lại BLE */
void test_ble_auto_reconnect_mechanism(void) {
    printf("   [TEST] 8. Kiểm tra cơ chế tự động kết nối lại (Auto Reconnect)...\n");
    printf("          -> Mock: Thử lại tối đa 3 lần sau khi tín hiệu sóng hồi phục\n");
    mock_ble_set_peer_available(true, "B8:27:EB:12:34:56");
    esp_conn_status_t status = esp_ble_auto_reconnect(3);
    printf("          -> Kết quả: Tự động khôi phục kết nối thành công! Mã = %d\n", status);

    TEST_ASSERT_EQUAL_INT(CONN_STATUS_CONNECTED, status);
    TEST_ASSERT_TRUE(esp_ble_is_connected());
}

/* 9. Test kết nối WiFi */
void test_wifi_connect_success(void) {
    printf("   [TEST] 9. Kết nối WiFi với SSID và Mật khẩu chính xác...\n");
    printf("          -> Mock: SSID = 'AccessControl_AP', Pass = '12345678'\n");
    mock_wifi_set_ap_credentials("AccessControl_AP", "12345678");

    esp_conn_status_t status = esp_wifi_connect("AccessControl_AP", "12345678");
    printf("          -> Kết quả: Kết nối Access Point thành công! Mã = %d\n", status);

    TEST_ASSERT_EQUAL_INT(CONN_STATUS_CONNECTED, status);
    TEST_ASSERT_TRUE(esp_wifi_is_connected());
}

/* 10. Test kết nối WiFi sai mật khẩu */
void test_wifi_connect_auth_fail(void) {
    printf("   [TEST] 10. Kết nối WiFi khi nhập sai mật khẩu...\n");
    printf("          -> Mock: Password nhập vào = 'wrong_pass_999'\n");
    mock_wifi_set_ap_credentials("AccessControl_AP", "12345678");

    esp_conn_status_t status = esp_wifi_connect("AccessControl_AP", "wrong_pass_999");
    printf("          -> Kết quả: Bắt lỗi xác thực, Mã = %d (CONN_STATUS_AUTH_FAIL)\n", status);

    TEST_ASSERT_EQUAL_INT(CONN_STATUS_AUTH_FAIL, status);
    TEST_ASSERT_FALSE(esp_wifi_is_connected());
}

/* 11. Test kết nối WiFi không tìm thấy SSID */
void test_wifi_connect_ap_not_found(void) {
    printf("   [TEST] 11. Kết nối WiFi tới SSID không tồn tại...\n");
    printf("          -> Mock: Tìm kiếm Access Point 'Unknown_WiFi_SSID'\n");
    mock_wifi_set_ap_credentials("AccessControl_AP", "12345678");

    esp_conn_status_t status = esp_wifi_connect("Unknown_WiFi_SSID", "12345678");
    printf("          -> Kết quả: Không tìm thấy mạng, Bắt mã lỗi = %d (CONN_STATUS_TIMEOUT)\n", status);

    TEST_ASSERT_EQUAL_INT(CONN_STATUS_TIMEOUT, status);
    TEST_ASSERT_FALSE(esp_wifi_is_connected());
}

/* 12. Test gửi telemetry qua WiFi */
void test_wifi_send_telemetry_data(void) {
    printf("   [TEST] 12. Gửi telemetry log sự kiện cửa qua WiFi HTTP endpoint...\n");
    printf("          -> Mock: Gửi JSON event: '{\"event\":\"DOOR_OPEN\",\"time\":1700000000}'\n");
    mock_wifi_set_ap_credentials("AccessControl_AP", "12345678");
    esp_wifi_connect("AccessControl_AP", "12345678");

    esp_conn_status_t status = esp_wifi_send_data("/api/v1/door/event", "{\"event\":\"DOOR_OPEN\",\"time\":1700000000}");
    printf("          -> Kết quả: Đẩy gói tin telemetry lên Server thành công! Mã = %d\n", status);

    TEST_ASSERT_EQUAL_INT(CONN_STATUS_CONNECTED, status);
}
