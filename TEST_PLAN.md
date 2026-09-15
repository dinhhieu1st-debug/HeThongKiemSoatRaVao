# TEST PLAN - KẾ HOẠCH KIỂM THỬ HỆ THỐNG
## Dự án: Hệ thống kiểm soát ra vào (Raspberry Pi + ESP32 BLE)
*Cơ sở tài liệu: Bám sát "Bài 11 - Quy trình test phần mềm" và hiện trạng mã nguồn thực tế.*

---

## 1. Mục tiêu và Phạm vi kiểm thử

### 1.1. Mục tiêu
- Đảm bảo tính đúng đắn của các đơn vị mã nguồn (Unit Testing) độc lập: Cơ sở dữ liệu SQLite, Quản lý cấu hình JSON, Giao thức mã hóa & phân tích Frame BLE `TOPIC|PAYLOAD`.
- Đảm bảo khả năng giao tiếp và tương tác đa tầng (Integration Testing) giữa ứng dụng Qt trên Raspberry Pi và firmware ESP32 qua BLE GATT.
- Kiểm tra tính toàn vẹn của các kịch bản toàn hệ thống (System Testing) trong điều kiện vận hành thực tế.
- Đối soát các tiêu chí nghiệm thu người dùng (Acceptance Testing) theo đặc tả yêu cầu.

### 1.2. Phân loại mức kiểm thử (Test Levels)
Theo mô hình chữ V và cấu trúc phân tầng trong Bài 11:
```text
Unit Test (Qt C++ & ESP32 PlatformIO)
  ↓
Integration Test (Qt/Pi ↔ BLE ↔ ESP32)
  ↓
System Test (End-to-End Scenarios)
  ↓
Acceptance Test (User Requirements Verification)
```

---

## 2. Danh mục chi tiết các Module và Test Case

### A. Module Cơ sở dữ liệu (`CoSoDuLieu`) - Qt Unit Test
- **File nguồn:** `include/cosodulieu.h`, `src/cosodulieu.cpp`
- **Môi trường chạy:** Local/Target (không cần phần cứng ngoại vi)
- **Framework:** Qt Test Framework (`QTest`)

