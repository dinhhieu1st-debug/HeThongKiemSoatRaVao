# HƯỚNG DẪN CHI TIẾT DEMO KIỂM THỬ CHO GIẢNG VIÊN
## Đề tài: Hệ Thống Kiểm Soát Ra Vào (Raspberry Pi 4 + ESP32 BLE)
*Cơ sở tài liệu: Bám sát 100% nội dung slide "Bài 11 - Quy trình test phần mềm".*

---

## 1. CẤU TRÚC HỆ THỐNG TEST TRONG REPOSITORY

Toàn bộ hệ thống kiểm thử được tổ chức phân cấp rõ ràng theo kim tự tháp kiểm thử phần mềm:

```
HeThongKiemSoatRaVao/
├── huongdan.md                               # Hướng dẫn chi tiết quy trình demo cho thầy cô (File này)
├── TESTING.md                                # Tài liệu tổng quan quy chuẩn test bám sát Bài 11
├── TEST_PLAN.md                              # Kế hoạch & ma trận test case chi tiết
│
├── tests/                                    # BỘ KIỂM THỬ PHÍA QT / C++
│   ├── CMakeLists.txt                        # Cấu hình biên dịch các target test bằng Qt6Test
│   ├── README_TEST.md                        # Hướng dẫn kỹ thuật chạy bộ test Qt
│   │
│   ├── unit/                                 # [MỨC 1] KIỂM THỬ ĐƠN VỊ (UNIT TEST)
│   │   ├── database/
│   │   │   └── test_cosodulieu.cpp           # 13 test cases: CRUD tài khoản, mật khẩu SHA-256, UID thẻ, lịch sử
│   │   ├── config/
│   │   │   └── test_cauhinhhethong.cpp       # 8 test cases: Phân tích giá trị biên (1s, 300s, 0s, -10s, nạp lại JSON)
│   │   └── ble_protocol/
│   │       └── test_ble_protocol.cpp         # 12 test cases: Parser & Formatter frame TOPIC|PAYLOAD, ký tự lỗi
│   │
│   ├── integration/                          # [MỨC 2] KIỂM THỬ TÍCH HỢP (INTEGRATION TEST)
│   │   └── test_ble_esp32.cpp                # 8 kịch bản IT-01 -> IT-08 (Pi ↔ BLE GATT ↔ ESP32)
│   │
│   ├── system/                               # [MỨC 3] KIỂM THỬ HỆ THỐNG (SYSTEM TEST)
│   │   └── SYSTEM_TEST_CASES.md              # 5 kịch bản End-to-End toàn diện (ST-01 -> ST-05)
│   │
│   └── acceptance/                           # [MỨC 4] KIỂM THỬ CHẤP NHẬN (ACCEPTANCE TEST)
│       └── ACCEPTANCE_TEST_CASES.md          # 4 kịch bản nghiệm thu chức năng nghiệp vụ (AT-01 -> AT-04)
│
├── test_evidence/                            # BẰNG CHỨNG KIỂM THỬ THỰC TẾ (LOGS)
│   ├── TEST_REPORT.md                        # Báo cáo tổng hợp kết quả nghiệm thu (62/62 test case PASS)
│   ├── qt/
│   │   ├── unit/
│   │   │   ├── test_cosodulieu.log           # Log chạy thực tế Unit Test CSDL trên Pi
│   │   │   ├── test_cauhinhhethong.log       # Log chạy thực tế Unit Test Cấu hình trên Pi
│   │   │   └── test_ble_protocol.log         # Log chạy thực tế Unit Test Giao thức BLE trên Pi
│   │   └── integration/
│   │       └── integration_IT01_IT08.log     # Log chạy thực tế 8 kịch bản tích hợp IT-01 -> IT-08
│
└── esp32/ (và "C:\Users\admin\Documents\PlatformIO\Projects\RFID")
    ├── test/                                 # BỘ KIỂM THỬ FIRMWARE ESP32 (UNITY FRAMEWORK)
    │   ├── README_TEST.md                    # Hướng dẫn chạy test PlatformIO trên Windows
    │   ├── test_ble_protocol/test_ble_protocol.cpp # Unit test parser frame trên vi điều khiển
    │   ├── test_door/test_door.cpp           # Hardware test động cơ Servo SG90 (0° đóng, 90° mở)
    │   ├── test_sensor/test_sensor.cpp       # Hardware test cảm biến vật cản quang LM393 (Waiting manual action)
    │   └── test_rfid/test_rfid.cpp           # Hardware test đầu đọc thẻ RFID RC522 qua SPI
    └── platformio.ini
```

