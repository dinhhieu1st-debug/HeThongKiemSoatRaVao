#include <QtTest>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include "cosodulieu.h"

class TestCoSoDuLieu : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    // Database test cases theo slide va TEST_PLAN.md
    void UT_QT_01_login_success();
    void UT_QT_02_login_wrong_password();
    void UT_QT_03_login_user_not_found();
    void UT_QT_04_add_account_valid();
    void UT_QT_05_add_account_duplicate_username();
    void UT_QT_06_add_rfid_card_valid();
    void UT_QT_07_add_rfid_duplicate_uid();
    void UT_QT_08_find_rfid_existing();
    void UT_QT_09_find_rfid_non_existing();
    void UT_QT_10_history_insert_and_retrieve();
    void UT_QT_11_history_filter();
};

void TestCoSoDuLieu::initTestCase()
{
    // Don dep file database cu cua test neu co
    QString appDir = QCoreApplication::applicationDirPath();
    QString dbPath = appDir + "/data/hethongkiemsoat.sqlite";
    if (QFile::exists(dbPath)) {
        QFile::remove(dbPath);
    }

    bool khoiTaoOk = CoSoDuLieu::khoiTao();
    QVERIFY2(khoiTaoOk, "Khoi tao CoSoDuLieu that bai");
}

void TestCoSoDuLieu::cleanupTestCase()
{
    // Giu lai file log / ket qua test
}

void TestCoSoDuLieu::UT_QT_01_login_success()
{
    // Tai khoan admin mac dinh sau khi khoi tao: admin / 123456 theo cosodulieu.cpp
    bool ok = CoSoDuLieu::kiemTraDangNhap("admin", "123456");
    QCOMPARE(ok, true);

    ThongTinTaiKhoan info;
    bool getOk = CoSoDuLieu::dangNhapVaLayThongTin("admin", "123456", info);
    QCOMPARE(getOk, true);
    QCOMPARE(info.tenDangNhap, QString("admin"));
    QCOMPARE(info.quyen, QString("Quan tri vien"));
}

void TestCoSoDuLieu::UT_QT_02_login_wrong_password()
{
    bool ok = CoSoDuLieu::kiemTraDangNhap("admin", "sai_mat_khau_123");
    QCOMPARE(ok, false);
}

void TestCoSoDuLieu::UT_QT_03_login_user_not_found()
{
    bool ok = CoSoDuLieu::kiemTraDangNhap("khong_ton_tai", "123456");
    QCOMPARE(ok, false);
}

void TestCoSoDuLieu::UT_QT_04_add_account_valid()
{
    QString thongBaoLoi;
    bool ok = CoSoDuLieu::themTaiKhoan(
        "nhanvien1",
        "pass123456",
        "Nguyen Van A",
        "NhanVien",
        thongBaoLoi
    );
    QVERIFY2(ok, qPrintable("Them tai khoan that bai: " + thongBaoLoi));

    // Kiem tra dang nhap voi tai khoan vua tao
    bool loginOk = CoSoDuLieu::kiemTraDangNhap("nhanvien1", "pass123456");
    QCOMPARE(loginOk, true);
}

void TestCoSoDuLieu::UT_QT_05_add_account_duplicate_username()
{
    QString thongBaoLoi;
    // Thu them lai tenDangNhap "nhanvien1" da ton tai
    bool ok = CoSoDuLieu::themTaiKhoan(
        "nhanvien1",
        "matkhaukhac",
        "Nguyen Van B",
        "NhanVien",
        thongBaoLoi
    );
    QCOMPARE(ok, false);
    QVERIFY(!thongBaoLoi.isEmpty());
}

void TestCoSoDuLieu::UT_QT_06_add_rfid_card_valid()
{
    QString thongBaoLoi;
    bool ok = CoSoDuLieu::themThe(
        "D72D6303",
        "Nguyen Van Hieu",
        "NV001",
        "KyThuat",
        thongBaoLoi
    );
    QVERIFY2(ok, qPrintable("Them the RFID that bai: " + thongBaoLoi));
}

void TestCoSoDuLieu::UT_QT_07_add_rfid_duplicate_uid()
{
    QString thongBaoLoi;
    // Them lai cung UID "D72D6303"
    bool ok = CoSoDuLieu::themThe(
        "D72D6303",
        "Nguoi Khac",
        "NV002",
        "Khach",
        thongBaoLoi
    );
    QCOMPARE(ok, false);
    QVERIFY(!thongBaoLoi.isEmpty());
}

void TestCoSoDuLieu::UT_QT_08_find_rfid_existing()
{
    ThongTinThe the;
    bool ok = CoSoDuLieu::timTheTheoUid("D72D6303", the);
    QCOMPARE(ok, true);
    QCOMPARE(the.uid, QString("D72D6303"));
    QCOMPARE(the.hoTen, QString("Nguyen Van Hieu"));
    QCOMPARE(the.maSo, QString("NV001"));
}

void TestCoSoDuLieu::UT_QT_09_find_rfid_non_existing()
{
    ThongTinThe the;
    bool ok = CoSoDuLieu::timTheTheoUid("FFFFFFFF", the);
    QCOMPARE(ok, false);
}

void TestCoSoDuLieu::UT_QT_10_history_insert_and_retrieve()
{
    QString thongBaoLoi;
    int idMoi = -1;
    bool ok = CoSoDuLieu::themLichSuRaVao(
        "D72D6303",
        "Nguyen Van Hieu",
        "NV001",
        "CHO PHÉP",
        "ĐÃ MỞ",
        thongBaoLoi,
        &idMoi
    );
    QVERIFY2(ok, qPrintable("Them lich su that bai: " + thongBaoLoi));
    QVERIFY(idMoi > 0);

    // Kiem tra doc lai lich su
    QList<ThongTinLichSu> danhSach = CoSoDuLieu::layLichSuRaVao();
    QVERIFY(!danhSach.isEmpty());
    QCOMPARE(danhSach.first().uid, QString("D72D6303"));
    QCOMPARE(danhSach.first().ketQua, QString("CHO PHÉP"));
}

void TestCoSoDuLieu::UT_QT_11_history_filter()
{
    QString thongBaoLoi;
    // Them ban ghi tu choi
    CoSoDuLieu::themLichSuRaVao(
        "A1B2C3D4",
        "Khong xac dinh",
        "",
        "TU CHOI",
        "ĐÃ ĐÓNG",
        thongBaoLoi
    );

    // Them ban ghi thao tac thu cong
    CoSoDuLieu::themLichSuRaVao(
        "MANUAL",
        "Admin",
        "",
        "MO THU CONG",
        "ĐÃ MỞ",
        thongBaoLoi
    );

    // Loc theo tu khoa
    QList<ThongTinLichSu> locTheoTuKhoa = CoSoDuLieu::timKiemLichSuRaVao("A1B2C3D4", "Tat ca");
    QCOMPARE(locTheoTuKhoa.size(), 1);
    QCOMPARE(locTheoTuKhoa.first().uid, QString("A1B2C3D4"));

    // Loc theo danh muc ket qua "TU CHOI"
    QList<ThongTinLichSu> locTuChoi = CoSoDuLieu::timKiemLichSuRaVao("", "TU CHOI");
    QVERIFY(locTuChoi.size() >= 1);
    for (const auto &item : locTuChoi) {
        QCOMPARE(item.ketQua, QString("TU CHOI"));
    }
}

QTEST_MAIN(TestCoSoDuLieu)
#include "test_cosodulieu.moc"