| Test ID | Tên Test Case | Hàm / Chức năng kiểm thử | Input | Expected Result | Loại Test | Cần phần cứng? |
|---|---|---|---|---|---|---|
| **UT-QT-01** | `khoiTao_taoBang` | `khoiTao()`, `taoBangTaiKhoan()`, `taoBangTheRfid()`, `taoBangLichSuRaVao()` | Mở kết nối SQLite và tạo bảng | Trả về `true`, 3 bảng `tai_khoan`, `the_rfid`, `lich_su_ra_vao` tồn tại | Unit (Normal) | Không |
| **UT-QT-02** | `taoTaiKhoanMacDinh` | `taoTaiKhoanMacDinh()` | Gọi lần đầu khi DB rỗng | Tạo thành công tài khoản `admin` / `123456`, quyền `Quan tri vien` | Unit (Normal) | Không |
| **UT-QT-03** | `login_success` | `kiemTraDangNhap()`, `dangNhapVaLayThongTin()` | `admin` / `123456` | Trả về `true`, thông tin họ tên & quyền khớp chính xác | Unit (Normal) | Không |
| **UT-QT-04** | `login_wrong_password` | `kiemTraDangNhap()` | `admin` / `sai_mat_khau` | Trả về `false`, không lấy được phiên | Unit (Error) | Không |
| **UT-QT-05** | `login_user_not_found` | `kiemTraDangNhap()` | `user_khong_ton_tai` / `123456` | Trả về `false` | Unit (Error) | Không |
| **UT-QT-06** | `add_account_success` | `themTaiKhoan()` | `user1`, `matkhau123`, `Nguyen Van A`, `Nhan vien` | Trả về `true`, tài khoản được thêm vào DB | Unit (Normal) | Không |
| **UT-QT-07** | `add_account_duplicate` | `themTaiKhoan()` | `admin` (trùng tên đăng nhập) | Trả về `false`, thông báo lỗi chứa `UNIQUE constraint` | Unit (Boundary/Error) | Không |
| **UT-QT-08** | `update_account` | `capNhatTaiKhoan()` | `id` hợp lệ, đổi họ tên và quyền | Trả về `true`, dữ liệu đọc lại cập nhật đúng | Unit (Normal) | Không |
| **UT-QT-09** | `change_password` | `doiMatKhauTaiKhoan()` | `id` hợp lệ, mật khẩu mới `newpass123` | Trả về `true`, đăng nhập bằng pass cũ thất bại, pass mới thành công | Unit (Normal) | Không |
| **UT-QT-10** | `delete_account` | `xoaTaiKhoan()` | Xóa tài khoản `user1` | Trả về `true`, tài khoản bị xóa khỏi DB | Unit (Normal) | Không |
| **UT-QT-11** | `delete_current_account_forbidden` | `xoaTaiKhoan()` | Xóa tài khoản trùng với `tenDangNhapHienTai` | Trả về `false`, không cho phép tự xóa phiên hiện tại | Unit (Boundary/Security) | Không |
| **UT-QT-12** | `add_card_success` | `themThe()` | UID: `D72D6303`, Tên: `Tran Van B`, Mã: `NV01`, Quyền: `Nhan vien` | Trả về `true`, thẻ được thêm thành công | Unit (Normal) | Không |
| **UT-QT-13** | `add_card_duplicate` | `themThe()` | Thêm lại UID `D72D6303` | Trả về `false`, báo lỗi trùng UID | Unit (Error) | Không |
| **UT-QT-14** | `find_card_found` | `timTheTheoUid()` | UID: `D72D6303` | Trả về `true`, trả về đúng tên `Tran Van B` | Unit (Normal) | Không |
| **UT-QT-15** | `find_card_not_found` | `timTheTheoUid()` | UID: `UNKNOWN99` | Trả về `false` | Unit (Error) | Không |
| **UT-QT-16** | `update_and_delete_card` | `capNhatThe()`, `xoaThe()` | Sửa tên thẻ rồi xóa thẻ | Trả về `true`, xóa xong tìm lại trả về `false` | Unit (Normal) | Không |
| **UT-QT-17** | `history_logging_and_filtering` | `themLichSuRaVao()`, `timKiemLichSuRaVao()` | Thêm các bản ghi: Quẹt thẻ, Mở thủ công, Đóng thủ công, BLE | Lưu thành công, lọc theo từng danh mục trả về đúng bản ghi | Unit (Normal/Filter) | Không |

---

### B. Module Quản lý Cấu hình (`QuanLyCauHinh`) - Qt Unit Test
- **File nguồn:** `include/cauhinhhethong.h`, `src/cauhinhhethong.cpp`
- **Môi trường chạy:** Local/Target
- **Framework:** Qt Test Framework (`QTest`)

| Test ID | Tên Test Case | Hàm / Chức năng | Input | Expected Result | Loại Test | Cần phần cứng? |
|---|---|---|---|---|---|---|
| **UT-QT-20** | `config_default_value` | `khoiTao()`, `docCauHinh()` | Không có file cấu hình cũ | Tạo file mặc định, `thoiGianDongCuaGiay = 15` | Unit (Normal) | Không |
| **UT-QT-21** | `config_save_and_reload` | `luuCauHinh()`, `docCauHinh()` | Đặt `thoiGianDongCuaGiay = 25` | Lưu thành công, đọc lại trả về đúng 25 | Unit (Normal) | Không |
| **UT-QT-22** | `config_boundary_min` | Kiểm tra giá trị biên dưới | `thoiGianDongCuaGiay = 1` | Hợp lệ (1 giây) | Unit (Boundary) | Không |
| **UT-QT-23** | `config_boundary_max` | Kiểm tra giá trị biên trên | `thoiGianDongCuaGiay = 300` | Hợp lệ (300 giây) | Unit (Boundary) | Không |
| **UT-QT-24** | `config_corrupted_file` | Đọc file JSON bị hỏng format | Ghi dữ liệu rác vào `cauhinh.json` | Tự phục hồi về giá trị mặc định (15 giây), không crash | Unit (Robustness) | Không |

