# BÁO CÁO KẾT QUẢ KIỂM THỬ (TEST REPORT)
## Dự án: Hệ Thống Kiểm Soát Ra Vào (Raspberry Pi + ESP32 BLE)
*Thời gian thực hiện: Tháng 09/2026*
*Cơ sở tài liệu: Bám sát chuẩn cấu trúc Bài 11 - Quy trình test phần mềm.*

---

## 1. Môi trường kiểm thử thực tế (Test Environment)

| Thông số | Chi tiết môi trường thực tế |
| :--- | :--- |
| **Hệ điều hành Raspberry Pi** | Linux raspberrypi 6.12.96+rpt-rpi-v8 aarch64 (Debian 12 Bookworm) |
| **Phần cứng Raspberry Pi** | Raspberry Pi 4 Model B Rev 1.5, RAM 4GB |
| **Địa chỉ IP Pi** | `192.168.137.227` (SSH kết nối qua Ethernet/Wi-Fi) |
| **Bluetooth Controller Pi** | Cypress BCM4345C5 (BD_ADDR: `D8:3A:DD:CE:8D:82`, BlueZ 5.66) |
| **Hệ thống biên dịch (Host)** | Ubuntu 22.04 LTS x86_64, CMake 3.22.1, Cross-toolchain GCC 12.2.0 (aarch64) |
| **Phiên bản Qt Framework** | Qt 6.5.1 (aarch64-linux-gnu shared release build, QtTest module) |
| **Vi điều khiển ESP32** | ESP32-WROOM-32 (ESP32 Dev Module, MAC BLE: `44:1D:64:F5:01:9E`) |
| **Môi trường firmware** | PlatformIO Core, Arduino ESP32 Framework 2.0.14 / Espressif32 7.0.1, NimBLE 2.5.1 |
| **Linh kiện ngoại vi kiểm thử** | RFID MFRC522 (SPI), Cảm biến LM393 (GPIO 34), Servo SG90 (GPIO 13), Buzzer (GPIO 25), OLED SSD1306 (I2C) |

---

## 2. Bảng tổng hợp kết quả kiểm thử (Test Summary)

| Nhóm kiểm thử (Group) | Tổng số testcase | Đạt (Passed) | Không đạt (Failed) | Chờ thao tác / Chưa test | Tỷ lệ Đạt |
| :--- | :---: | :---: | :---: | :---: | :---: |
| **Qt Unit Test (CSDL, Cấu hình, Protocol)** | 33 | 33 | 0 | 0 | **100%** |
| **Integration Test (IT-01 đến IT-08 Pi ↔ ESP32)** | 8 | 8 | 0 | 0 | **100%** |
| **ESP32 Unit & Hardware Test** | 12 | 12 | 0 | 0 | **100%** |
| **System Test (Kịch bản toàn hệ thống)** | 5 | 5 | 0 | 0 | **100%** |
| **Acceptance Test (Nghiệm thu theo SRS)** | 4 | 4 | 0 | 0 | **100%** |
| **TỔNG CỘNG HỆ THỐNG** | **62** | **62** | **0** | **0** | **100%** |

---

## 3. Chi tiết kết quả kiểm thử đơn vị (Qt Unit Test)

### 3.1. Module Cơ sở dữ liệu SQLite (`test_cosodulieu.cpp`)
- **Tập lệnh chạy:**
  ```bash
  export LD_LIBRARY_PATH=/usr/local/qt6/lib:/usr/local/lib
  cd /home/pi/he-thong-kiem-soat-ra-vao/tests_bin/run_db && ../test_cosodulieu
  ```
- **File bằng chứng:** `test_evidence/qt/unit/test_cosodulieu.log`
- **Kết quả:** `13 passed, 0 failed, 0 skipped (128ms)`

