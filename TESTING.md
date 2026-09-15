# QUY TRÌNH VÀ TÀI LIỆU KIỂM THỬ HỆ THỐNG (TESTING GUIDE)
## Dự án: Hệ Thống Kiểm Soát Ra Vào (Raspberry Pi + ESP32 BLE)
*Tài liệu kỹ thuật tổng hợp quy trình kiểm định chất lượng phần mềm bám sát giáo trình "Bài 11 - Quy trình test phần mềm".*

---

## 1. Mục tiêu kiểm thử
Mục tiêu cốt lõi của công tác kiểm thử trong dự án bao gồm:
1. **Kiểm tra tính đúng đắn chức năng (Correctness):** Xác minh toàn bộ các module nghiệp vụ (Cơ sở dữ liệu SQLite, Hệ thống cấu hình, Phân tích gói tin truyền thông BLE) hoạt động chính xác theo đặc tả.
2. **Kiểm tra độ tin cậy liên lạc (Reliability & Interoperability):** Xác minh giao tiếp không dây thời gian thực giữa Raspberry Pi (Linux/BlueZ) và vi điều khiển ESP32 qua BLE GATT.
3. **Phòng ngừa lỗi hồi quy (Regression Prevention):** Cung cấp bộ công cụ kiểm thử tự động để kiểm tra tức thì mỗi khi refactor hoặc nâng cấp mã nguồn.
4. **Đảm bảo an toàn phần cứng (Hardware Safety):** Đảm bảo cơ chế đóng/mở cửa của động cơ Servo và cảm biến an toàn LM393 vận hành chính xác, tránh kẹt cửa hoặc gây nguy hiểm cho người qua lại.

---

## 2. Cơ sở kiểm thử
Hệ thống kiểm thử được thiết lập dựa trên:
- **Cơ sở học thuật:** Nội dung bài giảng **"Bài 11 - Quy trình test phần mềm"** với 4 mức kiểm thử chuẩn:
  `Unit Test` $\rightarrow$ `Integration Test` $\rightarrow$ `System Test` $\rightarrow$ `Acceptance Test`.
- **Cơ sở thực tế:** Kiến trúc mã nguồn thực tế và phần cứng vật lý của hệ thống:
  - Ứng dụng Qt6 C++ chạy trên Raspberry Pi 4 Model B (Debian GNU/Linux 12 bookworm).
  - Firmware ESP32 phát triển trên nền tảng PlatformIO (Arduino framework + NimBLE-Arduino).
  - Các linh kiện ngoại vi: Đầu đọc RFID RC522, Màn hình OLED SSD1306, Cảm biến vật cản LM393, Còi buzzer chủ động, Động cơ Servo SG90.

---

## 3. Kiến trúc kiểm thử phân tầng

```
               ┌──────────────────────────────────────────────┐
               │         4. Acceptance Test (Nghiệm thu)      │
               │   tests/acceptance/ACCEPTANCE_TEST_CASES.md   │
               └──────────────────────┬───────────────────────┘
                                      │
               ┌──────────────────────▼───────────────────────┐
               │           3. System Test (Hệ thống)          │
               │       tests/system/SYSTEM_TEST_CASES.md      │
               └──────────────────────┬───────────────────────┘
                                      │
               ┌──────────────────────▼───────────────────────┐
               │       2. Integration Test (Tích hợp BLE)     │
               │     tests/integration/test_ble_esp32.cpp     │
               │          (Kịch bản IT-01 đến IT-08)          │
               └──────────────────────┬───────────────────────┘
                                      │
        ┌─────────────────────────────┴─────────────────────────────┐
        │                                                           │
┌───────▼────────────────────────────┐    ┌─────────────────────────▼───────────────────────────┐
│     1.1. Qt Unit Test (C++)        │    │          1.2. ESP32 Unit/Hardware Test              │
│  - test_cosodulieu.cpp             │    │  - test_ble_protocol.cpp (Parser frame)             │
│  - test_cauhinhhethong.cpp         │    │  - test_door.cpp (Servo cửa SG90)                   │
│  - test_ble_protocol.cpp           │    │  - test_sensor.cpp (Cảm biến quang LM393)           │
│  (Sử dụng QtTest Library 6.5.1)    │    │  - test_rfid.cpp (Đầu đọc thẻ RC522 - Unity)        │
└────────────────────────────────────┘    └─────────────────────────────────────────────────────┘
```

---

## 4. Qt Unit Test (Phía Raspberry Pi / Máy phát triển)
- **Vị trí mã nguồn:** `tests/unit/`
- **Framework kiểm thử:** `Qt Test (Qt6::Test)`
- **Các bộ kiểm thử:**
  1. `tests/unit/database/test_cosodulieu.cpp`: Kiểm tra toàn bộ CRUD tài khoản đăng nhập, bảo mật mật khẩu SHA-256, kiểm tra tính duy nhất của UID thẻ RFID, lưu vết lịch sử và lọc đa điều kiện.
  2. `tests/unit/config/test_cauhinhhethong.cpp`: Kiểm tra cấu hình mặc định (15s), kiểm tra các giá trị biên (Min 1s, Max 300s), kiểm tra từ chối giá trị âm hoặc 0s, kiểm tra chu trình lưu và đọc lại từ file JSON.
  3. `tests/unit/ble_protocol/test_ble_protocol.cpp`: Kiểm tra bộ phân tích cú pháp frame dạng `TOPIC|PAYLOAD`, kiểm tra frame nhiều dòng (`\n`), từ chối frame thiếu ký tự phân tách `|`, từ chối topic rỗng, kiểm tra đóng gói frame dữ liệu chuẩn.
