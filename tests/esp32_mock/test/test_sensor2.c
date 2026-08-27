#include "unity.h"
#include "esp_sensors.h"
#include <stdio.h>

/* 1. Test khởi tạo cảm biến 2 thành công */
void test_sensor2_init_success(void) {
    printf("   [TEST] 1. Khởi tạo Cảm biến 2 (Cảm biến Cửa / Từ tính)...\n");
    printf("          -> Mock: Driver GPIO cảm biến cửa hoạt động tốt.\n");
    mock_sensor2_reset();
    esp_sensor_status_t status = sensor2_init();
    printf("          -> Kết quả: Mã trạng thái = %d (SENSOR_OK)\n", status);
    TEST_ASSERT_EQUAL_INT(SENSOR_OK, status);
}

/* 2. Test đọc trạng thái cửa ĐÓNG (GPIO = 0) */
void test_sensor2_read_door_closed(void) {
    printf("   [TEST] 2. Đọc cảm biến 2 khi cửa đang ĐÓNG...\n");
    printf("          -> Mock: Mức logic chân GPIO = 0 (Tiếp điểm đóng).\n");
    door_state_t state;
    mock_sensor2_set_gpio_level(0);

    esp_sensor_status_t status = sensor2_read_door_state(&state);
    bool secure = sensor2_is_door_secure();
    printf("          -> Kết quả: Trạng thái cửa = %d (DOOR_STATE_CLOSED), An toàn = %s\n",
           state, secure ? "TRUE (Đã khóa an toàn)" : "FALSE");

    TEST_ASSERT_EQUAL_INT(SENSOR_OK, status);
    TEST_ASSERT_EQUAL_INT(DOOR_STATE_CLOSED, state);
    TEST_ASSERT_TRUE(secure);
}

/* 3. Test đọc trạng thái cửa MỞ (GPIO = 1) */
void test_sensor2_read_door_open(void) {
    printf("   [TEST] 3. Đọc cảm biến 2 khi cửa bị MỞ...\n");
    printf("          -> Mock: Mức logic chân GPIO = 1 (Tiếp điểm hở/cửa mở).\n");
    door_state_t state;
    mock_sensor2_set_gpio_level(1);

    esp_sensor_status_t status = sensor2_read_door_state(&state);
    bool secure = sensor2_is_door_secure();
    printf("          -> Kết quả: Trạng thái cửa = %d (DOOR_STATE_OPEN), An toàn = %s\n",
           state, secure ? "TRUE" : "FALSE (Cửa đang mở!)");

    TEST_ASSERT_EQUAL_INT(SENSOR_OK, status);
    TEST_ASSERT_EQUAL_INT(DOOR_STATE_OPEN, state);
    TEST_ASSERT_FALSE(secure);
}

/* 4. Test phát hiện hành vi cạy cửa/phá hoại */
void test_sensor2_read_tamper_alert(void) {
    printf("   [TEST] 4. Phát hiện hành vi cạy phá/tháo gỡ cảm biến (Tamper Alert)...\n");
    printf("          -> Mock: Kích hoạt công tắc bảo vệ chống cạy (Tamper = true).\n");
    door_state_t state;
    mock_sensor2_set_tamper_detected(true);

    esp_sensor_status_t status = sensor2_read_door_state(&state);
    printf("          -> Kết quả: Cảnh báo an ninh! Trạng thái = %d (DOOR_STATE_TAMPER)\n", state);

    TEST_ASSERT_EQUAL_INT(SENSOR_OK, status);
    TEST_ASSERT_EQUAL_INT(DOOR_STATE_TAMPER, state);
    TEST_ASSERT_FALSE(sensor2_is_door_secure());
}

/* 5. Test lỗi phần cứng cảm biến cửa */
void test_sensor2_read_hardware_fault(void) {
    printf("   [TEST] 5. Kiểm tra khi cảm biến cửa bị mất nguồn / đứt cáp...\n");
    printf("          -> Mock: Mất kết nối chân tín hiệu (Fault = true).\n");
    door_state_t state = DOOR_STATE_CLOSED;
    mock_sensor2_set_fault(true);

    esp_sensor_status_t status = sensor2_read_door_state(&state);
    printf("          -> Kết quả: Bắt mã lỗi phần cứng = %d (SENSOR_ERR_HARDWARE_FAULT)\n", status);

    TEST_ASSERT_EQUAL_INT(SENSOR_ERR_HARDWARE_FAULT, status);
    TEST_ASSERT_EQUAL_INT(DOOR_STATE_UNKNOWN, state);
}

/* 6. Test thuật toán khử rung/lọc nhiễu tín hiệu giả */
void test_sensor2_debounce_filter_noise(void) {
    printf("   [TEST] 6. Kiểm tra thuật toán lọc xung nhiễu chập chờn (Debounce Filter)...\n");
    printf("          -> Mock: Chuỗi mẫu tín hiệu [0, 0, 1 (xung nhiễu giả), 0, 0].\n");
    door_state_t debounced_state;
    int noisy_signal[] = {0, 0, 1, 0, 0};
    mock_sensor2_set_noisy_samples(noisy_signal, 5);

    esp_sensor_status_t status = sensor2_read_debounced(&debounced_state, 5);
    printf("          -> Kết quả: Thuật toán lọc bỏ thành công xung 1 giả, Trạng thái chuẩn = %d (DOOR_STATE_CLOSED)\n", debounced_state);

    TEST_ASSERT_EQUAL_INT(SENSOR_OK, status);
    TEST_ASSERT_EQUAL_INT(DOOR_STATE_CLOSED, debounced_state);
}

/* 7. Test kiểm tra an toàn con trỏ NULL */
void test_sensor2_read_null_pointer_safety(void) {
    printf("   [TEST] 7. Kiểm tra an toàn bộ nhớ khi truyền con trỏ NULL...\n");
    esp_sensor_status_t status = sensor2_read_door_state(NULL);
    printf("          -> Kết quả: Bắt mã lỗi = %d (SENSOR_ERR_INVALID_PARAM)\n", status);
    TEST_ASSERT_EQUAL_INT(SENSOR_ERR_INVALID_PARAM, status);
}
