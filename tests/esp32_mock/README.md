# HƯỚNG DẪN BỘ KIỂM THỬ MÔ PHỎNG ESP32

Thư mục `tests/esp32_mock/` là bộ kiểm thử C chạy trên máy phát triển. Nó dùng mock/stub và **không phải source firmware đang nạp trên ESP32**.

Một số test Wi-Fi được giữ lại như test kế thừa để tham khảo. Hệ thống Qt/ESP32 đang vận hành thực tế hiện dùng BLE-only.

---

## 1. Cấu trúc thư mục kiểm thử
```
tests/esp32_mock/
├── unity/                 # Thư viện Unit Test chuẩn của ESP-IDF / C
│   ├── unity.h
│   ├── unity.c
│   └── unity_internals.h
├── include/               # Khai báo hàm ESP & hàm Mock dữ liệu
│   ├── esp_sensors.h      # Cảm biến 1 & Cảm biến 2
│   └── esp_connectivity.h # Kết nối BLE & WiFi
├── src/                   # Triển khai hàm xử lý & cơ chế giả lập (Mock/Stub)
│   ├── esp_sensors.c
│   └── esp_connectivity.c
├── test/                  # Các kịch bản Test Case
│   ├── test_sensor1.c     # 10 test case cho Cảm biến 1
│   ├── test_sensor2.c     # 7 test case cho Cảm biến 2
│   ├── test_connectivity.c# 12 test case cho BLE & WiFi
│   └── test_main.c        # Hàm main() thực thi toàn bộ test
├── Makefile               # File build tự động với GCC
└── run_tests.sh           # Script 1-click chạy test
```

---

## 2. Cách chạy Test trên Terminal

### Cách 1: Chạy bằng Script 1-click (Khuyên dùng)
Mở Terminal tại thư mục `tests/esp32_mock` và gõ:
```bash
cd /home/pi/du_an/HeThongKiemSoatRaVao/Hethongkiemsoatravao/tests/esp32_mock
chmod +x run_tests.sh
./run_tests.sh
```

### Cách 2: Chạy bằng Makefile / gcc
```bash
cd /home/pi/du_an/HeThongKiemSoatRaVao/Hethongkiemsoatravao/tests/esp32_mock
make clean
make test
```

### Cách 3: Biên dịch thủ công bằng lệnh GCC trực tiếp
```bash
cd /home/pi/du_an/HeThongKiemSoatRaVao/Hethongkiemsoatravao/tests/esp32_mock
gcc -Wall -Iunity -Iinclude unity/unity.c src/*.c test/*.c -o run_esp_tests
./run_esp_tests
```

---

## 3. Danh sách các Test Case đã được xây dựng

### Nhóm 1: Đọc giá trị Cảm biến 1 (Khoảng cách / Vật cản)
| STT | Test Function | Mô tả kịch bản | Dữ liệu giả (Mock) |
|---|---|---|---|
| 1 | `test_sensor1_init_success` | Khởi tạo cảm biến 1 thành công | Trạng thái bình thường |
| 2 | `test_sensor1_init_hardware_fault` | Báo lỗi khi phần cứng cảm biến hỏng | `mock_sensor1_set_fault(true)` |
| 3 | `test_sensor1_read_normal_valid_distance` | Đọc khoảng cách hợp lệ trong dải đo | Mock khoảng cách 25.5 cm |
| 4 | `test_sensor1_read_adc_conversion` | Chuyển đổi ADC 12-bit sang khoảng cách | Mock raw ADC = 2048 (~200 cm) |
| 5 | `test_sensor1_read_out_of_range_low` | Giá trị dưới dải đo cho phép (< 2cm) | Mock khoảng cách 1.0 cm |
| 6 | `test_sensor1_read_out_of_range_high` | Giá trị vượt dải đo cho phép (> 400cm) | Mock khoảng cách 450.0 cm |
| 7 | `test_sensor1_read_timeout_error` | Xử lý khi cảm biến bị treo/timeout | `mock_sensor1_set_timeout(true)` |
| 8 | `test_sensor1_detect_object_within_threshold` | Phát hiện người trong ngưỡng cảnh báo (<=50cm) | Mock 30.0 cm -> `detected = true` |
| 9 | `test_sensor1_detect_object_outside_threshold` | Không kích hoạt khi vật thể ngoài ngưỡng | Mock 80.0 cm -> `detected = false` |
| 10| `test_sensor1_read_null_pointer_safety` | Kiểm tra an toàn khi truyền con trỏ NULL | Truyền NULL -> Trả về mã lỗi an toàn |

