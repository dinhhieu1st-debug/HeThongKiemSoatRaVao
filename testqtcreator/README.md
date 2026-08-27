# Bộ test hệ thống Qt

Bộ test dùng Qt Test chạy native trên máy ảo Ubuntu. Dữ liệu thử nghiệm được tạo trong `testqtcreator/build/` rồi tự xóa khi hoàn tất, không tác động dữ liệu trên Raspberry Pi.

Phạm vi kiểm tra:

- Tạo, đọc và ghi cấu hình JSON.
- Khởi tạo SQLite và tài khoản quản trị mặc định.
- Đăng nhập đúng/sai mật khẩu.
- Thêm, sửa, đổi mật khẩu và xóa tài khoản.
- Thêm, tìm kiếm, cập nhật và xóa thẻ RFID.
- Ghi, đọc và lọc lịch sử ra/vào.

Chạy toàn bộ test từ thư mục gốc project:

```bash
./testqtcreator/run_tests.sh
```

Hoặc chạy từng bước:

```bash
cmake -S testqtcreator -B testqtcreator/build -GNinja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DQt6_DIR=/usr/lib/x86_64-linux-gnu/cmake/Qt6
cmake --build testqtcreator/build --parallel
ctest --test-dir testqtcreator/build --output-on-failure
```

Chạy trực tiếp executable để xem chi tiết từng hàm test:

```bash
./testqtcreator/build/test_he_thong_qt -v2
```
