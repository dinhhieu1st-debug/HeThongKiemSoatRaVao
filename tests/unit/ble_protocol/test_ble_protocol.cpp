#include <QtTest>
#include "giaothucble.h"

class TestBleProtocol : public QObject
{
    Q_OBJECT

private slots:
    // BLE Protocol Parser & Formatter test cases theo slide va TEST_PLAN.md
    void UT_QT_18_parse_valid_single_frame();
    void UT_QT_19_parse_valid_multiline_frame();
    void UT_QT_20_parse_missing_separator();
    void UT_QT_21_parse_empty_topic();
    void UT_QT_22_parse_empty_payload();
    void UT_QT_23_format_valid_frame();
    void UT_QT_24_format_forbidden_chars_separator();
    void UT_QT_25_format_forbidden_chars_newline();
    void UT_QT_26_format_forbidden_chars_carriage_return();
    void UT_QT_27_format_empty_topic();
};

void TestBleProtocol::UT_QT_18_parse_valid_single_frame()
{
    QByteArray rawFrame = "access/door/command|OPEN";
    QList<QPair<QString, QString>> danhSachGoi;
    QString thongBaoLoi;

    bool ok = GiaoThucBle::phanTichFrame(rawFrame, danhSachGoi, thongBaoLoi);
    QCOMPARE(ok, true);
    QCOMPARE(danhSachGoi.size(), 1);
    QCOMPARE(danhSachGoi.first().first, QString("access/door/command"));
    QCOMPARE(danhSachGoi.first().second, QString("OPEN"));
}

void TestBleProtocol::UT_QT_19_parse_valid_multiline_frame()
{
    QByteArray rawFrame = "access/rfid|D72D6303\naccess/sensor|OBSTACLE\n";
    QList<QPair<QString, QString>> danhSachGoi;
    QString thongBaoLoi;

    bool ok = GiaoThucBle::phanTichFrame(rawFrame, danhSachGoi, thongBaoLoi);
    QCOMPARE(ok, true);
    QCOMPARE(danhSachGoi.size(), 2);
    QCOMPARE(danhSachGoi.at(0).first, QString("access/rfid"));
    QCOMPARE(danhSachGoi.at(0).second, QString("D72D6303"));
    QCOMPARE(danhSachGoi.at(1).first, QString("access/sensor"));
    QCOMPARE(danhSachGoi.at(1).second, QString("OBSTACLE"));
}

void TestBleProtocol::UT_QT_20_parse_missing_separator()
{
    QByteArray rawFrame = "access/door/commandOPEN";
    QList<QPair<QString, QString>> danhSachGoi;
    QString thongBaoLoi;

    bool ok = GiaoThucBle::phanTichFrame(rawFrame, danhSachGoi, thongBaoLoi);
    QCOMPARE(ok, false);
    QVERIFY(!thongBaoLoi.isEmpty());
}

void TestBleProtocol::UT_QT_21_parse_empty_topic()
{
    QByteArray rawFrame = "|OPEN";
    QList<QPair<QString, QString>> danhSachGoi;
    QString thongBaoLoi;

    bool ok = GiaoThucBle::phanTichFrame(rawFrame, danhSachGoi, thongBaoLoi);
    QCOMPARE(ok, false);
}

void TestBleProtocol::UT_QT_22_parse_empty_payload()
{
    QByteArray rawFrame = "access/door/command|";
    QList<QPair<QString, QString>> danhSachGoi;
    QString thongBaoLoi;

    bool ok = GiaoThucBle::phanTichFrame(rawFrame, danhSachGoi, thongBaoLoi);
    QCOMPARE(ok, false);
}

void TestBleProtocol::UT_QT_23_format_valid_frame()
{
    QString topic = "access/config/door_timeout";
    QString payload = "15";
    QByteArray frame;
    QString thongBaoLoi;

    bool ok = GiaoThucBle::dongGoiFrame(topic, payload, frame, thongBaoLoi);
    QCOMPARE(ok, true);
    QCOMPARE(frame, QByteArray("access/config/door_timeout|15"));
}

void TestBleProtocol::UT_QT_24_format_forbidden_chars_separator()
{
    QString topic = "access/test";
    QString payload = "VAL|123";
    QByteArray frame;
    QString thongBaoLoi;

    bool ok = GiaoThucBle::dongGoiFrame(topic, payload, frame, thongBaoLoi);
    QCOMPARE(ok, false);
    QVERIFY(!thongBaoLoi.isEmpty());
}

void TestBleProtocol::UT_QT_25_format_forbidden_chars_newline()
{
    QString topic = "access/test";
    QString payload = "VAL\n123";
    QByteArray frame;
    QString thongBaoLoi;

    bool ok = GiaoThucBle::dongGoiFrame(topic, payload, frame, thongBaoLoi);
    QCOMPARE(ok, false);
}

void TestBleProtocol::UT_QT_26_format_forbidden_chars_carriage_return()
{
    QString topic = "access/test";
    QString payload = "VAL\r123";
    QByteArray frame;
    QString thongBaoLoi;

    bool ok = GiaoThucBle::dongGoiFrame(topic, payload, frame, thongBaoLoi);
    QCOMPARE(ok, false);
}

void TestBleProtocol::UT_QT_27_format_empty_topic()
{
    QString topic = "";
    QString payload = "OPEN";
    QByteArray frame;
    QString thongBaoLoi;

    bool ok = GiaoThucBle::dongGoiFrame(topic, payload, frame, thongBaoLoi);
    QCOMPARE(ok, false);
}

QTEST_MAIN(TestBleProtocol)
#include "test_ble_protocol.moc"
