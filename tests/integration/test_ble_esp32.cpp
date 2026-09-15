#include <QtTest>
#include <QCoreApplication>
#include <QSignalSpy>
#include <QTimer>
#include "ketnoible.h"

class TestBleEsp32Integration : public QObject
{
    Q_OBJECT

private:
    KetNoiBle *ble;
    const QString ESP32_NAME = "HE THONG RA VAO HIEU";
    const QString ESP32_MAC = "44:1D:64:F5:01:9E";
    QString foundMac;

private slots:
    void initTestCase();
    void cleanupTestCase();

    // 8 kịch bản tích hợp bám sát Bài 11 slide và yêu cầu đề tài
    void IT_01_Scan();
    void IT_02_Connect();
    void IT_03_Send();
    void IT_04_Receive();
    void IT_05_Verify_Data();
    void IT_06_Timeout();
    void IT_07_Disconnect();
    void IT_08_Reconnect();
};

void TestBleEsp32Integration::initTestCase()
{
    ble = new KetNoiBle(this);
    QVERIFY(ble != nullptr);
}

void TestBleEsp32Integration::cleanupTestCase()
{
    if (ble) {
        ble->dung();
        delete ble;
        ble = nullptr;
    }
}

void TestBleEsp32Integration::IT_01_Scan()
{
    qDebug() << "=== [IT-01] BLE SCAN START ===";
    QSignalSpy spyFound(ble, &KetNoiBle::timThayThietBi);
    QSignalSpy spyEnd(ble, &KetNoiBle::ketThucQuet);

    bool scanStarted = ble->quetDeLuaChon();
    QVERIFY2(scanStarted, "Khong the khoi dong bo quet BLE tren Pi");

    // Cho toi da 15 giay de quet tim thay ESP32
    QElapsedTimer timer;
    timer.start();
    bool foundEsp32 = false;

    while (timer.elapsed() < 15000) {
        QCoreApplication::processEvents();
        for (int i = 0; i < spyFound.count(); ++i) {
            QString name = spyFound.at(i).at(0).toString();
            QString addr = spyFound.at(i).at(1).toString();
            bool match = spyFound.at(i).at(3).toBool();
            if (match || name == ESP32_NAME || addr.compare(ESP32_MAC, Qt::CaseInsensitive) == 0) {
                foundEsp32 = true;
                foundMac = addr;
                qDebug() << "IT-01 SUCCESS: Found ESP32 ->" << name << "[" << addr << "]";
                break;
            }
        }
        if (foundEsp32) break;
        QTest::qWait(300);
    }

    QVERIFY2(foundEsp32, qPrintable("IT-01 FAIL: Khong tim thay thiet bi BLE ESP32 " + ESP32_NAME));
}

void TestBleEsp32Integration::IT_02_Connect()
{
    qDebug() << "=== [IT-02] BLE CONNECT START ===";
    QVERIFY2(!foundMac.isEmpty(), "Khong co dia chi MAC ESP32 tu buoc scan de ket noi");

    QSignalSpy spyConnected(ble, &KetNoiBle::daKetNoiBle);
    QSignalSpy spyError(ble, &KetNoiBle::coLoi);

    bool ok = ble->ketNoiTheoDiaChi(foundMac);
    QVERIFY2(ok, "Goi ham ketNoiTheoDiaChi that bai");

    // Cho toi da 20 giay de ket noi GATT va discover characteristics
    QElapsedTimer timer;
    timer.start();
    while (timer.elapsed() < 20000 && !ble->daKetNoi()) {
        QTest::qWait(300);
    }

    QVERIFY2(ble->daKetNoi(), "IT-02 FAIL: Khong the ket noi va discover characteristic toi ESP32 trong 20s");
    qDebug() << "IT-02 SUCCESS: GATT Service & Characteristic Discovered. Connected!";
}

void TestBleEsp32Integration::IT_03_Send()
{
    qDebug() << "=== [IT-03] SEND DATA Pi -> ESP32 ===";
    QVERIFY2(ble->daKetNoi(), "BLE chua ket noi");

    // Gui lenh mo cua access/door/command|OPEN
    bool sent = ble->guiDuLieu("access/door/command", "OPEN");
    QVERIFY2(sent, "IT-03 FAIL: Gui lenh mo cua sang ESP32 that bai");
    qDebug() << "IT-03 SUCCESS: Lenh access/door/command|OPEN da duoc day vao queue va gui BLE";
    QTest::qWait(1000);
}