---

### C. Module BLE Protocol & Parser - Qt Unit Test
- **File nguồn:** `include/ketnoible.h`, `src/ketnoible.cpp` (logic parser và queue)
- **Môi trường chạy:** Local/Target
- **Framework:** Qt Test Framework (`QTest`)

| Test ID | Tên Test Case | Hàm / Chức năng | Input | Expected Result | Loại Test | Cần phần cứng? |
|---|---|---|---|---|---|---|
| **UT-QT-30** | `parse_valid_rfid_frame` | `xuLyFrame()` | `"access/rfid\|D72D6303"` | Phát signal `nhanDuLieu("access/rfid", "D72D6303")` | Unit (Normal) | Không |
| **UT-QT-31** | `parse_valid_door_status` | `xuLyFrame()` | `"access/door/status\|OPENED"` | Phát signal `nhanDuLieu("access/door/status", "OPENED")` | Unit (Normal) | Không |
| **UT-QT-32** | `parse_valid_sensor_status` | `xuLyFrame()` | `"access/sensor\|OBSTACLE"` | Phát signal `nhanDuLieu("access/sensor", "OBSTACLE")` | Unit (Normal) | Không |
| **UT-QT-33** | `parse_multiline_frame` | `xuLyFrame()` | `"access/status\|ONLINE\naccess/door/status\|CLOSED"` | Tách đúng 2 frame và phát 2 signal độc lập | Unit (Normal) | Không |
| **UT-QT-34** | `validate_missing_separator`| `xuLyFrame()` | `"access/statusONLINE"` | Phát signal `coLoi("Frame BLE khong dung TOPIC\|PAYLOAD")` | Unit (Error) | Không |
| **UT-QT-35** | `validate_empty_topic` | `xuLyFrame()` | `"\|PAYLOAD"` | Báo lỗi frame không hợp lệ | Unit (Error) | Không |
| **UT-QT-36** | `validate_forbidden_chars`| `guiDuLieu()` | Payload chứa `\|`, `\n`, `\r` | Trả về `false`, từ chối đưa vào hàng đợi | Unit (Boundary/Validation) | Không |

---

### D. Module Firmware ESP32 - Unity Unit & Hardware Test
- **File nguồn:** `esp32/include/`, `esp32/src/`
- **Môi trường chạy:** ESP32 Board (PlatformIO Unity Framework)

| Test ID | Tên Test Case | Module / Hàm | Input / Điều kiện | Expected Result | Loại Test | Cần phần cứng? |
|---|---|---|---|---|---|---|
| **UT-ESP-01**| `ble_protocol_parse_valid` | `xuLyFrame()` | `"access/door/command\|OPEN"` | Gọi đúng callback lệnh cửa với payload `"OPEN"` | Unit (Logic) | Không |
| **UT-ESP-02**| `ble_protocol_invalid_frame`| `xuLyFrame()` | `"invalid_frame_without_pipe"`| Bỏ qua frame, không gọi callback | Unit (Error) | Không |
| **UT-ESP-03**| `ble_timeout_config_boundary`| `xuLyFrame()` | `"access/config/door_timeout\|20"` | Cập nhật `thoiGianTuDongDongCuaMs = 20000` | Unit (Boundary) | Không |
| **UT-ESP-04**| `door_servo_logic_open_close` | `ServoCua::moCua()`, `dongCua()` | Lệnh mở rồi đóng | Trạng thái `dangMo()` đổi từ `true` sang `false`, xung PWM tương ứng | Hardware Test | Có (ESP32) |
| **UT-ESP-05**| `sensor_obstacle_debounce` | `ObstacleSensor::update()` | Đọc chân GPIO34 qua lọc nhiễu 100ms | Báo đúng trạng thái có/không có vật cản | Hardware Test | Có (WAITING FOR MANUAL ACTION nếu kích hoạt vật lý) |
| **UT-ESP-06**| `rfid_read_card_format` | `RFIDSensor::readCard()` | Quét thẻ RFID thật | Trả về chuỗi Hex UID viết hoa (ví dụ: `D72D6303`) | Hardware Test | Có (WAITING FOR MANUAL ACTION: Quẹt thẻ) |

