# HƯỚNG DẪN BỘ KIỂM THỬ PHÍA QT / C++ (TESTING GUIDE)
## Dự án: Hệ thống kiểm soát ra vào (Raspberry Pi + ESP32 BLE)
*Cơ sở tài liệu: Bám sát nội dung slide "Bài 11 - Quy trình test phần mềm".*

---

## 1. Mục đích của bộ test
Bộ test được thiết kế nhằm chuẩn hóa quy trình kiểm thử chất lượng cho toàn bộ hệ thống kiểm soát ra vào theo đúng quy chuẩn kỹ thuật:
- Đảm bảo tính đúng đắn và độ tin cậy của các hàm xử lý dữ liệu, nghiệp vụ xác thực thẻ, quản lý cấu hình và giao thức BLE.
- Tự động hóa phát hiện hồi quy (regression) khi nâng cấp mã nguồn.
- Kiểm tra khả năng tích hợp thực tế giữa Raspberry Pi và mạch phần cứng ESP32 qua sóng BLE.

---

## 2. Cơ sở lý thuyết bám sát Slide "Bài 11 - Quy trình test phần mềm"
Bộ kiểm thử tuân thủ mô hình phân cấp kiểm thử chuẩn:
```
         ▲
        / \     Acceptance Test (Nghiệm thu theo yêu cầu người dùng)
       /   \
      /     \    System Test (Kiểm thử toàn diện luồng hệ thống)
     /       \
    /         \   Integration Test (Kiểm thử tích hợp giao tiếp Pi ↔ ESP32)
   /           \
  /_____________\  Unit Test (Kiểm thử đơn vị từng module Qt/C++ & ESP32)
```

- **Unit Test**: Kiểm tra từng hàm, từng lớp riêng biệt mà không phụ thuộc vào toàn bộ hệ thống (dùng `QtTest` framework).
- **Integration Test**: Kiểm thử giao tiếp liên module giữa ứng dụng Qt trên Raspberry Pi và firmware ESP32 qua BLE BlueZ (kịch bản IT-01 đến IT-08).
- **Hardware Test**: Kiểm thử chức năng có sự tham gia của linh kiện ngoại vi thật (RC522, LM393, Servo SG90).
- **System Test**: Kịch bản kiểm thử toàn bộ hệ thống từ lúc quẹt thẻ đến khi mở/đóng cửa và ghi nhật ký CSDL.
- **Acceptance Test**: Đánh giá sự đáp ứng với các tiêu chí nghiệm thu phần mềm của người quản trị.

---

## 3. Cấu trúc thư mục kiểm thử
```
tests/
├── README_TEST.md                     # Tài liệu hướng dẫn chi tiết kiểm thử Qt
├── CMakeLists.txt                     # Cấu hình biên dịch các target test bằng Qt6Test
│
├── unit/                              # KIỂM THỬ ĐƠN VỊ (UNIT TEST)
│   ├── database/
│   │   └── test_cosodulieu.cpp        # Test CRUD tài khoản, thẻ RFID, nhật ký lịch sử
│   ├── config/
│   │   └── test_cauhinhhethong.cpp    # Test đọc/lưu file cấu hình, giá trị biên timeout
│   └── ble_protocol/
│       └── test_ble_protocol.cpp      # Test phân tích cú pháp TOPIC|PAYLOAD và ký tự lỗi
│
├── integration/                       # KIỂM THỬ TÍCH HỢP (INTEGRATION TEST)
│   └── test_ble_esp32.cpp             # 8 kịch bản tích hợp Pi ↔ ESP32 (IT-01 -> IT-08)
│
├── system/                            # KIỂM THỬ HỆ THỐNG (SYSTEM TEST)
│   └── SYSTEM_TEST_CASES.md           # 5 kịch bản kiểm thử hệ thống tổng thể (ST-01 -> ST-05)
│
└── acceptance/                        # KIỂM THỬ CHẤP NHẬN (ACCEPTANCE TEST)
    └── ACCEPTANCE_TEST_CASES.md       # 4 tiêu chí chấp nhận nghiệm thu (AT-01 -> AT-04)
```

---

## 4. Ý nghĩa từng file test & Phân loại môi trường

