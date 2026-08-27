# BÁO CÁO KIỂM THỬ ĐƠN VỊ (UNIT TEST REPORT)
## DỰ ÁN HỆ THỐNG KIỂM SOÁT RA VÀO - MÔ-ĐUN ESP32
**Công cụ kiểm thử:** Unity Test Framework (C / Embedded Standard)  
**Thời gian thực hiện:** Tháng 08/2026  
**Người thực hiện / Báo cáo:** Kỹ sư nhúng / Nhóm phát triển hệ thống  
**Tổng số Test Case:** 29 | **Passed:** 29 (100%) | **Failed:** 0 | **Ignored:** 0  

---

## I. MỤC TIÊU KIỂM THỬ
1. Tích hợp framework **Unity** vào dự án ESP để tự động hóa kiểm thử mã nguồn C.
2. Xây dựng và thực thi các kịch bản kiểm thử cho:
   - **Chức năng 1:** Đọc giá trị Cảm biến 1 (Cảm biến khoảng cách / phát hiện vật cản).
   - **Chức năng 2:** Đọc giá trị Cảm biến 2 (Cảm biến trạng thái cửa / công tắc từ bảo mật).
   - **Chức năng 3 & 4:** Kết nối truyền thông không dây BLE và WiFi.
3. Sử dụng kỹ thuật giả lập phần cứng (**Mocking / Stubbing data**) để kiểm tra toàn diện các trường hợp: hoạt động bình thường, dữ liệu biên, vượt dải đo, lỗi mất kết nối, timeout và cơ chế tự phục hồi.

---

## II. BẢNG CHI TIẾT CÁC TEST CASE VÀ KẾT QUẢ KIỂM THỬ

### 1. Nhóm kiểm thử: Cảm biến 1 (Khoảng cách / Vật cản)
| Mã TC | Tên Hàm Test | Kịch bản / Điều kiện kiểm thử | Dữ liệu giả lập (Mock Data) | Kết quả mong đợi | Kết quả thực tế | Trạng thái |
|:---:|:---|:---|:---|:---|:---|:---:|
| **TC_S1_01** | `test_sensor1_init_success` | Khởi tạo cảm biến 1 khi phần cứng sẵn sàng | Hardware bình thường | Trả về `SENSOR_OK` | `SENSOR_OK` | **PASS** |
| **TC_S1_02** | `test_sensor1_init_hardware_fault` | Khởi tạo khi cảm biến bị hỏng/đứt dây | `mock_sensor1_set_fault(true)` | Trả về `SENSOR_ERR_HARDWARE_FAULT` | `SENSOR_ERR_HARDWARE_FAULT` | **PASS** |
| **TC_S1_03** | `test_sensor1_read_normal_valid_distance` | Đọc khoảng cách trong dải đo chuẩn | Mock khoảng cách = $25.5\text{ cm}$ | Trả về `SENSOR_OK`, giá trị $25.5\text{ cm}$ | `SENSOR_OK`, $25.5000\text{ cm}$ | **PASS** |
| **TC_S1_04** | `test_sensor1_read_adc_conversion` | Chuyển đổi ADC 12-bit thô sang khoảng cách | Mock raw ADC = $2048$ ($50\%$) | Tính toán khoảng cách $\approx 200\text{ cm}$ | $\approx 200.04\text{ cm}$ | **PASS** |
| **TC_S1_05** | `test_sensor1_read_out_of_range_low` | Khoảng cách nhỏ hơn dải đo tối thiểu ($< 2\text{cm}$) | Mock khoảng cách = $1.0\text{ cm}$ | Bắt lỗi `SENSOR_ERR_OUT_OF_RANGE` | `SENSOR_ERR_OUT_OF_RANGE` | **PASS** |
| **TC_S1_06** | `test_sensor1_read_out_of_range_high` | Khoảng cách vượt quá dải đo tối đa ($> 400\text{cm}$) | Mock khoảng cách = $450.0\text{ cm}$ | Bắt lỗi `SENSOR_ERR_OUT_OF_RANGE` | `SENSOR_ERR_OUT_OF_RANGE` | **PASS** |
| **TC_S1_07** | `test_sensor1_read_timeout_error` | Xử lý khi cảm biến bị treo không phản hồi | `mock_sensor1_set_timeout(true)` | Trả về `SENSOR_ERR_TIMEOUT` | `SENSOR_ERR_TIMEOUT` | **PASS** |
| **TC_S1_08** | `test_sensor1_detect_object_within_threshold` | Phát hiện người/vật thể trong ngưỡng cảnh báo | Mock khoảng cách $30\text{cm} \le 50\text{cm}$ | Cờ `detected = true` | `true` | **PASS** |
| **TC_S1_09** | `test_sensor1_detect_object_outside_threshold` | Không kích hoạt khi vật thể nằm ngoài ngưỡng | Mock khoảng cách $80\text{cm} > 50\text{cm}$ | Cờ `detected = false` | `false` | **PASS** |
| **TC_S1_10** | `test_sensor1_read_null_pointer_safety` | An toàn bộ nhớ khi truyền con trỏ NULL | Truyền con trỏ `NULL` | Trả về `SENSOR_ERR_INVALID_PARAM` | `SENSOR_ERR_INVALID_PARAM` | **PASS** |