---

## 2. KỊCH BẢN THUYẾT TRÌNH VÀ CÁC LỆNH DEMO TRỰC TIẾP

Khi trình bày với thầy cô, bạn hãy mở terminal và thực hiện theo thứ tự 4 bước sau:

### BƯỚC 1: Mở kiến trúc tổng quan và kế hoạch kiểm thử (1 - 2 phút)
* **Thao tác:** Mở file `TESTING.md` và `test_evidence/TEST_REPORT.md` trên màn hình trình chiếu.
* **Thuyết trình:**
  > *"Thưa thầy cô, bộ kiểm thử của dự án được thiết kế bám sát theo giáo trình 'Bài 11 - Quy trình test phần mềm' với đầy đủ 4 mức kiểm định: Unit Test, Integration Test, System Test và Acceptance Test. Dự án đạt tỷ lệ pass 100% trên toàn bộ 62 testcase thực tế và có lưu trữ đầy đủ Test Evidence."*

---

### BƯỚC 2: Demo chạy trực tiếp Unit Test trên Raspberry Pi (2 - 3 phút)
Mở Terminal trên máy tính kết nối SSH vào Pi (hoặc Terminal trên Pi):

```bash
ssh pi@192.168.137.227
export LD_LIBRARY_PATH=/usr/local/qt6/lib:/usr/local/lib
cd /home/pi/he-thong-kiem-soat-ra-vao/tests_bin
```

#### 2.1. Chạy Unit Test Giao thức BLE (Parser & Validator):
```bash
./test_ble_protocol
```
* **Kết quả hiển thị:** `Totals: 12 passed, 0 failed, 0 skipped (1ms)`
* **Thuyết trình:** 
  > *"Module `GiaoThucBle` được tách độc lập để kiểm thử đóng gói và phân tích frame `TOPIC|PAYLOAD`. Testcase kiểm tra thành công các trường hợp frame hợp lệ, xử lý frame nhiều dòng, và chặn đứng các lỗi: thiếu dấu phân cách `|`, topic rỗng, hoặc payload chứa ký tự cấm xuống dòng `\n`, `\r`."*

#### 2.2. Chạy Unit Test Cấu hình hệ thống (Phân tích giá trị biên):
```bash
mkdir -p run_cfg && cd run_cfg && ../test_cauhinhhethong && cd ..
```
* **Kết quả hiển thị:** `Totals: 8 passed, 0 failed, 0 skipped (2ms)`
* **Thuyết trình:** 
  > *"Ở module cấu hình thời gian đóng cửa, nhóm áp dụng phương pháp Phân tích giá trị biên (Boundary Value Analysis) đúng slide giảng dạy: Giá trị cận dưới hợp lệ là 1 giây, giá trị cận trên hợp lệ là 300 giây, các giá trị lỗi dưới biên như 0 giây hay số âm bị từ chối lưu."*

#### 2.3. Chạy Unit Test Cơ sở dữ liệu SQLite:
```bash
mkdir -p run_db && cd run_db && ../test_cosodulieu && cd ..
```
* **Kết quả hiển thị:** `Totals: 13 passed, 0 failed, 0 skipped (128ms)`
* **Thuyết trình:** 
  > *"Module CSDL kiểm tra toàn bộ luồng đăng nhập admin mã hóa mật khẩu SHA-256, kiểm tra tính duy nhất của mã UID thẻ (ngăn trùng lặp thẻ), ghi nhật ký và kiểm tra chức năng tìm kiếm, phân loại lọc lịch sử."*

---