| Tên file test | Module kiểm thử | Cần phần cứng? | Môi trường chạy |
| :--- | :--- | :--- | :--- |
| `test_cosodulieu.cpp` | CSDL SQLite (`CoSoDuLieu`) | **Không** | Máy build Ubuntu hoặc Pi |
| `test_cauhinhhethong.cpp` | Cấu hình JSON (`QuanLyCauHinh`) | **Không** | Máy build Ubuntu hoặc Pi |
| `test_ble_protocol.cpp` | Parser frame (`GiaoThucBle`) | **Không** | Máy build Ubuntu hoặc Pi |
| `test_ble_esp32.cpp` | BLE Tích hợp (IT-01 -> IT-08) | **Có (Pi + ESP32)** | Raspberry Pi thật |
| `SYSTEM_TEST_CASES.md` | Toàn hệ thống (End-to-End) | **Có (Thẻ + Cảm biến)** | Hệ thống vật lý hoàn chỉnh |
| `ACCEPTANCE_TEST_CASES.md` | Nghiệp vụ người dùng | **Có (Màn hình + Chuột)** | Bàn giao nghiệm thu |

---

## 5. Hướng dẫn biên dịch và chạy test

### 5.1. Biên dịch bộ test
Trên máy phát triển Ubuntu (dùng cross-compiler cho Raspberry Pi):
```bash
cd /home/pi/du_an/HeThongKiemSoatRaVao/Hethongkiemsoatravao
cmake --build build-pi -j$(nproc)
```
Các file thực thi sinh ra tại thư mục `build-pi/tests/`:
- `test_cosodulieu`
- `test_cauhinhhethong`
- `test_ble_protocol`
- `test_ble_esp32`

### 5.2. Chạy Unit Test trên Raspberry Pi
Copy các file test sang Raspberry Pi:
```bash
scp build-pi/tests/test_* pi@192.168.137.227:/home/pi/he-thong-kiem-soat-ra-vao/tests_bin/
```
Đăng nhập SSH vào Pi và chạy:
```bash
# Thiết lập biến môi trường thư viện Qt6
export LD_LIBRARY_PATH=/usr/local/qt6/lib:/usr/local/lib
cd /home/pi/he-thong-kiem-soat-ra-vao/tests_bin

# 1. Chạy Unit Test CSDL:
mkdir -p run_db && cd run_db && ../test_cosodulieu && cd ..

# 2. Chạy Unit Test Cấu hình:
mkdir -p run_cfg && cd run_cfg && ../test_cauhinhhethong && cd ..

# 3. Chạy Unit Test Giao thức BLE:
./test_ble_protocol
```

### 5.3. Chạy Integration Test (IT-01 đến IT-08)
Đảm bảo ESP32 đang cấp nguồn và phát sóng BLE (`HE THONG RA VAO HIEU`):
```bash
export LD_LIBRARY_PATH=/usr/local/qt6/lib:/usr/local/lib
cd /home/pi/he-thong-kiem-soat-ra-vao/tests_bin
./test_ble_esp32
```

---

## 6. Cách đọc kết quả kiểm thử & Xử lý trạng thái

### 6.1. Đọc kết quả PASS / FAIL
- `PASS`: Test case thực thi thành công, kết quả thực tế trùng khớp hoàn toàn với Expected Result (`QCOMPARE` hoặc `QVERIFY` thỏa mãn).
- `FAIL`: Có ít nhất một assertion không thỏa mãn, trình kiểm thử dừng tại vị trí lỗi và in rõ dòng mã, giá trị `Actual` vs `Expected`.

### 6.2. Xử lý WAITING FOR MANUAL ACTION
- Trong các kịch bản kiểm thử tích hợp (ví dụ IT-04 quẹt thẻ RFID hoặc thử kích hoạt cảm biến LM393): nếu trong cửa sổ thời gian chờ không có tác động vật lý, test case ghi nhận trạng thái:
  `WAITING FOR MANUAL ACTION: Vui long quet the RFID hoac kich hoat cam bien`
- Người kiểm thử cần thực hiện hành vi vật lý tương ứng để hoàn tất xác minh trước khi đánh dấu ĐẠT.

---

## 7. Vị trí lưu nhật ký kiểm thử (Test Evidence)
Toàn bộ log thực tế từ các lần chạy được lưu giữ tại:
- `test_evidence/qt/unit/test_cosodulieu.log`
- `test_evidence/qt/unit/test_cauhinhhethong.log`
- `test_evidence/qt/unit/test_ble_protocol.log`
- `test_evidence/qt/integration/integration_IT01_IT08.log`
- `test_evidence/TEST_REPORT.md`