---

### 2. Nhóm kiểm thử: Cảm biến 2 (Trạng thái Cửa / Công tắc từ)
| Mã TC | Tên Hàm Test | Kịch bản / Điều kiện kiểm thử | Dữ liệu giả lập (Mock Data) | Kết quả mong đợi | Kết quả thực tế | Trạng thái |
|:---:|:---|:---|:---|:---|:---|:---:|
| **TC_S2_01** | `test_sensor2_init_success` | Khởi tạo cảm biến cửa thành công | Khởi tạo bình thường | Trả về `SENSOR_OK` | `SENSOR_OK` | **PASS** |
| **TC_S2_02** | `test_sensor2_read_door_closed` | Đọc trạng thái khi cửa đóng an toàn | Mock GPIO Level = $0$ | `DOOR_STATE_CLOSED`, `secure = true` | `DOOR_STATE_CLOSED`, `secure = true` | **PASS** |
| **TC_S2_03** | `test_sensor2_read_door_open` | Đọc trạng thái khi cửa bị mở | Mock GPIO Level = $1$ | `DOOR_STATE_OPEN`, `secure = false` | `DOOR_STATE_OPEN`, `secure = false` | **PASS** |
| **TC_S2_04** | `test_sensor2_read_tamper_alert` | Cảnh báo khi có hành vi cạy phá/tháo gỡ cảm biến | `mock_tamper = true` | Báo trạng thái `DOOR_STATE_TAMPER` | `DOOR_STATE_TAMPER` | **PASS** |
| **TC_S2_05** | `test_sensor2_read_hardware_fault` | Xử lý sự cố mất tín hiệu cảm biến cửa | `mock_fault = true` | Trả về `SENSOR_ERR_HARDWARE_FAULT` | `SENSOR_ERR_HARDWARE_FAULT` | **PASS** |
| **TC_S2_06** | `test_sensor2_debounce_filter_noise` | Thuật toán Debounce khử xung nhiễu tín hiệu | Chuỗi xung nhiễu `[0, 0, 1, 0, 0]` | Khử nhiễu thành công: `CLOSED` | `DOOR_STATE_CLOSED` | **PASS** |
| **TC_S2_07** | `test_sensor2_read_null_pointer_safety` | An toàn con trỏ NULL khi đọc trạng thái | Truyền con trỏ `NULL` | Trả về `SENSOR_ERR_INVALID_PARAM` | `SENSOR_ERR_INVALID_PARAM` | **PASS** |

---

