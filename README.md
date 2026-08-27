# Hệ thống kiểm soát ra vào

Ứng dụng quản lý kiểm soát cửa bằng **Qt/C++ trên Raspberry Pi** và giao tiếp trực tiếp với **ESP32 qua Bluetooth Low Energy (BLE)**. Raspberry Pi thực hiện giao diện, xác thực thẻ, quản lý người dùng và lưu lịch sử; ESP32 điều khiển RFID, servo khóa cửa, cảm biến, OLED và buzzer.

Hệ thống hiện vận hành ở chế độ **BLE-only**. Wi-Fi/MQTT đã được loại khỏi ứng dụng Qt để tránh xung đột radio và giữ kết nối BLE ổn định.

## 1. Kiến trúc hệ thống

```text
Máy ảo Ubuntu (phát triển)
  Qt Creator + Qt 6 cross toolchain
             |
             | build ARM64 + SCP/SSH
             v
Raspberry Pi (ứng dụng Qt, BLE Central, SQLite)
             |
             | BLE GATT: TOPIC|PAYLOAD
             v
ESP32 "HE THONG RA VAO HIEU" (BLE Peripheral)
  RFID + servo + cảm biến cửa/vật cản + OLED + buzzer
```

Phân công xử lý:

- **Ứng dụng Qt trên Pi:** đăng nhập, phân quyền, quản lý thẻ RFID, quyết định cấp/từ chối truy cập, điều khiển cửa, cấu hình thời gian đóng cửa và lưu lịch sử.
- **ESP32:** đọc phần cứng, quảng bá BLE, gửi UID/trạng thái lên Pi và nhận lệnh điều khiển cửa.
- **Máy ảo Ubuntu:** chứa source Qt và bộ cross-compile ARM64; không chạy binary ARM64 trực tiếp.

## 2. Chức năng chính

- Đăng nhập và phân quyền quản trị viên/người dùng.
- Tạo, sửa, xóa tài khoản và đổi mật khẩu.
- Quản lý danh sách thẻ RFID.
- Quét và chọn đúng thiết bị BLE trước khi kết nối.
- Tự thử kết nối lại khi BLE bị ngắt.
- Nhận UID RFID, trạng thái ESP32, cảm biến và trạng thái cửa.
- Gửi lệnh mở/đóng cửa thủ công.
- Tự động gửi kết quả `GRANTED` hoặc `DENIED` về ESP32.
- Cấu hình thời gian tự đóng cửa từ 1 đến 300 giây.
- Lưu lịch sử ra/vào bằng SQLite và xuất CSV.

Tài khoản tạo lần đầu:

```text
Tên đăng nhập: admin
Mật khẩu:      123456
Quyền:         Quản trị viên
```

Nên đổi mật khẩu mặc định ngay sau lần đăng nhập đầu tiên.

## 3. Cấu trúc thư mục

```text
.
├── CMakeLists.txt              # Khai báo project và dependency Qt
├── CMakeLists.txt.user         # Kit Qt Creator ARM64 của máy ảo hiện tại
├── README.md                   # Tài liệu tổng thể
├── include/                    # Header C++
├── src/                        # Mã nguồn ứng dụng Qt
├── ui/                         # Giao diện Qt Designer
├── scripts/
│   └── deploy_to_pi.sh         # Build ARM64, chép và chạy trên Pi
├── tests/
│   └── esp32_mock/             # Unit test C dùng mock, không phải firmware thật
├── testqtcreator/              # Qt Test cho cấu hình, SQLite và nghiệp vụ dữ liệu
├── docs/
│   └── test-reports/           # Báo cáo kiểm thử đã tạo
└── build-pi/                   # Sản phẩm cross-build, được CMake tạo lại
```

Firmware ESP32 thật hiện được quản lý ở project PlatformIO riêng trên Windows:

```text
C:\Users\admin\Documents\PlatformIO\Projects\RFID
```

Không dùng mã trong `tests/esp32_mock/` để nạp lên ESP32.

## 4. Giao thức BLE

ESP32 quảng bá với tên:

```text
HE THONG RA VAO HIEU
```

Các UUID:

| Thành phần | UUID | Hướng |
|---|---|---|
| Service | `7d9a0001-6f71-4b88-9a5e-40cba20d1000` | Dịch vụ chính |
| Characteristic Notify | `7d9a0002-6f71-4b88-9a5e-40cba20d1000` | ESP32 → Pi |
| Characteristic Write | `7d9a0003-6f71-4b88-9a5e-40cba20d1000` | Pi → ESP32 |

Mỗi frame có định dạng một dòng:

```text
TOPIC|PAYLOAD
```

Các topic ứng dụng đang xử lý:

| Topic | Hướng | Ý nghĩa |
|---|---|---|
| `access/rfid` | ESP32 → Pi | UID thẻ vừa quét |
| `access/sensor` | ESP32 → Pi | Trạng thái cảm biến vật cản |
| `access/status` | ESP32 → Pi | Trạng thái hoạt động ESP32 |
| `access/door/status` | ESP32 → Pi | `OPENED` hoặc `CLOSED` |
| `access/door/command` | Pi → ESP32 | `OPEN` hoặc `CLOSE` |
| `access/result` | Pi → ESP32 | `GRANTED` hoặc `DENIED` |
| `access/config/door_timeout` | Pi → ESP32 | Số giây tự đóng cửa |

