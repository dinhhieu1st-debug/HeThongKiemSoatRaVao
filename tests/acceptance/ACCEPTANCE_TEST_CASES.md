# ACCEPTANCE TEST CASES - KIỂM THỬ CHẤP NHẬN
## Dự án: Hệ thống kiểm soát ra vào (Raspberry Pi + ESP32 BLE)
*Cơ sở tài liệu: Bám sát "Bài 11 - Quy trình test phần mềm", tài liệu đặc tả chức năng (SRS), và mã nguồn thực tế của hệ thống.*

---

## 1. Mục tiêu kiểm thử chấp nhận (User Acceptance Testing - UAT)
Đánh giá mức độ đáp ứng của hệ thống phần mềm đối với các yêu cầu nghiệp vụ thực tế của người quản trị tòa nhà / nhân viên kiểm soát an ninh trước khi bàn giao đưa vào vận hành chính thức.

---

## 2. Tiêu chuẩn chấp nhận (Acceptance Criteria)
Một kịch bản Acceptance Test được coi là **ĐẠT (PASS)** khi và chỉ khi:
1. Toàn bộ các bước kiểm thử được thực hiện đầy đủ trên hệ thống tích hợp thực tế.
2. Kết quả thực tế (Actual Result) trùng khớp hoàn toàn với kết quả mong đợi (Expected Result) quy định trong tài liệu.
3. Không gây lỗi tiềm ẩn (crash ứng dụng, treo giao diện, hỏng dữ liệu SQLite).

---

## 3. Danh mục kịch bản Acceptance Test

### AT-01: Quản trị tài khoản và xác thực phân quyền
- **Mã kịch bản:** AT-01
- **Chức năng nghiệp vụ:** Đăng nhập hệ thống, kiểm soát phiên làm việc và bảo mật phân quyền.
- **Tiêu chuẩn chấp nhận:**
  - Hệ thống chỉ cho phép người dùng đăng nhập khi cung cấp chính xác tên đăng nhập và mật khẩu đã mã hóa trong CSDL.
  - Phân quyền đúng: Tài khoản quản trị viên (`admin`) có toàn quyền chỉnh sửa cấu hình, thêm/sửa/xóa thẻ, xem và xuất nhật ký; người dùng không thể can thiệp dữ liệu khi chưa đăng nhập.
  - Xử lý khi đăng nhập sai: Hiển thị thông báo lỗi rõ ràng, không tiết lộ cấu trúc hệ thống hoặc crash chương trình.
- **Dữ liệu kiểm thử:**
  - Hợp lệ: `admin` / `admin123` -> Đăng nhập thành công, mở khóa toàn bộ màn hình điều khiển.
  - Sai mật khẩu: `admin` / `wrongpass` -> Báo lỗi "Tài khoản hoặc mật khẩu không chính xác".
  - Không tồn tại: `fakeuser` / `123456` -> Báo lỗi không tìm thấy người dùng.
- **Kết quả nghiệm thu:** ĐẠT (PASS)

---

### AT-02: Quản lý danh bạ thẻ RFID
- **Mã kịch bản:** AT-02
- **Chức năng nghiệp vụ:** Đăng ký thẻ mới cho nhân viên/cư dân, cập nhật thông tin và thu hồi thẻ khi cần thiết.
- **Tiêu chuẩn chấp nhận:**
  - Quản trị viên có thể thêm thẻ mới với đầy đủ UID, Họ và tên chủ thẻ, Ghi chú / Phòng ban.
  - Hệ thống tự động kiểm tra tính duy nhất của UID thẻ: Không cho phép thêm hai thẻ có cùng mã UID (ngăn chặn xung đột định danh).
  - Cho phép tra cứu nhanh theo UID hoặc Tên chủ thẻ.
  - Cho phép sửa thông tin chủ thẻ hoặc vô hiệu hóa/xóa thẻ khỏi hệ thống ngay lập tức có hiệu lực.
- **Dữ liệu kiểm thử:**
  - Thêm thẻ `D72D6303` - Chủ thẻ `hieu` -> Thêm thành công, hiển thị trên bảng danh sách.
  - Thêm lại UID `D72D6303` -> Hệ thống báo lỗi trùng lặp mã thẻ và từ chối lưu.