### 3. Nhóm kiểm thử: Kết nối không dây BLE & WiFi
| Mã TC | Tên Hàm Test | Kịch bản / Điều kiện kiểm thử | Dữ liệu giả lập (Mock Data) | Kết quả mong đợi | Kết quả thực tế | Trạng thái |
|:---:|:---|:---|:---|:---|:---|:---:|
| **TC_NET_01** | `test_ble_init_success` | Khởi tạo Stack BLE với tên thiết bị | Tên `"ESP32_AccessControl"` | Khởi tạo xong ở chế độ chờ | `CONN_STATUS_DISCONNECTED` | **PASS** |
| **TC_NET_02** | `test_ble_connect_success` | Kết nối BLE tới đúng địa chỉ MAC đích | MAC `"B8:27:EB:12:34:56"` | Trả về `CONN_STATUS_CONNECTED` | `CONN_STATUS_CONNECTED` | **PASS** |
| **TC_NET_03** | `test_ble_connect_auth_fail_wrong_mac` | Kết nối thất bại do sai địa chỉ MAC | Sai MAC `"FF:FF:FF:00:00:00"` | Bắt lỗi `CONN_STATUS_AUTH_FAIL` | `CONN_STATUS_AUTH_FAIL` | **PASS** |
| **TC_NET_04** | `test_ble_connect_peer_unavailable_timeout` | Kết nối Timeout khi thiết bị ngoài vùng sóng | `peer_available = false` | Bắt lỗi `CONN_STATUS_TIMEOUT` | `CONN_STATUS_TIMEOUT` | **PASS** |
| **TC_NET_05** | `test_ble_send_rfid_card_payload` | Gửi dữ liệu quẹt thẻ RFID qua BLE topic | Topic `rfid/scan`, UID `E20000190108` | Gói tin được đẩy lên GATT buffer | Gửi thành công | **PASS** |
| **TC_NET_06** | `test_ble_send_data_when_disconnected` | Chặn gửi dữ liệu khi chưa kết nối BLE | Trạng thái đang ngắt kết nối | Trả về `CONN_STATUS_DISCONNECTED` | `CONN_STATUS_DISCONNECTED` | **PASS** |
| **TC_NET_07** | `test_ble_receive_unlock_command` | Nhận lệnh điều khiển mở cửa từ BLE | Giả lập gói tin `door/command=UNLOCK` | Đọc đúng topic và nội dung lệnh | Nhận chính xác `UNLOCK_DOOR` | **PASS** |
| **TC_NET_08** | `test_ble_auto_reconnect_mechanism` | Tự động kết nối lại khi bị rớt sóng | Thử lại tối đa 3 lần sau rớt sóng | Kết nối lại thành công | `CONN_STATUS_CONNECTED` | **PASS** |
| **TC_NET_09** | `test_wifi_connect_success` | Kết nối WiFi với đúng SSID & Password | SSID `"AccessControl_AP"`, Pass `"12345678"` | Kết nối WiFi thành công | `CONN_STATUS_CONNECTED` | **PASS** |
| **TC_NET_10** | `test_wifi_connect_auth_fail` | Kết nối WiFi thất bại do sai mật khẩu | Sai mật khẩu `"wrong_pass_999"` | Bắt lỗi `CONN_STATUS_AUTH_FAIL` | `CONN_STATUS_AUTH_FAIL` | **PASS** |
| **TC_NET_11** | `test_wifi_connect_ap_not_found` | Kết nối WiFi khi Access Point không tồn tại | SSID lạ `"Unknown_WiFi_SSID"` | Bắt lỗi `CONN_STATUS_TIMEOUT` | `CONN_STATUS_TIMEOUT` | **PASS** |
| **TC_NET_12** | `test_wifi_send_telemetry_data` | Gửi log sự kiện cửa qua WiFi JSON endpoint | Endpoint `/api/v1/door/event` | Gửi thành công dữ liệu Telemetry | `CONN_STATUS_CONNECTED` | **PASS** |

---

## III. LOG KẾT QUẢ THỰC THI THỰC TẾ (CONSOLE LOG)

