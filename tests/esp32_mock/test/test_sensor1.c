#include "unity.h"
#include "esp_sensors.h"
#include <stdio.h>

/* 1. Test khởi tạo thành công */
void test_sensor1_init_success(void) {
    printf("   [TEST] 1. Khởi tạo Cảm biến 1...\n");
    printf("          -> Mock: Phần cứng bình thường, không có lỗi.\n");
    mock_sensor1_reset();
    esp_sensor_status_t status = sensor1_init();
    printf("          -> Kết quả: Mã trạng thái = %d (SENSOR_OK)\n", status);
    TEST_ASSERT_EQUAL_INT(SENSOR_OK, status);
}

/* 2. Test khởi tạo thất bại khi lỗi phần cứng */
void test_sensor1_init_hardware_fault(void) {
    printf("   [TEST] 2. Khởi tạo khi cảm biến 1 bị lỗi phần cứng / đứt dây...\n");
    printf("          -> Mock: Giả lập lỗi phần cứng (Hardware Fault = true).\n");
    mock_sensor1_reset();
    mock_sensor1_set_fault(true);
    esp_sensor_status_t status = sensor1_init();
    printf("          -> Kết quả: Bắt mã lỗi = %d (SENSOR_ERR_HARDWARE_FAULT)\n", status);
    TEST_ASSERT_EQUAL_INT(SENSOR_ERR_HARDWARE_FAULT, status);
}

/* 3. Test đọc khoảng cách bình thường (Normal Case) */
void test_sensor1_read_normal_valid_distance(void) {
    printf("   [TEST] 3. Đọc khoảng cách cảm biến 1 ở điều kiện tiêu chuẩn...\n");
    printf("          -> Mock: Giá trị đo được thực tế = 25.50 cm.\n");
    float distance = 0.0f;
    mock_sensor1_set_distance(25.5f);

    esp_sensor_status_t status = sensor1_read_distance(&distance);
    printf("          -> Kết quả: Đọc thành công! Khoảng cách = %.2f cm (Mã: %d)\n", distance, status);

    TEST_ASSERT_EQUAL_INT(SENSOR_OK, status);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 25.5f, distance);
}

/* 4. Test chuyển đổi từ ADC thô */
void test_sensor1_read_adc_conversion(void) {
    printf("   [TEST] 4. Đọc cảm biến 1 qua chuyển đổi điện áp ADC 12-bit...\n");
    printf("          -> Mock: Tín hiệu điện áp ADC = 2048 / 4095 (50%% dải đo).\n");
    float distance = 0.0f;
    mock_sensor1_set_raw_adc(2048);

    esp_sensor_status_t status = sensor1_read_distance(&distance);
    printf("          -> Kết quả: Quy đổi ADC sang khoảng cách = %.2f cm\n", distance);

    TEST_ASSERT_EQUAL_INT(SENSOR_OK, status);
    TEST_ASSERT_FLOAT_WITHIN(1.0f, 200.0f, distance);
}

/* 5. Test giá trị nhỏ hơn dải đo */
void test_sensor1_read_out_of_range_low(void) {
    printf("   [TEST] 5. Đọc cảm biến 1 với khoảng cách quá gần (< 2.0 cm)...\n");
    printf("          -> Mock: Khoảng cách = 1.00 cm (Dưới ngưỡng Min 2.0 cm).\n");
    float distance = 0.0f;
    mock_sensor1_set_distance(1.0f);

    esp_sensor_status_t status = sensor1_read_distance(&distance);
    printf("          -> Kết quả: Bắt lỗi cảnh báo = %d (SENSOR_ERR_OUT_OF_RANGE)\n", status);

    TEST_ASSERT_EQUAL_INT(SENSOR_ERR_OUT_OF_RANGE, status);
}

/* 6. Test giá trị vượt quá dải đo */
void test_sensor1_read_out_of_range_high(void) {
    printf("   [TEST] 6. Đọc cảm biến 1 với khoảng cách quá xa (> 400.0 cm)...\n");
    printf("          -> Mock: Khoảng cách = 450.00 cm (Vượt ngưỡng Max 400.0 cm).\n");
    float distance = 0.0f;
    mock_sensor1_set_distance(450.0f);

    esp_sensor_status_t status = sensor1_read_distance(&distance);
    printf("          -> Kết quả: Bắt lỗi cảnh báo = %d (SENSOR_ERR_OUT_OF_RANGE)\n", status);

    TEST_ASSERT_EQUAL_INT(SENSOR_ERR_OUT_OF_RANGE, status);
}

/* 7. Test xử lý timeout cảm biến */
void test_sensor1_read_timeout_error(void) {
    printf("   [TEST] 7. Kiểm tra xử lý khi cảm biến 1 bị treo (Timeout)...\n");
    printf("          -> Mock: Giả lập không phản hồi bus I2C/UART (Timeout = true).\n");
    float distance = 0.0f;
    mock_sensor1_set_timeout(true);

    esp_sensor_status_t status = sensor1_read_distance(&distance);
    printf("          -> Kết quả: Bắt mã lỗi = %d (SENSOR_ERR_TIMEOUT)\n", status);

    TEST_ASSERT_EQUAL_INT(SENSOR_ERR_TIMEOUT, status);
}

/* 8. Test phát hiện người/vật thể khi khoảng cách trong ngưỡng */
void test_sensor1_detect_object_within_threshold(void) {
    printf("   [TEST] 8. Phát hiện người/vật thể trong ngưỡng cảnh báo (<= 50 cm)...\n");
    printf("          -> Mock: Người đứng cách cảm biến 30.00 cm, Ngưỡng = 50.00 cm.\n");
    mock_sensor1_set_distance(30.0f);
    esp_sensor_status_t status;

    bool detected = sensor1_is_object_detected(50.0f, &status);
    printf("          -> Kết quả: Cờ phát hiện = %s (Đúng kịch bản phát hiện)\n", detected ? "TRUE (Có người)" : "FALSE");

    TEST_ASSERT_EQUAL_INT(SENSOR_OK, status);
    TEST_ASSERT_TRUE(detected);
}

/* 9. Test không phát hiện người khi khoảng cách ngoài ngưỡng */
void test_sensor1_detect_object_outside_threshold(void) {
    printf("   [TEST] 9. Kiểm tra khi không có người trong ngưỡng cảnh báo...\n");
    printf("          -> Mock: Khoảng cách = 80.00 cm, Ngưỡng = 50.00 cm.\n");
    mock_sensor1_set_distance(80.0f);
    esp_sensor_status_t status;

    bool detected = sensor1_is_object_detected(50.0f, &status);
    printf("          -> Kết quả: Cờ phát hiện = %s (Không có vật cản gần)\n", detected ? "TRUE" : "FALSE (An toàn)");

    TEST_ASSERT_EQUAL_INT(SENSOR_OK, status);
    TEST_ASSERT_FALSE(detected);
}

/* 10. Test kiểm tra an toàn con trỏ NULL */
void test_sensor1_read_null_pointer_safety(void) {
    printf("   [TEST] 10. Kiểm tra an toàn bộ nhớ khi truyền con trỏ NULL...\n");
    esp_sensor_status_t status = sensor1_read_distance(NULL);
    printf("          -> Kết quả: Bắt mã lỗi tham số = %d (SENSOR_ERR_INVALID_PARAM)\n", status);
    TEST_ASSERT_EQUAL_INT(SENSOR_ERR_INVALID_PARAM, status);
}
