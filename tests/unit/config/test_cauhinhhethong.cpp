#include <QtTest>
#include <QCoreApplication>
#include <QFile>
#include "cauhinhhethong.h"

class TestCauHinhHeThong : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    // Config test cases theo slide va TEST_PLAN.md
    void UT_QT_12_config_default();
    void UT_QT_13_timeout_min_boundary();
    void UT_QT_14_timeout_max_boundary();
    void UT_QT_15_timeout_below_min();
    void UT_QT_16_timeout_negative();
    void UT_QT_17_save_and_reload_roundtrip();
};

void TestCauHinhHeThong::init()
{
    // Xoa file cau hinh truoc moi test de dam bao tinh doc lap
    QString path = QuanLyCauHinh::layDuongDanFile();
    if (QFile::exists(path)) {
        QFile::remove(path);
    }
}

void TestCauHinhHeThong::cleanup()
{
    // Don dep sau khi test
}

void TestCauHinhHeThong::UT_QT_12_config_default()
{
    // Khi chua co file cau hinh, he thong phai tra ve gia tri mac dinh la 15 giay
    CauHinhHeThong ch = QuanLyCauHinh::docCauHinh();
    QCOMPARE(ch.thoiGianDongCuaGiay, 15);
}

void TestCauHinhHeThong::UT_QT_13_timeout_min_boundary()
{
    // Bien duoi hop le la 1 giay
    CauHinhHeThong ch;
    ch.thoiGianDongCuaGiay = 1;
    bool ok = QuanLyCauHinh::luuCauHinh(ch);
    QCOMPARE(ok, true);

    CauHinhHeThong chDocLai = QuanLyCauHinh::docCauHinh();
    QCOMPARE(chDocLai.thoiGianDongCuaGiay, 1);
}

void TestCauHinhHeThong::UT_QT_14_timeout_max_boundary()
{
    // Bien tren hop le theo quy chuan giao dien la 300 giay
    CauHinhHeThong ch;
    ch.thoiGianDongCuaGiay = 300;
    bool ok = QuanLyCauHinh::luuCauHinh(ch);
    QCOMPARE(ok, true);

    CauHinhHeThong chDocLai = QuanLyCauHinh::docCauHinh();
    QCOMPARE(chDocLai.thoiGianDongCuaGiay, 300);
}

void TestCauHinhHeThong::UT_QT_15_timeout_below_min()
{
    // Gia tri 0 giay duoi nguong toi thieu -> QuanLyCauHinh phai tu choi luu
    CauHinhHeThong ch;
    ch.thoiGianDongCuaGiay = 0;
    bool ok = QuanLyCauHinh::luuCauHinh(ch);
    QCOMPARE(ok, false);
}

void TestCauHinhHeThong::UT_QT_16_timeout_negative()
{
    // Gia tri am -> QuanLyCauHinh phai tu choi luu
    CauHinhHeThong ch;
    ch.thoiGianDongCuaGiay = -10;
    bool ok = QuanLyCauHinh::luuCauHinh(ch);
    QCOMPARE(ok, false);
}

void TestCauHinhHeThong::UT_QT_17_save_and_reload_roundtrip()
{
    // Kiem tra chu trinh luu va doc lai (Normal path)
    CauHinhHeThong ch;
    ch.thoiGianDongCuaGiay = 25;
    bool ok = QuanLyCauHinh::luuCauHinh(ch);
    QCOMPARE(ok, true);

    CauHinhHeThong chDocLai = QuanLyCauHinh::docCauHinh();
    QCOMPARE(chDocLai.thoiGianDongCuaGiay, 25);
}

QTEST_MAIN(TestCauHinhHeThong)
#include "test_cauhinhhethong.moc"