```
--------------------------------------------------
UNITY TEST EXECUTION: test/test_main.c
--------------------------------------------------

>>> =================================================== <<<
>>> NHÓM TEST 1: ĐỌC GIÁ TRỊ CẢM BIẾN 1 (KHOẢNG CÁCH)  <<<
>>> =================================================== <<<
test/test_main.c:40:test_sensor1_init_success:PASS
test/test_main.c:41:test_sensor1_init_hardware_fault:PASS
test/test_main.c:42:test_sensor1_read_normal_valid_distance:PASS
test/test_main.c:43:test_sensor1_read_adc_conversion:PASS
test/test_main.c:44:test_sensor1_read_out_of_range_low:PASS
test/test_main.c:45:test_sensor1_read_out_of_range_high:PASS
test/test_main.c:46:test_sensor1_read_timeout_error:PASS
test/test_main.c:47:test_sensor1_detect_object_within_threshold:PASS
test/test_main.c:48:test_sensor1_detect_object_outside_threshold:PASS
test/test_main.c:49:test_sensor1_read_null_pointer_safety:PASS

>>> =================================================== <<<
>>> NHÓM TEST 2: ĐỌC GIÁ TRỊ CẢM BIẾN 2 (TRẠNG THÁI CỬA)<<<
>>> =================================================== <<<
test/test_main.c:54:test_sensor2_init_success:PASS
test/test_main.c:55:test_sensor2_read_door_closed:PASS
test/test_main.c:56:test_sensor2_read_door_open:PASS
test/test_main.c:57:test_sensor2_read_tamper_alert:PASS
test/test_main.c:58:test_sensor2_read_hardware_fault:PASS
test/test_main.c:59:test_sensor2_debounce_filter_noise:PASS
test/test_main.c:60:test_sensor2_read_null_pointer_safety:PASS

>>> =================================================== <<<
>>> NHÓM TEST 3: KẾT NỐI KHÔNG DÂY (BLE & WIFI)         <<<
>>> =================================================== <<<
test/test_main.c:65:test_ble_init_success:PASS
test/test_main.c:66:test_ble_connect_success:PASS
test/test_main.c:67:test_ble_connect_auth_fail_wrong_mac:PASS
test/test_main.c:68:test_ble_connect_peer_unavailable_timeout:PASS
test/test_main.c:69:test_ble_send_rfid_card_payload:PASS
test/test_main.c:70:test_ble_send_data_when_disconnected:PASS
test/test_main.c:71:test_ble_receive_unlock_command:PASS
test/test_main.c:72:test_ble_auto_reconnect_mechanism:PASS
test/test_main.c:73:test_wifi_connect_success:PASS
test/test_main.c:74:test_wifi_connect_auth_fail:PASS
test/test_main.c:75:test_wifi_connect_ap_not_found:PASS
test/test_main.c:76:test_wifi_send_telemetry_data:PASS
--------------------------------------------------
29 Tests 0 Failures 0 Ignored
OK - ALL TESTS PASSED!
--------------------------------------------------
```

---

## IV. ĐÁNH GIÁ VÀ KẾT LUẬN
1. **Tính độc lập & an toàn:** Bộ kiểm thử Unity chạy độc lập qua cơ chế Mocking, không can thiệp hay ảnh hưởng đến bất kỳ file nguồn nào của ứng dụng giao diện Qt chính.
2. **Độ bao phủ (Coverage):**
   - Đạt độ bao phủ đầy đủ các luồng xử lý: luồng chuẩn (Happy path), luồng lỗi phần cứng, luồng lỗi kết nối mạng và dữ liệu biên/ngoài phạm vi.
   - Thuật toán khử rung (Debounce) và cơ chế tự động kết nối lại (Auto Reconnect) hoạt động chính xác theo đúng yêu cầu thiết kế.
3. **Đánh giá chung:** Toàn bộ **29/29 test cases đạt kết quả PASS (100%)**, sẵn sàng nộp báo cáo và triển khai trên vi điều khiển ESP thực tế.