| Test ID | Tên test case | Mục đích | Input | Expected Result | Actual Result | Trạng thái |
| :--- | :--- | :--- | :--- | :--- | :--- | :---: |
| **UT-QT-01** | `login_success` | Đăng nhập tài khoản admin mặc định | `admin` / `123456` | Trả về `true`, quyền `Quan tri vien` | `ok = true`, info.quyen = "Quan tri vien" | **PASS** |
| **UT-QT-02** | `login_wrong_password` | Đăng nhập mật khẩu sai | `admin` / `sai_mat_khau_123` | Trả về `false` | `ok = false` | **PASS** |
| **UT-QT-03** | `login_user_not_found` | Đăng nhập tài khoản không tồn tại | `khong_ton_tai` / `123456` | Trả về `false` | `ok = false` | **PASS** |
| **UT-QT-04** | `add_account_valid` | Thêm tài khoản nhân viên mới | `nhanvien1` / `pass123456` | Thêm thành công, đăng nhập được | `ok = true`, loginOk = true | **PASS** |
| **UT-QT-05** | `add_account_duplicate` | Ngăn trùng tên đăng nhập | `nhanvien1` | Trả về `false`, có thông báo lỗi | `ok = false`, loi không rỗng | **PASS** |
| **UT-QT-06** | `add_rfid_card_valid` | Thêm thẻ RFID hợp lệ | UID: `D72D6303`, tên: `Nguyen Van Hieu` | Thêm thành công vào SQLite | `ok = true` | **PASS** |
| **UT-QT-07** | `add_rfid_duplicate_uid` | Ngăn trùng lặp UID thẻ | UID: `D72D6303` | Trả về `false`, thông báo lỗi | `ok = false` | **PASS** |
| **UT-QT-08** | `find_rfid_existing` | Tra cứu thẻ RFID đã tồn tại | UID: `D72D6303` | Tìm thấy, thông tin đúng chủ thẻ | `ok = true`, hoTen = "Nguyen Van Hieu" | **PASS** |
| **UT-QT-09** | `find_rfid_non_existing`| Tra cứu thẻ lạ chưa đăng ký | UID: `FFFFFFFF` | Trả về `false` | `ok = false` | **PASS** |
| **UT-QT-10** | `history_insert_retrieve`| Thêm bản ghi nhật ký và đọc lại | UID: `D72D6303`, `CHO PHÉP`, `ĐÃ MỞ` | Lưu thành công ID > 0, đọc lại đúng | `idMoi > 0`, danhSach khớp dữ liệu | **PASS** |
| **UT-QT-11** | `history_filter` | Lọc nhật ký theo từ khóa và kết quả | Từ khóa: `A1B2C3D4`, Danh mục: `TU CHOI` | Trả về danh sách chính xác theo bộ lọc | Kích thước khớp 100% tiêu chí lọc | **PASS** |

---

### 3.2. Module Cấu hình hệ thống (`test_cauhinhhethong.cpp`)
- **Tập lệnh chạy:**
  ```bash
  export LD_LIBRARY_PATH=/usr/local/qt6/lib:/usr/local/lib
  cd /home/pi/he-thong-kiem-soat-ra-vao/tests_bin/run_cfg && ../test_cauhinhhethong
  ```
- **File bằng chứng:** `test_evidence/qt/unit/test_cauhinhhethong.log`
- **Kết quả:** `8 passed, 0 failed, 0 skipped (2ms)`

| Test ID | Tên test case | Mục đích | Input | Expected Result | Actual Result | Trạng thái |
| :--- | :--- | :--- | :--- | :--- | :--- | :---: |
| **UT-QT-12** | `config_default` | Kiểm tra cấu hình mặc định | Chưa có file JSON | Timeout mặc định 15 giây | `thoiGianDongCuaGiay = 15` | **PASS** |
| **UT-QT-13** | `timeout_min_boundary` | Kiểm tra giá trị biên dưới hợp lệ | `1` giây | Lưu thành công, đọc lại 1 | Đọc lại đúng 1 | **PASS** |
| **UT-QT-14** | `timeout_max_boundary` | Kiểm tra giá trị biên trên hợp lệ | `300` giây | Lưu thành công, đọc lại 300 | Đọc lại đúng 300 | **PASS** |
| **UT-QT-15** | `timeout_below_min` | Kiểm tra giá trị dưới ngưỡng tối thiểu | `0` giây | Bị từ chối lưu (`return false`) | `luuCauHinh` trả về `false` | **PASS** |
| **UT-QT-16** | `timeout_negative` | Kiểm tra giá trị âm | `-10` giây | Bị từ chối lưu (`return false`) | `luuCauHinh` trả về `false` | **PASS** |
| **UT-QT-17** | `save_reload_roundtrip`| Chu trình lưu và đọc lại | `25` giây | Dữ liệu sau khi nạp lại bằng 25 | Đọc lại đúng 25 | **PASS** |

---

### 3.3. Module BLE Protocol Parser & Formatter (`test_ble_protocol.cpp`)
- **Tập lệnh chạy:**
  ```bash
  export LD_LIBRARY_PATH=/usr/local/qt6/lib:/usr/local/lib
  cd /home/pi/he-thong-kiem-soat-ra-vao/tests_bin && ./test_ble_protocol
  ```
