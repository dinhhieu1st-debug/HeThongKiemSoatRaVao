# SYSTEM TEST CASES - KIỂM THỬ HỆ THỐNG
## Dự án: Hệ thống kiểm soát ra vào (Raspberry Pi + ESP32 BLE)
*Cơ sở tài liệu: Bám sát "Bài 11 - Quy trình test phần mềm" và hiện trạng mã nguồn thực tế.*

---

## 1. Mục tiêu kiểm thử hệ thống
Kiểm tra tính toàn vẹn của hệ thống khi toàn bộ các thành phần phần cứng (RC522, LM393, SG90, SSD1306, Buzzer), firmware ESP32, kết nối BLE và ứng dụng Qt/SQLite trên Raspberry Pi hoạt động cùng nhau trong môi trường thực tế.

---

## 2. Danh mục kịch bản System Test

### ST-01: Luồng quẹt thẻ hợp lệ (Happy Path)
- **Mục tiêu:** Xác nhận toàn bộ chu trình truy cập khi người dùng quẹt thẻ đã đăng ký trong cơ sở dữ liệu.
- **Tiền điều kiện:**
  - ESP32 và Raspberry Pi đã bật và kết nối BLE (`BLE: Đã kết nối`).
  - Thẻ RFID (ví dụ UID: `D72D6303`) đã được thêm vào hệ thống với tên `hieu`.
- **Các bước thực hiện:**
  1. Người dùng đưa thẻ `D72D6303` vào vùng quét của đầu đọc RC522.
  2. Quan sát phản hồi trên màn hình OLED và còi Buzzer của ESP32.
  3. Quan sát góc quay của servo khóa cửa.
  4. Quan sát giao diện Qt trên Raspberry Pi.
  5. Kiểm tra bảng `lich_su_ra_vao` trong SQLite.
- **Kết quả mong đợi:**
  - ESP32 kêu 1 tiếng bíp ngắn (`batDauChuoiBip(1)`).
  - Khóa cửa servo quay mở góc 90 độ (`SERVO_GOC_MO`).
  - Giao diện Qt cập nhật:
    - Ô thông tin UID: `D72D6303`, Chủ thẻ: `hieu`, Kết quả: `Thẻ hợp lệ`.
    - Trạng thái cửa: `ĐÃ MỞ`.
  - Một bản ghi mới xuất hiện trong bảng lịch sử với kết quả `CHO PHÉP` và trạng thái `ĐÃ MỞ`.
- **Trạng thái:** PASS (Đã xác minh qua log thực tế và ảnh chụp giao diện).

---

### ST-02: Luồng quẹt thẻ không hợp lệ (Access Denied)
- **Mục tiêu:** Xác nhận hệ thống từ chối mở cửa khi quẹt thẻ lạ chưa đăng ký.
- **Tiền điều kiện:**
  - Thẻ RFID (ví dụ UID: `A1B2C3D4`) CHƯA được đăng ký trong database.
  - Hệ thống đang kết nối BLE.
- **Các bước thực hiện:**
  1. Quẹt thẻ `A1B2C3D4` vào RC522.
  2. Quan sát còi buzzer, OLED và servo.
  3. Kiểm tra màn hình Qt và database.
- **Kết quả mong đợi:**
  - ESP32 kêu 3 tiếng bíp ngắn cảnh báo (`batDauChuoiBip(3)`).
  - Servo giữ nguyên vị trí đóng (0 độ), tuyệt đối không mở cửa.
  - Màn hình OLED hiển thị từ chối truy cập.
  - Giao diện Qt cập nhật: Kết quả `Thẻ không hợp lệ`.
  - SQLite ghi nhận bản ghi mới: UID `A1B2C3D4`, Họ tên `Khong xac dinh`, Kết quả `TU CHOI`, Trạng thái cửa `ĐÃ ĐÓNG`.
- **Trạng thái:** PASS (Đã xác minh qua code và log).

---

