# HƯỚNG DẪN KIỂM THỬ FIRMWARE ESP32 (PLATFORMIO & UNITY)
## Dự án: Hệ thống kiểm soát ra vào (Phần cứng ESP32)
*Cơ sở tài liệu: Bám sát nội dung slide "Bài 11 - Quy trình test phần mềm" (Mục: Test có phần cứng & Unit Test với Unity Framework).*

---

## 1. Mục đích của bộ test ESP32
Bộ kiểm thử firmware ESP32 được xây dựng nhằm:
- Đảm bảo tính độc lập và chính xác của từng module ngoại vi (RFID RC522, Cảm biến vật cản LM393, Servo điều khiển góc mở/đóng cửa, Giao thức BLE).
- Tuân thủ phương pháp "Test có phần cứng" được hướng dẫn trong giáo trình Bài 11 bằng việc tách các bộ phận thành các hàm chức năng và kiểm thử trực tiếp trên phần cứng thật.

---

## 2. Cấu trúc thư mục kiểm thử ESP32
```
RFID/ (hoặc esp32/)
├── platformio.ini                     # Cấu hình PlatformIO môi trường esp32dev
├── include/                           # Header các module (rfid, sensor, servo, ble...)
├── src/                               # Triển khai source code các module và main.cpp
└── test/                              # BỘ KIỂM THỬ FIRMWARE
    ├── README_TEST.md                 # Tài liệu này
    ├── test_ble_protocol/             # Kiểm thử phân tích frame BLE (Pure logic / Host hoặc Hardware)
    │   └── test_ble_protocol.cpp
    ├── test_door/                     # Kiểm thử điều khiển góc Servo cửa
    │   └── test_door.cpp
    ├── test_sensor/                   # Kiểm thử cảm biến quang phát hiện người LM393
    │   └── test_sensor.cpp
    └── test_rfid/                     # Kiểm thử đầu đọc thẻ từ RC522
        └── test_rfid.cpp
```

---

## 3. Danh mục các module kiểm thử

| Tên suite test | Module kiểm thử | Linh kiện phần cứng | Ghi chú |
| :--- | :--- | :--- | :--- |
| `test_ble_protocol` | Parser frame `TOPIC\|PAYLOAD` | Không bắt buộc | Chạy giả lập logic hoặc trực tiếp trên ESP32 |
| `test_door` | Điều khiển Servo SG90 | Servo SG90 (GPIO 13) | Kiểm tra góc quay 0° (đóng) và 90° (mở) |
| `test_sensor` | Cảm biến vật cản LM393 | Module LM393 (GPIO 34) | Kiểm tra ngắt mức LOW/HIGH và debounce |
| `test_rfid` | Đầu đọc RFID MFRC522 | RC522 qua giao tiếp SPI | Kiểm tra khởi tạo thanh ghi và đọc UID thẻ |

---

## 4. Hướng dẫn chạy test trên PlatformIO (Windows / VS Code)

Do board ESP32 kết nối cổng USB với máy tính Windows (đường dẫn: `C:\Users\admin\Documents\PlatformIO\Projects\RFID`), việc chạy test được thực hiện thông qua PlatformIO CLI:

### 4.1. Chạy từng bộ test riêng biệt
Mở Terminal tại thư mục project trên Windows:
```bash
# Chạy test giao thức BLE:
pio test -e esp32dev -f test_ble_protocol

# Chạy test điều khiển Servo cửa:
pio test -e esp32dev -f test_door

# Chạy test cảm biến vật cản LM393:
pio test -e esp32dev -f test_sensor

# Chạy test đầu đọc thẻ RFID:
pio test -e esp32dev -f test_rfid
```

### 4.2. Chạy toàn bộ các bài test
```bash
pio test -e esp32dev
```

---

## 5. Xử lý trạng thái "WAITING FOR MANUAL ACTION"
Khi chạy các bài test ngoại vi có sự can thiệp của người dùng:
1. **Bài test cảm biến `test_sensor`**:
   - Màn hình Serial in: `WAITING FOR MANUAL ACTION: Please trigger the LM393 sensor (place hand in front)`.
   - Người kiểm thử đưa tay che trước cảm biến LM393 trong vòng 5 giây.
2. **Bài test quẹt thẻ `test_rfid`**:
   - Màn hình Serial in: `WAITING FOR MANUAL ACTION: Please scan RFID card now`.
   - Người kiểm thử áp thẻ RFID vào bề mặt anten RC522.
3. Nếu không có thao tác vật lý trong thời gian đếm ngược, bài kiểm thử ghi nhận thông báo `WAITING FOR MANUAL ACTION` và tiếp tục mà không làm crash chương trình.