---

### E. Integration Test (8 Kịch bản Bài 11: IT-01 đến IT-08)
- **Môi trường thực thi:** Raspberry Pi (`192.168.137.227`) kết nối BLE tới ESP32 (`44:1D:64:F5:01:9E`).

| Test ID | Tên Kịch bản | Mục tiêu kiểm thử | Quy trình thực hiện | Expected Result | Ghi nhận thực tế |
|---|---|---|---|---|---|
| **IT-01** | `BLE Scan` | Pi phát hiện ESP32 | Quét thiết bị BLE qua BlueZ | Tìm thấy thiết bị `HE THONG RA VAO HIEU` với UUID hợp lệ | Chạy thật trên Pi |
| **IT-02** | `BLE Connect` | Pi kết nối GATT tới ESP32 | Kết nối tới địa chỉ MAC của ESP32 | Kết nối GATT thành công, subscribe Notify Service `7d9a0001-...` | Chạy thật trên Pi |
| **IT-03** | `BLE Send` | Pi gửi lệnh sang ESP32 | Gửi `access/door/command\|OPEN` | Lệnh được truyền qua Characteristic Write thành công | Chạy thật trên Pi |
| **IT-04** | `BLE Receive` | Pi nhận dữ liệu từ ESP32 | ESP32 phát Notify trạng thái cửa | Pi nhận được frame `access/door/status\|OPENED` | Chạy thật trên Pi |
| **IT-05** | `Verify Data` | Kiểm tra tính toàn vẹn RX = TX | Pi gửi chuỗi cấu hình timeout (ví dụ 11s) | ESP32 nhận đúng giá trị 11, phản hồi trạng thái khớp | Chạy thật trên Pi |
| **IT-06** | `Timeout / Dropped Frame` | Thiết bị không phản hồi tức thì | Gửi frame khi kết nối bận | Hàng đợi FIFO xếp hàng an toàn, không crash, không mất gói | Chạy thật trên Pi |
| **IT-07** | `Disconnect` | Phát hiện ngắt kết nối BLE | Ngắt nguồn ESP32 hoặc ngắt BLE | Pi phát hiện mất kết nối (`daNgatKetNoiBle`), cập nhật giao diện an toàn | Chạy thật trên Pi |
| **IT-08** | `Reconnect` | Tự động kết nối lại | Bật lại nguồn ESP32 | Pi phát hiện thiết bị và tự động tái kết nối, đồng bộ lại timeout | Chạy thật trên Pi |

---

### F. System Test (ST-01 đến ST-05) & Acceptance Test (AT-01 đến AT-04)
Đặc tả chi tiết trong `tests/system/SYSTEM_TEST_CASES.md` và `tests/acceptance/ACCEPTANCE_TEST_CASES.md`.

---

## 3. Quy trình thực hiện và Bằng chứng kiểm thử (Evidence)
1. Mọi bài test được chạy trên môi trường thực tế (Ubuntu VM / Raspberry Pi).
2. Output của từng lần chạy được ghi vào `test_evidence/` theo định dạng text/log.
3. Báo cáo tổng kết được biên soạn tại `test_evidence/TEST_REPORT.md` với đầy đủ thông số môi trường, bảng thống kê Passed/Failed/Waiting và bằng chứng log.