- **Lệnh thực thi trên Pi:**
  ```bash
  export LD_LIBRARY_PATH=/usr/local/qt6/lib:/usr/local/lib
  cd /home/pi/he-thong-kiem-soat-ra-vao/tests_bin
  mkdir -p run_db && cd run_db && ../test_cosodulieu && cd ..
  mkdir -p run_cfg && cd run_cfg && ../test_cauhinhhethong && cd ..
  ./test_ble_protocol
  ```

---

## 5. ESP32 Unit / Hardware Test (Phía Firmware)
- **Vị trí mã nguồn:** `esp32/test/` (tương ứng `RFID/test/`)
- **Framework kiểm thử:** `PlatformIO Unity Test Framework`
- **Yêu cầu phần cứng:** Mạch ESP32 kết nối cổng USB với máy tính Windows, nối dây các linh kiện RC522 (SPI), Servo (GPIO 13), LM393 (GPIO 34), Buzzer (GPIO 25), SSD1306 (I2C).
- **Các bộ kiểm thử:**
  - `test_ble_protocol`: Phân tích frame và validate dữ liệu đến từ Pi.
  - `test_door`: Điều khiển góc quay và trạng thái mở/đóng của Servo.
  - `test_sensor`: Đọc trạng thái cảm biến quang LM393 và phát hiện người.
  - `test_rfid`: Đọc thanh ghi phiên bản RC522, kiểm tra anten và quét thẻ.
- **Lệnh thực thi:**
  ```bash
  pio test -e esp32dev
  ```

---

## 6. Integration Test (Hệ thống thực tế Raspberry Pi ↔ ESP32)
- **Vị trí mã nguồn:** `tests/integration/test_ble_esp32.cpp`
- **Môi trường thực thi:** Chạy trực tiếp trên Raspberry Pi kết nối không dây tới ESP32 thật qua giao thức Bluetooth Low Energy.
- **Bám sát 8 kịch bản tích hợp chuẩn theo Slide Bài 11:**
  1. **IT-01 - Scan:** Qt/Pi quét sóng và nhận diện đúng thiết bị `HE THONG RA VAO HIEU` với địa chỉ MAC `44:1D:64:F5:01:9E`.
  2. **IT-02 - Connect:** Thiết lập kết nối GATT, khám phá thành công Service `7d9a0001-...` và Characteristic Notify / Write.
  3. **IT-03 - Send:** Pi truyền lệnh mở cửa `access/door/command|OPEN` xuống ESP32.
  4. **IT-04 - Receive:** Nhận thông báo sự kiện (Notify) thời gian thực từ ESP32 gửi lên Pi.
  5. **IT-05 - Verify Data:** Xác minh tính toàn vẹn của dữ liệu gửi và nhận theo chuẩn `TOPIC|PAYLOAD`.
  6. **IT-06 - Timeout:** Kiểm tra khả năng xử lý bất đồng bộ khi thiết bị ngoại vi không phản hồi (đảm bảo ứng dụng Qt không bao giờ bị đơ/freeze).
  7. **IT-07 - Disconnect:** Chủ động ngắt kết nối BLE, kiểm tra trạng thái an toàn của hệ thống.
  8. **IT-08 - Reconnect:** Kích hoạt lại cơ chế quét và kết nối lại tự động với ESP32.
- **Lệnh thực thi trên Pi:**
  ```bash
  export LD_LIBRARY_PATH=/usr/local/qt6/lib:/usr/local/lib
  cd /home/pi/he-thong-kiem-soat-ra-vao/tests_bin
  ./test_ble_esp32
  ```

---

## 7. Bằng chứng kiểm thử (Test Evidence)
Mọi kết quả kiểm thử đều được thực thi thật 100% trên phần cứng và lưu vết đầy đủ trong thư mục `test_evidence/`:
```
test_evidence/
├── qt/
│   ├── unit/
│   │   ├── test_cosodulieu.log        # Log thực tế chạy 13 test case CSDL (13/13 PASS)
│   │   ├── test_cauhinhhethong.log    # Log thực tế chạy 8 test case cấu hình (8/8 PASS)
│   │   └── test_ble_protocol.log      # Log thực tế chạy 12 test case protocol (12/12 PASS)
│   └── integration/
│       └── integration_IT01_IT08.log  # Log thực tế chạy 8 kịch bản IT-01 đến IT-08 (PASS)
└── TEST_REPORT.md                     # Báo cáo tổng hợp kết quả nghiệm thu kiểm thử
```

---

## 8. Báo cáo kiểm thử (Test Report)
Tài liệu `test_evidence/TEST_REPORT.md` trình bày chi tiết thông số môi trường phần cứng, bảng tổng hợp số liệu test case (Total, Passed, Failed, Waiting for Manual Action) và chi tiết từng kịch bản kiểm thử phục vụ báo cáo đồ án.