- **Kết quả nghiệm thu:** ĐẠT (PASS)

---

### AT-03: Kiểm soát cửa truy cập, cảm biến an toàn và đồng bộ tham số
- **Mã kịch bản:** AT-03
- **Chức năng nghiệp vụ:** Điều khiển đóng/mở cửa tự động qua xác thực thẻ, mở khẩn cấp thủ công, bảo vệ an toàn bằng cảm biến quang, và điều chỉnh tham số vận hành.
- **Tiêu chuẩn chấp nhận:**
  - Quẹt thẻ đã kích hoạt: Cửa tự động mở trong vòng < 1 giây, OLED và buzzer phản hồi xác nhận.
  - Quẹt thẻ chưa kích hoạt / thẻ lạ: Cửa tuyệt đối giữ đóng, còi báo động phát tín hiệu cảnh báo.
  - Admin có nút bấm "Mở cửa thủ công" trên giao diện Qt để giải quyết tình huống đặc biệt; có nút "Đóng cửa" tức thì.
  - Khi có người đang đi qua (cảm biến LM393 bị che), cửa không được đóng vào người gây mất an toàn; chỉ đóng sau khi người đã qua hẳn (an toàn 3 giây).
  - Cấu hình "Thời gian tự đóng cửa": Khi thay đổi (ví dụ từ 5s sang 11s) và bấm Lưu, tham số phải được đồng bộ không dây ngay lập tức xuống ESP32 mà không cần khởi động lại ứng dụng.
- **Dữ liệu kiểm thử:**
  - Đổi timeout từ 5s sang 11s -> Bấm Lưu -> ESP32 nhận frame `access/config/door_timeout|11` và cập nhật biến `thoiGianTuDongDongCuaMs = 11000`.
- **Kết quả nghiệm thu:** ĐẠT (PASS)

---

### AT-04: Nhật ký truy cập đa kênh và xuất báo cáo kiểm toán
- **Mã kịch bản:** AT-04
- **Chức năng nghiệp vụ:** Lưu trữ toàn bộ sự kiện diễn ra tại cửa (quẹt thẻ, bấm thủ công, kết nối/ngắt BLE) và trích xuất dữ liệu phục vụ đối soát, kiểm toán an ninh.
- **Tiêu chuẩn chấp nhận:**
  - Mọi sự kiện quẹt thẻ (Hợp lệ hay Từ chối) đều được ghi nhận thời gian chính xác xuống CSDL SQLite.
  - Mọi thao tác bấm nút mở cửa / đóng cửa thủ công của quản trị viên đều được tự động lưu vào nhật ký.
  - Mọi sự kiện kết nối thành công hoặc mất kết nối BLE giữa Pi và ESP32 đều được ghi nhận vào nhật ký.
  - Hỗ trợ bộ lọc trực quan theo 6 danh mục: `Tất cả sự kiện`, `Chỉ quẹt thẻ`, `Mở cửa thành công`, `Thẻ bị từ chối`, `Thao tác thủ công`, `Trạng thái BLE`.
  - Hỗ trợ tìm kiếm theo khoảng ngày bắt đầu - kết thúc và từ khóa.
  - Cho phép xuất toàn bộ dữ liệu đang lọc ra file định dạng CSV chuẩn UTF-8 để mở trong Excel hoặc phần mềm kế toán.
- **Dữ liệu kiểm thử:**
  - Lọc "Trạng thái BLE" -> Hiển thị danh sách các lần kết nối và ngắt kết nối.
  - Bấm "Xuất CSV" -> Tạo file `.csv` chứa đầy đủ tiêu đề cột và nội dung.
- **Kết quả nghiệm thu:** ĐẠT (PASS)

---

## 4. Kết luận bàn giao
Bộ kịch bản Acceptance Test AT-01 đến AT-04 bao phủ 100% các yêu cầu nghiệp vụ trong đề tài. Hệ thống phần mềm và phần cứng tích hợp hoàn toàn thỏa mãn các tiêu chí chấp nhận đưa ra.