Ứng dụng không yêu cầu pair/bonding. Nó dừng discovery trước khi kết nối, chờ adapter ổn định rồi mới tạo GATT controller và subscribe Notify.

## 5. Dữ liệu khi chạy trên Raspberry Pi

Binary được triển khai tại:

```text
/home/pi/he-thong-kiem-soat-ra-vao/Hethongkiemsoatravao
```

Các file runtime nằm cạnh binary:

```text
/home/pi/he-thong-kiem-soat-ra-vao/data/hethongkiemsoat.sqlite
/home/pi/he-thong-kiem-soat-ra-vao/config/cauhinh.json
/home/pi/hethongkiemsoatravao.log
```

SQLite có ba bảng chính: `tai_khoan`, `the_rfid` và `lich_su_ra_vao`. Mật khẩu được lưu dưới dạng SHA-256; đây là mức bảo vệ hiện tại của ứng dụng, chưa phải cơ chế băm mật khẩu có salt chuyên dụng.

## 6. Cross-compile ARM64

Các đường dẫn toolchain đang dùng trên máy ảo:

```text
Toolchain: /home/pi/Qt6Cross/qt6/pi-build/toolchain.cmake
Host Qt:   /home/pi/Qt6Cross/qt6/host
Target Qt: /home/pi/Qt6Cross/qt6/pi
Build:     ./build-pi
```

Cấu hình thủ công:

```bash
cmake -S . -B build-pi -GNinja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_TOOLCHAIN_FILE=/home/pi/Qt6Cross/qt6/pi-build/toolchain.cmake \
  -DQT_HOST_PATH=/home/pi/Qt6Cross/qt6/host \
  -DCMAKE_PREFIX_PATH=/home/pi/Qt6Cross/qt6/pi \
  -DQt6_DIR=/home/pi/Qt6Cross/qt6/pi/lib/cmake/Qt6

cmake --build build-pi --parallel
file build-pi/Hethongkiemsoatravao
```

Kết quả của lệnh `file` phải chứa `ARM aarch64`. Không dùng Qt tại `/usr/lib/x86_64-linux-gnu` vì đó là Qt của máy ảo và không tạo được chương trình chạy trên Pi.

## 7. Build và chạy từ Qt Creator

1. Mở file `CMakeLists.txt` bằng Qt Creator.
2. Chọn cấu hình `RaspberryPi ARM64`.
3. Bấm **Build** hoặc **Run**.
4. Build step gọi `scripts/deploy_to_pi.sh` để build, kiểm tra ARM64, SCP binary và khởi động lại ứng dụng trên Pi.

Qt Creator phải dùng build directory `build-pi`. Nếu xuất hiện đường dẫn Qt6 trong `/usr/lib/x86_64-linux-gnu`, đang chọn nhầm Desktop Kit.

## 8. Triển khai bằng terminal

Pi hiện được cấu hình tại `192.168.137.227`; SSH key phải đăng nhập được cho tài khoản `pi`.

```bash
./scripts/deploy_to_pi.sh
```

Script sẽ dừng nếu binary không phải ARM aarch64. Có thể thay `PI_IP` trong script nếu địa chỉ Pi thay đổi.

## 9. Chạy unit test mô phỏng ESP32

```bash
cd tests/esp32_mock
./run_tests.sh
```

Bộ test này chạy bằng GCC trên máy ảo, kiểm tra logic mock cho cảm biến và kết nối. Một số test Wi-Fi cũ được giữ để tham khảo nhưng không phản ánh cấu hình BLE-only đang triển khai. Báo cáo nằm trong `docs/test-reports/`.

## 10. Xử lý sự cố nhanh

- **Không tìm thấy ESP32:** xác nhận ESP32 đang quảng bá tên `HE THONG RA VAO HIEU`, chưa bị thiết bị khác giữ kết nối và service UUID đúng.
- **BLE lỗi sau khi scan:** không scan liên tục trong lúc controller đang Connecting/Connected; ứng dụng đã có khoảng chờ sau khi dừng scan.
- **BLE chập chờn khi Pi dùng Wi-Fi 2.4 GHz:** ưu tiên cho Pi kết nối Wi-Fi 5 GHz để giảm nhiễu với BLE 2.4 GHz.
- **Servo chỉ kêu nhưng không quay:** kiểm tra nguồn servo, nối chung GND với ESP32 và kiểm tra góc/xung PWM trong firmware.
- **Qt báo thiếu Bluetooth:** kiểm tra Kit ARM64 và `Qt6_DIR`; không cài bù module vào Qt x86_64 để giải quyết cross-build.
- **Chương trình không hiện trên Pi:** xem `/home/pi/hethongkiemsoatravao.log`, phiên đồ họa `DISPLAY=:0` và quyền truy cập X/DBus.

## 11. Lưu ý bảo trì

- Backup cả project Qt này và project PlatformIO trước khi đổi giao thức BLE hoặc chân phần cứng.
- Khi đổi UUID/topic phải sửa đồng thời firmware ESP32 và `src/ketnoible.cpp`/`src/cuasochinh.cpp`.
- Không đưa database runtime hoặc mật khẩu thật vào source control.
- `CMakeLists.txt.user` chứa đường dẫn riêng của máy ảo hiện tại; nếu chuyển máy cần tạo hoặc sửa lại Kit.