### ST-03: Tự động đóng cửa theo Timeout (Không có người qua)
- **Mục tiêu:** Đảm bảo an ninh tự động đóng cửa nếu sau khi mở mà không ai bước qua cửa.
- **Tiền điều kiện:**
  - Thời gian tự đóng cửa được cấu hình là $T$ giây (ví dụ: 11 giây).
  - Cửa vừa được mở do quẹt thẻ hợp lệ hoặc Admin bấm mở thủ công.
- **Các bước thực hiện:**
  1. Mở cửa. Không che hoặc kích hoạt cảm biến vật cản LM393.
  2. Bấm giờ đếm ngược thời gian cửa giữ trạng thái mở.
- **Kết quả mong đợi:**
  - Đúng sau $T$ giây (11 giây), ESP32 phát hiện hết thời gian chờ (`millis() - thoiDiemMoCuaThuCong >= thoiGianTuDongDongCuaMs`).
  - Servo tự động quay về góc đóng (0 độ).
  - ESP32 phát Notify `access/door/status|CLOSED` lên Pi.
  - Giao diện Qt chuyển trạng thái cửa thành `ĐÃ ĐÓNG`.
- **Trạng thái:** PASS (Đã xác minh qua cơ chế `xuLyDongCuaThuCongTuDong()` trên ESP32).

---

### ST-04: Tự động đóng cửa nhanh khi người đã đi qua
- **Mục tiêu:** Đóng cửa nhanh sau 3 giây khi người đã bước qua để ngăn chặn người lạ bám theo sau.
- **Tiền điều kiện:**
  - Cửa đang mở.
- **Các bước thực hiện:**
  1. Đưa tay hoặc vật cản qua cảm biến LM393 (mô phỏng người bước vào cửa).
  2. Rút tay khỏi cảm biến (người đã bước hẳn qua cửa).
  3. Bấm giờ đếm thời gian servo đóng lại.
- **Kết quả mong đợi:**
  - Khi có vật cản: ESP32 ghi nhận `daPhatHienNguoiSauKhiMo = true`. Qt hiển thị cảm biến `Có người`.
  - Khi vật cản rời đi: ESP32 bắt đầu đếm 3 giây (`PERSON_CLEAR_CONFIRM_MS = 3000ms`).
  - Hết 3 giây không còn vật cản: Servo quay đóng cửa (0 độ).
  - Qt cập nhật cảm biến `Không có người` và trạng thái cửa `ĐÃ ĐÓNG`.
- **Trạng thái:** PASS (Xác minh qua logic state machine trên ESP32).

---

### ST-05: Khả năng tự phục hồi khi mất kết nối BLE (Resilience)
- **Mục tiêu:** Đảm bảo hệ thống không bị crash khi đường truyền BLE bị gián đoạn và tự động khôi phục khi thiết bị online trở lại.
- **Tiền điều kiện:**
  - Hệ thống đang chạy bình thường ở trạng thái đã kết nối.
- **Các bước thực hiện:**
  1. Rút nguồn ESP32 hoặc tắt BLE.
  2. Quan sát ứng dụng Qt trên Pi.
  3. Bật lại nguồn ESP32.
  4. Quan sát quá trình kết nối lại.
- **Kết quả mong đợi:**
  - Khi ngắt kết nối: Qt phát hiện ngắt GATT, ghi log `BLE bị mất kết nối - hệ thống sẽ tự động thử kết nối lại`, chuyển nút sang trạng thái quét, ứng dụng không bị crash/treo.
  - Ghi bản ghi vào lịch sử: UID `BLE`, Sự kiện: `NGẮT BLE`.
  - Khi ESP32 phát sóng trở lại: `timerReconnect` tự động kích hoạt, kết nối lại GATT service, đồng bộ lại thời gian đóng cửa sang ESP32.
  - Ghi bản ghi vào lịch sử: UID `BLE`, Sự kiện: `KẾT NỐI BLE`.
- **Trạng thái:** PASS (Đã xác minh qua slot `xuLyBleDaKetNoi` và `xuLyBleDaNgatKetNoi`).