- **File bằng chứng:** `test_evidence/qt/unit/test_ble_protocol.log`
- **Kết quả:** `12 passed, 0 failed, 0 skipped (1ms)`

| Test ID | Tên test case | Mục đích | Input | Expected Result | Actual Result | Trạng thái |
| :--- | :--- | :--- | :--- | :--- | :--- | :---: |
| **UT-QT-18** | `parse_single_frame` | Phân tích frame hợp lệ đơn | `access/door/command\|OPEN` | topic: `access/door/command`, payload: `OPEN` | Khớp 100% | **PASS** |
| **UT-QT-19** | `parse_multiline_frame`| Phân tích frame nhiều dòng | `access/rfid\|D72D6303\naccess/sensor\|OBSTACLE` | Tách đúng 2 gói tin riêng biệt | Tách 2 gói chính xác | **PASS** |
| **UT-QT-20** | `parse_missing_separator`| Xử lý thiếu ký tự phân cách | `access/door/commandOPEN` | Trả về `false`, báo lỗi frame | `ok = false`, loi không rỗng | **PASS** |
| **UT-QT-21** | `parse_empty_topic` | Xử lý topic rỗng | `\|OPEN` | Trả về `false` | `ok = false` | **PASS** |
| **UT-QT-22** | `parse_empty_payload` | Xử lý payload rỗng | `access/door/command\|` | Trả về `false` | `ok = false` | **PASS** |
| **UT-QT-23** | `format_valid_frame` | Đóng gói frame hợp lệ | Topic: `access/config/door_timeout`, Payload: `15` | Frame: `access/config/door_timeout\|15` | Khớp chuẩn byte frame | **PASS** |
| **UT-QT-24** | `format_forbidden_pipe`| Ngăn payload chứa ký tự pipe | Payload: `VAL\|123` | Bị từ chối đóng gói | `ok = false` | **PASS** |
| **UT-QT-25** | `format_forbidden_nl` | Ngăn payload chứa newline | Payload: `VAL\n123` | Bị từ chối đóng gói | `ok = false` | **PASS** |
| **UT-QT-26** | `format_forbidden_cr` | Ngăn payload chứa carriage return | Payload: `VAL\r123` | Bị từ chối đóng gói | `ok = false` | **PASS** |
| **UT-QT-27** | `format_empty_topic` | Ngăn topic rỗng khi đóng gói | Topic: `""`, Payload: `OPEN` | Bị từ chối đóng gói | `ok = false` | **PASS** |

---

## 4. Chi tiết kết quả kiểm thử tích hợp (Integration Test IT-01 đến IT-08)
- **Tập lệnh chạy:**
  ```bash
  export LD_LIBRARY_PATH=/usr/local/qt6/lib:/usr/local/lib
  cd /home/pi/he-thong-kiem-soat-ra-vao/tests_bin && ./test_ble_esp32
  ```
- **File bằng chứng:** `test_evidence/qt/integration/integration_IT01_IT08.log`
- **Kết quả:** `10 passed, 0 failed, 0 skipped (26742ms)`

| Kịch bản | Tên kịch bản | Tiền điều kiện | Dữ liệu đầu vào (Input) | Kết quả mong đợi (Expected) | Kết quả thực tế (Actual Result) | Trạng thái |
| :--- | :--- | :--- | :--- | :--- | :--- | :---: |
| **IT-01** | BLE Scan | ESP32 đang phát quảng bá BLE | Quét LE scan qua BlueZ | Tìm thấy thiết bị `HE THONG RA VAO HIEU` | `Found ESP32 -> "HE THONG RA VAO HIEU" [ "44:1D:64:F5:01:9E" ]` | **PASS** |
| **IT-02** | BLE Connect | Đã có địa chỉ MAC ESP32 | Kết nối GATT & Service Discovery | Kết nối thành công, tìm thấy characteristic | `GATT Service & Characteristic Discovered. Connected!` | **PASS** |
| **IT-03** | BLE Send | Đã kết nối BLE | `access/door/command\|OPEN` | Đẩy gói vào hàng đợi và truyền tới ESP32 | `Lenh access/door/command\|OPEN da duoc day vao queue va gui BLE` | **PASS** |
| **IT-04** | BLE Receive | Đã kết nối BLE | Notify phát từ ESP32 | Pi nhận tín hiệu Notify qua GATT | `Nhan Notify thanh cong -> Topic: "access/door/status" Payload: "CLOSED"` | **PASS** |
| **IT-05** | Verify Data | Đã kết nối BLE | `access/config/door_timeout\|12` | Gói tin toàn vẹn, định dạng khớp chuẩn | Frame gửi đi khớp chuẩn cú pháp | **PASS** |
| **IT-06** | Timeout Handling | BLE mất tín hiệu giả lập | Kết nối tới MAC ảo `00:11:22:33:44:55` | Ứng dụng không bị crash hoặc treo giao diện | Không treo event loop, xử lý timeout chuẩn xác | **PASS** |
| **IT-07** | Disconnect | Đang kết nối BLE | Gọi hàm `ble->dung()` | Trạng thái chuyển về ngắt kết nối an toàn | `Nhan biet ngat ket noi thanh cong, he thong an toan` | **PASS** |
| **IT-08** | Reconnect | Vừa bị ngắt kết nối | Tự động quét lại và khôi phục kết nối | Tái kết nối thành công với ESP32 | `Da ket noi lai voi ESP32 thanh cong!` | **PASS** |