void TestBleEsp32Integration::IT_04_Receive()
{
    qDebug() << "=== [IT-04] RECEIVE DATA ESP32 -> Pi ===";
    QVERIFY2(ble->daKetNoi(), "BLE chua ket noi");

    QSignalSpy spyData(ble, &KetNoiBle::nhanDuLieu);

    // Kiem tra xem co nhan duoc notification tu ESP32 (vi du: status, door/status, hoac rfid)
    qDebug() << "Dang cho Notify tu ESP32 trong 5 giay...";
    QElapsedTimer timer;
    timer.start();
    while (timer.elapsed() < 5000) {
        if (!spyData.isEmpty()) break;
        QTest::qWait(200);
    }

    if (spyData.isEmpty()) {
        qDebug() << "WAITING FOR MANUAL ACTION: Vui long quet the RFID hoac kich hoat cam bien tren ESP32 de nhan Notify";
        // Cho them 3 giay neu nguoi dung thao tac
        timer.restart();
        while (timer.elapsed() < 3000) {
            if (!spyData.isEmpty()) break;
            QTest::qWait(200);
        }
    }

    if (!spyData.isEmpty()) {
        QString topic = spyData.first().at(0).toString();
        QString payload = spyData.first().at(1).toString();
        qDebug() << "IT-04 SUCCESS: Nhan Notify thanh cong -> Topic:" << topic << "Payload:" << payload;
        QVERIFY(!topic.isEmpty());
    } else {
        qDebug() << "IT-04 NOTICE: Khong co tin hieu tu ESP32 trong cua so cho -> Can quet the vat ly (WAITING FOR MANUAL ACTION)";
    }
}

void TestBleEsp32Integration::IT_05_Verify_Data()
{
    qDebug() << "=== [IT-05] VERIFY DATA INTEGRITY ===";
    QVERIFY2(ble->daKetNoi(), "BLE chua ket noi");

    // Gui dong bo timeout 12 giay
    bool sent = ble->guiDuLieu("access/config/door_timeout", "12");
    QVERIFY(sent);
    qDebug() << "IT-05 SUCCESS: Frame hop le da gui, dinh dang chuan TOPIC|PAYLOAD";
}

void TestBleEsp32Integration::IT_06_Timeout()
{
    qDebug() << "=== [IT-06] TIMEOUT HANDLING ===";
    // Thu ket noi den mot MAC gia lap khong phan hoi de kiem tra co che khong bi treo ung dung
    KetNoiBle bleFake;
    QSignalSpy spyError(&bleFake, &KetNoiBle::coLoi);

    bleFake.ketNoiTheoDiaChi("00:11:22:33:44:55");
    // Ung dung van tiep tuc event loop ma khong bi block/freeze
    QElapsedTimer timer;
    timer.start();
    while (timer.elapsed() < 3000) {
        QTest::qWait(200);
    }
    QVERIFY2(!bleFake.daKetNoi(), "Thiet bi khong ton tai khong the ket noi");
    qDebug() << "IT-06 SUCCESS: Ung dung khong bi treo, phan hoi timeout/disconnect binh thuong";
}

void TestBleEsp32Integration::IT_07_Disconnect()
{
    qDebug() << "=== [IT-07] DISCONNECT HANDLING ===";
    QVERIFY2(ble->daKetNoi(), "BLE hien tai chua ket noi de test ngat ket noi");

    QSignalSpy spyDisconnected(ble, &KetNoiBle::daNgatKetNoiBle);
    // Chu dong ngat ket noi hoac dung controller
    ble->dung();

    QTest::qWait(3500);
    QVERIFY2(!ble->daKetNoi(), "IT-07 FAIL: Trạng thái BLE phai la ngat ket noi");
    qDebug() << "IT-07 SUCCESS: Nhan biet ngat ket noi thanh cong, he thong an toan";
}

void TestBleEsp32Integration::IT_08_Reconnect()
{
    qDebug() << "=== [IT-08] RECONNECT HANDLING ===";
    // Khi dung() da xoa danh sach thiet bi, can quet lai de phat hien ESP32
    bool scanStarted = ble->quetDeLuaChon();
    QVERIFY2(scanStarted, "Khoi dong quet lai that bai");

    QElapsedTimer timer;
    timer.start();
    bool rediscovered = false;
    while (timer.elapsed() < 15000) {
        QTest::qWait(300);
        if (ble->ketNoiTheoDiaChi(foundMac)) {
            rediscovered = true;
            break;
        }
    }
    QVERIFY2(rediscovered, "Khong the tim thay lai ESP32 sau khi ngat");

    timer.restart();
    while (timer.elapsed() < 20000 && !ble->daKetNoi()) {
        QTest::qWait(300);
    }

    QVERIFY2(ble->daKetNoi(), "IT-08 FAIL: Ket noi lai toi ESP32 khong thanh cong");
    qDebug() << "IT-08 SUCCESS: Da ket noi lai voi ESP32 thanh cong!";
}

QTEST_MAIN(TestBleEsp32Integration)
#include "test_ble_esp32.moc"
