#include "cauhinhhethong.h"
#include "cosodulieu.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QSqlDatabase>
#include <QtTest>

class TestHeThongQt : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void testCauHinhMacDinhVaLuuLai();
    void testKhoiTaoVaDangNhapMacDinh();
    void testQuanLyTaiKhoan();
    void testQuanLyTheRfid();
    void testLichSuRaVao();
    void cleanupTestCase();

private:
    QString thuMucChay() const;
};

QString TestHeThongQt::thuMucChay() const
{
    return QCoreApplication::applicationDirPath();
}

void TestHeThongQt::initTestCase()
{
    if (QSqlDatabase::contains("ket_noi_chinh"))
    {
        {
            QSqlDatabase db =
                QSqlDatabase::database("ket_noi_chinh");
            db.close();
        }

        QSqlDatabase::removeDatabase("ket_noi_chinh");
    }

    QDir(thuMucChay() + "/data").removeRecursively();
    QDir(thuMucChay() + "/config").removeRecursively();
}

void TestHeThongQt::testCauHinhMacDinhVaLuuLai()
{
    QVERIFY(QuanLyCauHinh::khoiTao());

    const QString duongDan = QuanLyCauHinh::layDuongDanFile();
    QVERIFY(QFileInfo::exists(duongDan));
    QVERIFY(duongDan.startsWith(thuMucChay()));

    QCOMPARE(
        QuanLyCauHinh::docCauHinh().thoiGianDongCuaGiay,
        15
    );

    CauHinhHeThong cauHinh;
    cauHinh.thoiGianDongCuaGiay = 45;
    QVERIFY(QuanLyCauHinh::luuCauHinh(cauHinh));
    QCOMPARE(
        QuanLyCauHinh::docCauHinh().thoiGianDongCuaGiay,
        45
    );

    cauHinh.thoiGianDongCuaGiay = 0;
    QVERIFY(!QuanLyCauHinh::luuCauHinh(cauHinh));
    QCOMPARE(
        QuanLyCauHinh::docCauHinh().thoiGianDongCuaGiay,
        45
    );
}

void TestHeThongQt::testKhoiTaoVaDangNhapMacDinh()
{
    QVERIFY(CoSoDuLieu::khoiTao());
    QVERIFY(QFileInfo::exists(
        thuMucChay() + "/data/hethongkiemsoat.sqlite"
    ));

    QVERIFY(CoSoDuLieu::kiemTraDangNhap("admin", "123456"));
    QVERIFY(!CoSoDuLieu::kiemTraDangNhap("admin", "sai-mat-khau"));

    ThongTinTaiKhoan admin;
    QVERIFY(CoSoDuLieu::dangNhapVaLayThongTin(
        "admin",
        "123456",
        admin
    ));
    QCOMPARE(admin.tenDangNhap, QString("admin"));
    QCOMPARE(admin.quyen, QString("Quan tri vien"));
}

void TestHeThongQt::testQuanLyTaiKhoan()
{
    QString loi;

    QVERIFY(CoSoDuLieu::themTaiKhoan(
        "nhanvien_test",
        "abcdef",
        "Nhan vien thu nghiem",
        "Nhan vien",
        loi
    ));

    QVERIFY(!CoSoDuLieu::themTaiKhoan(
        "nhanvien_test",
        "abcdef",
        "Trung tai khoan",
        "Nhan vien",
        loi
    ));

    int idNhanVien = -1;

    for (const ThongTinTaiKhoan &taiKhoan :
         CoSoDuLieu::layDanhSachTaiKhoan())
    {
        if (taiKhoan.tenDangNhap == "nhanvien_test")
        {
            idNhanVien = taiKhoan.id;
            break;
        }
    }

    QVERIFY(idNhanVien > 0);
    QVERIFY(CoSoDuLieu::capNhatTaiKhoan(
        idNhanVien,
        "Nhan vien da cap nhat",
        "Nhan vien",
        loi
    ));
    QVERIFY(CoSoDuLieu::doiMatKhauTaiKhoan(
        idNhanVien,
        "matkhau-moi",
        loi
    ));
    QVERIFY(CoSoDuLieu::kiemTraDangNhap(
        "nhanvien_test",
        "matkhau-moi"
    ));

    QVERIFY(!CoSoDuLieu::xoaTaiKhoan(
        idNhanVien,
        "nhanvien_test",
        loi
    ));
    QVERIFY(CoSoDuLieu::xoaTaiKhoan(idNhanVien, "admin", loi));
    QVERIFY(!CoSoDuLieu::kiemTraDangNhap(
        "nhanvien_test",
        "matkhau-moi"
    ));
}

void TestHeThongQt::testQuanLyTheRfid()
{
    QString loi;

    QVERIFY(CoSoDuLieu::themThe(
        "d72d6303",
        "Hieu",
        "SV001",
        "Sinh vien",
        loi
    ));
    QVERIFY(!CoSoDuLieu::themThe(
        "D72D6303",
        "The trung",
        "SV999",
        "Sinh vien",
        loi
    ));

    ThongTinThe thongTin;
    QVERIFY(CoSoDuLieu::timTheTheoUid("d72d6303", thongTin));
    QCOMPARE(thongTin.uid, QString("D72D6303"));
    QCOMPARE(thongTin.maSo, QString("SV001"));

    QVERIFY(CoSoDuLieu::capNhatThe(
        "D72D6303",
        "Hieu cap nhat",
        "SV002",
        "Sinh vien",
        loi
    ));
    QCOMPARE(CoSoDuLieu::timKiemThe("SV002").size(), 1);
}

void TestHeThongQt::testLichSuRaVao()
{
    QString loi;
    int idLichSuChoXacNhan = -1;

    QVERIFY(CoSoDuLieu::themLichSuRaVao(
        "D72D6303",
        "Hieu cap nhat",
        "SV002",
        "GRANTED",
        "CHO XAC NHAN",
        loi,
        &idLichSuChoXacNhan
    ));
    QVERIFY(idLichSuChoXacNhan > 0);
    QVERIFY(CoSoDuLieu::capNhatTrangThaiCuaLichSu(
        idLichSuChoXacNhan,
        "OPENED",
        loi
    ));
    QVERIFY(CoSoDuLieu::themLichSuRaVao(
        "UNKNOWN01",
        "",
        "",
        "DENIED",
        "CLOSED",
        loi
    ));

    QCOMPARE(CoSoDuLieu::layLichSuRaVao().size(), 2);
    QCOMPARE(
        CoSoDuLieu::layLichSuRaVao().at(1).trangThaiCua,
        QString("OPENED")
    );
    QCOMPARE(
        CoSoDuLieu::timKiemLichSuRaVao("D72D", "GRANTED").size(),
        1
    );
    QCOMPARE(
        CoSoDuLieu::timKiemLichSuRaVao("", "DENIED").size(),
        1
    );

    QVERIFY(CoSoDuLieu::xoaThe("D72D6303", loi));
    ThongTinThe daXoa;
    QVERIFY(!CoSoDuLieu::timTheTheoUid("D72D6303", daXoa));
}

void TestHeThongQt::cleanupTestCase()
{
    {
        QSqlDatabase db = QSqlDatabase::database("ket_noi_chinh");
        db.close();
    }

    QSqlDatabase::removeDatabase("ket_noi_chinh");
    QDir(thuMucChay() + "/data").removeRecursively();
    QDir(thuMucChay() + "/config").removeRecursively();
}

QTEST_GUILESS_MAIN(TestHeThongQt)

#include "test_he_thong_qt.moc"