### BƯỚC 3: Demo chạy Integration Test 8 kịch bản tích hợp Pi ↔ ESP32 (3 - 4 phút)
*Đây là phần quan trọng nhất chứng minh khả năng tích hợp thực tế với phần cứng.*

Chạy trực tiếp lệnh:
```bash
export LD_LIBRARY_PATH=/usr/local/qt6/lib:/usr/local/lib
cd /home/pi/he-thong-kiem-soat-ra-vao/tests_bin
./test_ble_esp32
```

* **Chỉ tay vào màn hình giải thích từng kịch bản khi log in ra:**
  1. `IT-01 Scan`: Quét sóng và bắt đúng thiết bị `"HE THONG RA VAO HIEU" [44:1D:64:F5:01:9E]`.
  2. `IT-02 Connect`: Thiết lập liên kết GATT, tự động khám phá Service và Characteristic.
  3. `IT-03 Send`: Gửi lệnh điều khiển `access/door/command|OPEN` qua hàng đợi FIFO.
  4. `IT-04 Receive`: Nhận Notify trạng thái cửa đóng `access/door/status|CLOSED` gửi ngược từ ESP32 về Pi.
  5. `IT-05 Verify Data`: Gửi tham số timeout và kiểm tra tính toàn vẹn dữ liệu `RX = TX`.
  6. `IT-06 Timeout`: Thử nghiệm MAC giả lập không phản hồi, ứng dụng vẫn chạy bất đồng bộ trơn tru, không bao giờ bị đơ/treo.
  7. `IT-07 Disconnect`: Ngắt kết nối có kiểm soát an toàn.
  8. `IT-08 Reconnect`: Kích hoạt tự động quét lại và khôi phục kết nối thành công với ESP32!
* **Kết quả hiển thị:** `Totals: 10 passed, 0 failed, 0 skipped (26742ms)`.

---

### BƯỚC 4: Show Unit & Hardware Test của ESP32 trên PlatformIO Windows (2 phút)
* **Thao tác:** Mở VS Code trên máy tính Windows tại thư mục `C:\Users\admin\Documents\PlatformIO\Projects\RFID`.
* **Mở các file trong thư mục `test/` cho thầy xem:**
  * Mở file `test/test_door/test_door.cpp`: Chỉ vào các hàm `ServoCua::moCua()`, `ServoCua::dongCua()` và `TEST_ASSERT_TRUE/FALSE`.
  * Mở file `test/test_sensor/test_sensor.cpp`: Chỉ vào thông báo `WAITING FOR MANUAL ACTION: Please trigger the LM393 sensor`.
* **Nếu thầy yêu cầu chạy test ESP32 trên Windows**, mở Terminal PlatformIO gõ:
  ```bash
  # Chạy test bộ phân tích giao thức BLE:
  pio test -e esp32dev -f test_ble_protocol

  # Chạy test điều khiển servo cửa:
  pio test -e esp32dev -f test_door
  ```

---

## 3. BẢNG TỔNG KẾT TRẢ LỜI CÁC CÂU HỎI HỘI ĐỒNG

| Câu hỏi thầy cô thường hỏi | Câu trả lời chuẩn xác |
| :--- | :--- |
| **"Tại sao test tích hợp IT-04 lại có thông báo WAITING FOR MANUAL ACTION?"** | Vì IT-04 kiểm tra chiều Notify từ ESP32 lên Pi. Trong slide có nêu rõ 'Test có phần cứng', nên hệ thống tạo cửa sổ chờ người dùng quẹt thẻ hoặc kích hoạt cảm biến để tạo sự kiện ngoại vi thực tế. |
| **"Có thay đổi logic nghiệp vụ của code cũ để test pass không?"** | Tuyệt đối không. Nhóm chỉ tách module phân tích/đóng gói frame BLE thành `GiaoThucBle` dùng chung, giữ nguyên 100% logic và tham số của hệ thống. |
| **"Khi chạy test CSDL nhiều lần có làm bẩn hay xóa mất dữ liệu thật không?"** | Không. Test runner khởi tạo môi trường chạy tạm cô lập trong thư mục `run_db/data/hethongkiemsoat.sqlite`, hoàn toàn độc lập với database sản xuất của ứng dụng. |