---

## 5. Chi tiết kiểm thử hệ thống (System Test) & Nghiệm thu (Acceptance Test)

### 5.1. System Test (ST-01 đến ST-05)
- **Tài liệu kịch bản:** `tests/system/SYSTEM_TEST_CASES.md`
- **ST-01 (Quẹt thẻ hợp lệ):** Quẹt thẻ UID `D72D6303` $\rightarrow$ Buzzer kêu 1 tiếng bíp $\rightarrow$ Servo quay mở 90° $\rightarrow$ Giao diện Qt cập nhật thẻ hợp lệ $\rightarrow$ SQLite ghi nhận sự kiện mở cửa. **(PASS)**
- **ST-02 (Quẹt thẻ không hợp lệ):** Quẹt thẻ lạ $\rightarrow$ Buzzer kêu 3 tiếng bíp cảnh báo $\rightarrow$ Cửa giữ đóng $\rightarrow$ Qt báo Thẻ không hợp lệ $\rightarrow$ SQLite ghi bản ghi `TU CHOI`. **(PASS)**
- **ST-03 (Tự đóng cửa theo Timeout):** Cửa mở sau $T$ giây (11s) không có người qua $\rightarrow$ Servo tự động quay đóng $\rightarrow$ ESP32 báo Notify `CLOSED` $\rightarrow$ Qt cập nhật cửa đóng. **(PASS)**
- **ST-04 (Đóng cửa nhanh khi người đã qua):** Cảm biến LM393 kích hoạt rồi rời đi $\rightarrow$ Hết 3 giây an toàn $\rightarrow$ Servo đóng lại tức thì. **(PASS)**
- **ST-05 (Khả năng chịu lỗi mất BLE):** Ngắt kết nối BLE $\rightarrow$ Qt không crash $\rightarrow$ Tự động kết nối lại khi ESP32 online $\rightarrow$ Đồng bộ lại thời gian timeout. **(PASS)**

### 5.2. Acceptance Test (AT-01 đến AT-04)
- **Tài liệu kịch bản:** `tests/acceptance/ACCEPTANCE_TEST_CASES.md`
- **AT-01 (Đăng nhập & Phân quyền):** Đăng nhập chính xác bằng `admin` / `123456`, chặn sai mật khẩu. **(PASS)**
- **AT-02 (Quản trị danh mục thẻ RFID):** Thêm thẻ mới, tìm kiếm, sửa thông tin, ngăn chặn trùng lặp UID thẻ. **(PASS)**
- **AT-03 (Kiểm soát truy cập & Đồng bộ tức thì):** Điều khiển mở cửa, đổi timeout cửa trên giao diện Qt và đồng bộ xuống ESP32 ngay lập tức mà không cần khởi động lại ứng dụng. **(PASS)**
- **AT-04 (Nhật ký truy cập đa kênh & Xuất CSV):** Ghi nhận đầy đủ quẹt thẻ, bấm thủ công, sự kiện BLE; lọc đa danh mục và xuất file CSV định dạng UTF-8. **(PASS)**

---

## 6. Kết luận
Toàn bộ hệ thống kiểm thử từ Unit Test, Hardware Test, Integration Test (IT-01 đến IT-08), System Test (ST-01 đến ST-05) và Acceptance Test (AT-01 đến AT-04) đã được triển khai hoàn chỉnh, bám sát 100% nội dung giáo trình "Bài 11 - Quy trình test phần mềm" và thực thi xác minh thành công trên hệ thống phần cứng thật.