---

### Nhóm 2: Đọc giá trị Cảm biến 2 (Trạng thái Cửa / Công tắc từ)
| STT | Test Function | Mô tả kịch bản | Dữ liệu giả (Mock) |
|---|---|---|---|
| 1 | `test_sensor2_init_success` | Khởi tạo cảm biến cửa thành công | Khởi tạo bình thường |
| 2 | `test_sensor2_read_door_closed` | Cửa đóng an toàn (GPIO 0) | Mock GPIO = 0 -> `DOOR_STATE_CLOSED` |
| 3 | `test_sensor2_read_door_open` | Cửa mở (GPIO 1) | Mock GPIO = 1 -> `DOOR_STATE_OPEN` |
| 4 | `test_sensor2_read_tamper_alert` | Cảnh báo khi có hành vi cạy cửa/phá hoại | Mock `tamper = true` -> `DOOR_STATE_TAMPER` |
| 5 | `test_sensor2_read_hardware_fault` | Báo lỗi khi mất tín hiệu cảm biến | Mock `fault = true` -> `HARDWARE_FAULT` |
| 6 | `test_sensor2_debounce_filter_noise` | Thuật toán Debounce khử xung nhiễu giả | Chuỗi xung nhiễu `[0, 0, 1, 0, 0]` -> Cửa vẫn đóng |
| 7 | `test_sensor2_read_null_pointer_safety` | An toàn bộ nhớ khi truyền con trỏ NULL | Truyền NULL -> Báo lỗi tham số |

---

### Nhóm 3: Kết nối không dây (BLE & WiFi)
| STT | Test Function | Mô tả kịch bản | Dữ liệu giả (Mock) |
|---|---|---|---|
| 1 | `test_ble_init_success` | Khởi tạo driver BLE thành công | Tên thiết bị `"ESP32_Device"` |
| 2 | `test_ble_connect_success` | Kết nối BLE thành công tới MAC Raspberry Pi | MAC `"B8:27:EB:12:34:56"` |
| 3 | `test_ble_connect_auth_fail_wrong_mac` | Kết nối thất bại do sai địa chỉ MAC | Sai MAC -> Trả về `AUTH_FAIL` |
| 4 | `test_ble_connect_peer_unavailable_timeout` | Kết nối timeout khi thiết bị ngoài vùng sóng | Mock `peer_available = false` -> `TIMEOUT` |
| 5 | `test_ble_send_rfid_card_payload` | Gửi dữ liệu quét thẻ RFID qua BLE topic `rfid/scan` | Quét thẻ UID `"E200001901080123"` |
| 6 | `test_ble_send_data_when_disconnected` | Bắt lỗi gửi dữ liệu khi chưa kết nối BLE | Trạng thái ngắt kết nối -> `DISCONNECTED` |
| 7 | `test_ble_receive_unlock_command` | Nhận lệnh điều khiển mở cửa từ BLE | Mock gói tin `door/command` = `"UNLOCK_DOOR"` |
| 8 | `test_ble_auto_reconnect_mechanism` | Tự động kết nối lại BLE sau khi rớt mạng | Thử kết nối lại thành công sau retry |
| 9 | `test_wifi_connect_success` | Kết nối WiFi đúng SSID & Mật khẩu | SSID `"AccessControl_AP"`, Pass `"12345678"` |
| 10| `test_wifi_connect_auth_fail` | Kết nối WiFi sai mật khẩu | Sai password -> `AUTH_FAIL` |
| 11| `test_wifi_connect_ap_not_found` | Kết nối WiFi khi không tìm thấy AP | Không thấy SSID -> `TIMEOUT` |
| 12| `test_wifi_send_telemetry_data` | Gửi log sự kiện cửa qua WiFi | Gửi JSON payload thành công |
