#include "cosodulieu.h"

#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDateTime>
#include <QDir>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

namespace
{

QString maHoaMatKhau(const QString &matKhau)
{
    // Ma hoa mat khau bang SHA-256
    return QString(
        QCryptographicHash::hash(
            matKhau.toUtf8(),
            QCryptographicHash::Sha256
        ).toHex()
    );
}

QString duongDanCoSoDuLieu()
{
    // Luu co so du lieu trong thu muc data
    const QString thuMucChuongTrinh =
        QCoreApplication::applicationDirPath();

    QDir thuMuc(thuMucChuongTrinh);

    if (!thuMuc.exists("data"))
    {
        thuMuc.mkdir("data");
    }

    return thuMuc.filePath(
        "data/hethongkiemsoat.sqlite"
    );
}

ThongTinTaiKhoan docThongTinTaiKhoan(
    const QSqlQuery &truyVan
)
{
    ThongTinTaiKhoan thongTin;

    thongTin.id =
        truyVan.value("id").toInt();

    thongTin.tenDangNhap =
        truyVan.value("ten_dang_nhap").toString();

    thongTin.hoTen =
        truyVan.value("ho_ten").toString();

    thongTin.quyen =
        truyVan.value("quyen").toString();

    thongTin.ngayTao =
        truyVan.value("ngay_tao").toString();

    return thongTin;
}

ThongTinThe docThongTinThe(
    const QSqlQuery &truyVan
)
{
    ThongTinThe thongTin;

    thongTin.id =
        truyVan.value("id").toInt();

    thongTin.uid =
        truyVan.value("uid").toString();

    thongTin.hoTen =
        truyVan.value("ho_ten").toString();

    thongTin.maSo =
        truyVan.value("ma_so").toString();

    thongTin.vaiTro =
        truyVan.value("vai_tro").toString();

    thongTin.ngayDangKy =
        truyVan.value("ngay_dang_ky").toString();

    return thongTin;
}

ThongTinLichSu docThongTinLichSu(
    const QSqlQuery &truyVan
)
{
    ThongTinLichSu thongTin;

    thongTin.id =
        truyVan.value("id").toInt();

    thongTin.thoiGian =
        truyVan.value("thoi_gian").toString();

    thongTin.uid =
        truyVan.value("uid").toString();

    thongTin.hoTen =
        truyVan.value("ho_ten").toString();

    thongTin.maSo =
        truyVan.value("ma_so").toString();

    thongTin.ketQua =
        truyVan.value("ket_qua").toString();

    thongTin.trangThaiCua =
        truyVan.value("trang_thai_cua").toString();

    return thongTin;
}

}

bool CoSoDuLieu::khoiTao()
{
    QSqlDatabase coSoDuLieu;

    if (QSqlDatabase::contains("ket_noi_chinh"))
    {
        coSoDuLieu =
            QSqlDatabase::database("ket_noi_chinh");
    }
    else
    {
        coSoDuLieu =
            QSqlDatabase::addDatabase(
                "QSQLITE",
                "ket_noi_chinh"
            );

        coSoDuLieu.setDatabaseName(
            duongDanCoSoDuLieu()
        );
    }

    if (!coSoDuLieu.open())
    {
        return false;
    }

    if (!taoBangTaiKhoan())
    {
        return false;
    }

    if (!taoBangTheRfid())
    {
        return false;
    }

    if (!taoBangLichSuRaVao())
    {
        return false;
    }

    if (!taoTaiKhoanMacDinh())
    {
        return false;
    }

    return true;
}

bool CoSoDuLieu::taoBangTaiKhoan()
{
    QSqlDatabase coSoDuLieu =
        QSqlDatabase::database("ket_noi_chinh");

    QSqlQuery truyVan(coSoDuLieu);

    const QString cauLenh =
        "CREATE TABLE IF NOT EXISTS tai_khoan ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "ten_dang_nhap TEXT UNIQUE NOT NULL,"
        "mat_khau TEXT NOT NULL,"
        "ho_ten TEXT NOT NULL,"
        "quyen TEXT NOT NULL,"
        "ngay_tao TEXT NOT NULL"
        ")";

    return truyVan.exec(cauLenh);
}

bool CoSoDuLieu::taoBangTheRfid()
{
    QSqlDatabase coSoDuLieu =
        QSqlDatabase::database("ket_noi_chinh");

    QSqlQuery truyVan(coSoDuLieu);

    const QString cauLenh =
        "CREATE TABLE IF NOT EXISTS the_rfid ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "uid TEXT UNIQUE NOT NULL,"
        "ho_ten TEXT NOT NULL,"
        "ma_so TEXT NOT NULL,"
        "vai_tro TEXT NOT NULL,"
        "ngay_dang_ky TEXT NOT NULL"
        ")";

    return truyVan.exec(cauLenh);
}

bool CoSoDuLieu::taoBangLichSuRaVao()
{
    QSqlDatabase coSoDuLieu =
        QSqlDatabase::database("ket_noi_chinh");

    QSqlQuery truyVan(coSoDuLieu);

    const QString cauLenh =
        "CREATE TABLE IF NOT EXISTS lich_su_ra_vao ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "thoi_gian TEXT NOT NULL,"
        "uid TEXT NOT NULL,"
        "ho_ten TEXT,"
        "ma_so TEXT,"
        "ket_qua TEXT NOT NULL,"
        "trang_thai_cua TEXT NOT NULL"
        ")";

    return truyVan.exec(cauLenh);
}

bool CoSoDuLieu::taoTaiKhoanMacDinh()
{
    QSqlDatabase coSoDuLieu =
        QSqlDatabase::database("ket_noi_chinh");

    QSqlQuery truyVan(coSoDuLieu);

    truyVan.prepare(
        "SELECT COUNT(*) "
        "FROM tai_khoan "
        "WHERE ten_dang_nhap = :ten_dang_nhap"
    );

    truyVan.bindValue(
        ":ten_dang_nhap",
        "admin"
    );

    if (!truyVan.exec() || !truyVan.next())
    {
        return false;
    }

    if (truyVan.value(0).toInt() > 0)
    {
        return true;
    }

    QString thongBaoLoi;

    return themTaiKhoan(
        "admin",
        "123456",
        "Quan tri vien mac dinh",
        "Quan tri vien",
        thongBaoLoi
    );
}

bool CoSoDuLieu::kiemTraDangNhap(
    const QString &tenDangNhap,
    const QString &matKhau
)
{
    ThongTinTaiKhoan thongTinTaiKhoan;

    return dangNhapVaLayThongTin(
        tenDangNhap,
        matKhau,
        thongTinTaiKhoan
    );
}

bool CoSoDuLieu::dangNhapVaLayThongTin(
    const QString &tenDangNhap,
    const QString &matKhau,
    ThongTinTaiKhoan &thongTinTaiKhoan
)
{
    QSqlDatabase coSoDuLieu =
        QSqlDatabase::database("ket_noi_chinh");

    if (!coSoDuLieu.isOpen())
    {
        return false;
    }

    QSqlQuery truyVan(coSoDuLieu);

    truyVan.prepare(
        "SELECT "
        "id,"
        "ten_dang_nhap,"
        "ho_ten,"
        "quyen,"
        "ngay_tao "
        "FROM tai_khoan "
        "WHERE ten_dang_nhap = :ten_dang_nhap "
        "AND mat_khau = :mat_khau"
    );

    truyVan.bindValue(
        ":ten_dang_nhap",
        tenDangNhap.trimmed()
    );

    truyVan.bindValue(
        ":mat_khau",
        maHoaMatKhau(matKhau)
    );

    if (!truyVan.exec())
    {
        return false;
    }

    if (!truyVan.next())
    {
        return false;
    }

    thongTinTaiKhoan =
        docThongTinTaiKhoan(truyVan);

    return true;
}

bool CoSoDuLieu::themTaiKhoan(
    const QString &tenDangNhap,
    const QString &matKhau,
    const QString &hoTen,
    const QString &quyen,
    QString &thongBaoLoi
)
{
    QSqlDatabase coSoDuLieu =
        QSqlDatabase::database("ket_noi_chinh");

    if (!coSoDuLieu.isOpen())
    {
        thongBaoLoi =
            "Co so du lieu chua duoc mo";

        return false;
    }

    QSqlQuery truyVan(coSoDuLieu);

    truyVan.prepare(
        "INSERT INTO tai_khoan ("
        "ten_dang_nhap,"
        "mat_khau,"
        "ho_ten,"
        "quyen,"
        "ngay_tao"
        ") VALUES ("
        ":ten_dang_nhap,"
        ":mat_khau,"
        ":ho_ten,"
        ":quyen,"
        ":ngay_tao"
        ")"
    );

    truyVan.bindValue(
        ":ten_dang_nhap",
        tenDangNhap.trimmed()
    );

    truyVan.bindValue(
        ":mat_khau",
        maHoaMatKhau(matKhau)
    );

    truyVan.bindValue(
        ":ho_ten",
        hoTen.trimmed()
    );

    truyVan.bindValue(
        ":quyen",
        quyen.trimmed()
    );

    truyVan.bindValue(
        ":ngay_tao",
        QDateTime::currentDateTime()
            .toString("yyyy-MM-dd HH:mm:ss")
    );

    if (!truyVan.exec())
    {
        thongBaoLoi =
            truyVan.lastError().text();

        return false;
    }

    return true;
}

QList<ThongTinTaiKhoan>
CoSoDuLieu::layDanhSachTaiKhoan()
{
    QList<ThongTinTaiKhoan> danhSach;

    QSqlDatabase coSoDuLieu =
        QSqlDatabase::database("ket_noi_chinh");

    if (!coSoDuLieu.isOpen())
    {
        return danhSach;
    }

    QSqlQuery truyVan(coSoDuLieu);

    if (!truyVan.exec(
            "SELECT "
            "id,"
            "ten_dang_nhap,"
            "ho_ten,"
            "quyen,"
            "ngay_tao "
            "FROM tai_khoan "
            "ORDER BY id ASC"
        ))
    {
        return danhSach;
    }

    while (truyVan.next())
    {
        danhSach.append(
            docThongTinTaiKhoan(truyVan)
        );
    }

    return danhSach;
}

bool CoSoDuLieu::capNhatTaiKhoan(
    int id,
    const QString &hoTen,
    const QString &quyen,
    QString &thongBaoLoi
)
{
    QSqlDatabase coSoDuLieu =
        QSqlDatabase::database("ket_noi_chinh");

    if (!coSoDuLieu.isOpen())
    {
        thongBaoLoi =
            "Co so du lieu chua duoc mo";

        return false;
    }

    if (id <= 0)
    {
        thongBaoLoi =
            "ID tai khoan khong hop le";

        return false;
    }

    if (hoTen.trimmed().isEmpty())
    {
        thongBaoLoi =
            "Ho ten khong duoc de trong";

        return false;
    }

    if (
        quyen.trimmed() != "Quan tri vien" &&
        quyen.trimmed() != "Nhan vien"
    )
    {
        thongBaoLoi =
            "Quyen tai khoan khong hop le";

        return false;
    }

    QSqlQuery truyVan(coSoDuLieu);

    truyVan.prepare(
        "UPDATE tai_khoan SET "
        "ho_ten = :ho_ten,"
        "quyen = :quyen "
        "WHERE id = :id"
    );

    truyVan.bindValue(
        ":ho_ten",
        hoTen.trimmed()
    );

    truyVan.bindValue(
        ":quyen",
        quyen.trimmed()
    );

    truyVan.bindValue(
        ":id",
        id
    );

    if (!truyVan.exec())
    {
        thongBaoLoi =
            truyVan.lastError().text();

        return false;
    }

    if (truyVan.numRowsAffected() == 0)
    {
        thongBaoLoi =
            "Khong tim thay tai khoan can cap nhat";

        return false;
    }

    return true;
}

bool CoSoDuLieu::doiMatKhauTaiKhoan(
    int id,
    const QString &matKhauMoi,
    QString &thongBaoLoi
)
{
    QSqlDatabase coSoDuLieu =
        QSqlDatabase::database("ket_noi_chinh");

    if (!coSoDuLieu.isOpen())
    {
        thongBaoLoi =
            "Co so du lieu chua duoc mo";

        return false;
    }

    if (id <= 0)
    {
        thongBaoLoi =
            "ID tai khoan khong hop le";

        return false;
    }

    if (matKhauMoi.length() < 6)
    {
        thongBaoLoi =
            "Mat khau moi phai co it nhat 6 ky tu";

        return false;
    }

    QSqlQuery truyVan(coSoDuLieu);

    truyVan.prepare(
        "UPDATE tai_khoan SET "
        "mat_khau = :mat_khau "
        "WHERE id = :id"
    );

    truyVan.bindValue(
        ":mat_khau",
        maHoaMatKhau(matKhauMoi)
    );

    truyVan.bindValue(
        ":id",
        id
    );

    if (!truyVan.exec())
    {
        thongBaoLoi =
            truyVan.lastError().text();

        return false;
    }

    if (truyVan.numRowsAffected() == 0)
    {
        thongBaoLoi =
            "Khong tim thay tai khoan can doi mat khau";

        return false;
    }

    return true;
}

bool CoSoDuLieu::xoaTaiKhoan(
    int id,
    const QString &tenDangNhapHienTai,
    QString &thongBaoLoi
)
{
    QSqlDatabase coSoDuLieu =
        QSqlDatabase::database("ket_noi_chinh");

    if (!coSoDuLieu.isOpen())
    {
        thongBaoLoi =
            "Co so du lieu chua duoc mo";

        return false;
    }

    if (id <= 0)
    {
        thongBaoLoi =
            "ID tai khoan khong hop le";

        return false;
    }

    QSqlQuery truyVanKiemTra(coSoDuLieu);

    truyVanKiemTra.prepare(
        "SELECT ten_dang_nhap, quyen "
        "FROM tai_khoan "
        "WHERE id = :id"
    );

    truyVanKiemTra.bindValue(
        ":id",
        id
    );

    if (
        !truyVanKiemTra.exec() ||
        !truyVanKiemTra.next()
    )
    {
        thongBaoLoi =
            "Khong tim thay tai khoan can xoa";

        return false;
    }

    const QString tenDangNhapCanXoa =
        truyVanKiemTra
            .value("ten_dang_nhap")
            .toString();

    const QString quyenCanXoa =
        truyVanKiemTra
            .value("quyen")
            .toString();

    if (
        tenDangNhapCanXoa ==
        tenDangNhapHienTai.trimmed()
    )
    {
        thongBaoLoi =
            "Khong the xoa tai khoan dang dang nhap";

        return false;
    }

    if (quyenCanXoa == "Quan tri vien")
    {
        QSqlQuery truyVanDem(coSoDuLieu);

        truyVanDem.prepare(
            "SELECT COUNT(*) "
            "FROM tai_khoan "
            "WHERE quyen = :quyen"
        );

        truyVanDem.bindValue(
            ":quyen",
            "Quan tri vien"
        );

        if (
            !truyVanDem.exec() ||
            !truyVanDem.next()
        )
        {
            thongBaoLoi =
                "Khong dem duoc so tai khoan quan tri";

            return false;
        }

        if (truyVanDem.value(0).toInt() <= 1)
        {
            thongBaoLoi =
                "He thong phai con it nhat mot quan tri vien";

            return false;
        }
    }

    QSqlQuery truyVanXoa(coSoDuLieu);

    truyVanXoa.prepare(
        "DELETE FROM tai_khoan "
        "WHERE id = :id"
    );

    truyVanXoa.bindValue(
        ":id",
        id
    );

    if (!truyVanXoa.exec())
    {
        thongBaoLoi =
            truyVanXoa.lastError().text();

        return false;
    }

    if (truyVanXoa.numRowsAffected() == 0)
    {
        thongBaoLoi =
            "Khong tim thay tai khoan can xoa";

        return false;
    }

    return true;
}

bool CoSoDuLieu::themThe(
    const QString &uid,
    const QString &hoTen,
    const QString &maSo,
    const QString &vaiTro,
    QString &thongBaoLoi
)
{
    QSqlDatabase coSoDuLieu =
        QSqlDatabase::database("ket_noi_chinh");

    QSqlQuery truyVan(coSoDuLieu);

    truyVan.prepare(
        "INSERT INTO the_rfid ("
        "uid,"
        "ho_ten,"
        "ma_so,"
        "vai_tro,"
        "ngay_dang_ky"
        ") VALUES ("
        ":uid,"
        ":ho_ten,"
        ":ma_so,"
        ":vai_tro,"
        ":ngay_dang_ky"
        ")"
    );

    truyVan.bindValue(
        ":uid",
        uid.trimmed().toUpper()
    );

    truyVan.bindValue(
        ":ho_ten",
        hoTen.trimmed()
    );

    truyVan.bindValue(
        ":ma_so",
        maSo.trimmed()
    );

    truyVan.bindValue(
        ":vai_tro",
        vaiTro.trimmed()
    );

    truyVan.bindValue(
        ":ngay_dang_ky",
        QDateTime::currentDateTime()
            .toString("yyyy-MM-dd HH:mm:ss")
    );

    if (!truyVan.exec())
    {
        thongBaoLoi =
            truyVan.lastError().text();

        return false;
    }

    return true;
}

bool CoSoDuLieu::capNhatThe(
    const QString &uid,
    const QString &hoTen,
    const QString &maSo,
    const QString &vaiTro,
    QString &thongBaoLoi
)
{
    QSqlDatabase coSoDuLieu =
        QSqlDatabase::database("ket_noi_chinh");

    QSqlQuery truyVan(coSoDuLieu);

    truyVan.prepare(
        "UPDATE the_rfid SET "
        "ho_ten = :ho_ten,"
        "ma_so = :ma_so,"
        "vai_tro = :vai_tro "
        "WHERE uid = :uid"
    );

    truyVan.bindValue(
        ":uid",
        uid.trimmed().toUpper()
    );

    truyVan.bindValue(
        ":ho_ten",
        hoTen.trimmed()
    );

    truyVan.bindValue(
        ":ma_so",
        maSo.trimmed()
    );

    truyVan.bindValue(
        ":vai_tro",
        vaiTro.trimmed()
    );

    if (!truyVan.exec())
    {
        thongBaoLoi =
            truyVan.lastError().text();

        return false;
    }

    if (truyVan.numRowsAffected() == 0)
    {
        thongBaoLoi =
            "Khong tim thay UID can cap nhat";

        return false;
    }

    return true;
}

bool CoSoDuLieu::xoaThe(
    const QString &uid,
    QString &thongBaoLoi
)
{
    QSqlDatabase coSoDuLieu =
        QSqlDatabase::database("ket_noi_chinh");

    QSqlQuery truyVan(coSoDuLieu);

    truyVan.prepare(
        "DELETE FROM the_rfid "
        "WHERE uid = :uid"
    );

    truyVan.bindValue(
        ":uid",
        uid.trimmed().toUpper()
    );

    if (!truyVan.exec())
    {
        thongBaoLoi =
            truyVan.lastError().text();

        return false;
    }

    if (truyVan.numRowsAffected() == 0)
    {
        thongBaoLoi =
            "Khong tim thay UID can xoa";

        return false;
    }

    return true;
}

QList<ThongTinThe> CoSoDuLieu::layDanhSachThe()
{
    QList<ThongTinThe> danhSach;

    QSqlDatabase coSoDuLieu =
        QSqlDatabase::database("ket_noi_chinh");

    QSqlQuery truyVan(coSoDuLieu);

    if (!truyVan.exec(
            "SELECT "
            "id,"
            "uid,"
            "ho_ten,"
            "ma_so,"
            "vai_tro,"
            "ngay_dang_ky "
            "FROM the_rfid "
            "ORDER BY id DESC"
        ))
    {
        return danhSach;
    }

    while (truyVan.next())
    {
        danhSach.append(
            docThongTinThe(truyVan)
        );
    }

    return danhSach;
}

QList<ThongTinThe> CoSoDuLieu::timKiemThe(
    const QString &tuKhoa
)
{
    QList<ThongTinThe> danhSach;

    QSqlDatabase coSoDuLieu =
        QSqlDatabase::database("ket_noi_chinh");

    QSqlQuery truyVan(coSoDuLieu);

    truyVan.prepare(
        "SELECT "
        "id,"
        "uid,"
        "ho_ten,"
        "ma_so,"
        "vai_tro,"
        "ngay_dang_ky "
        "FROM the_rfid "
        "WHERE uid LIKE :tu_khoa "
        "OR ho_ten LIKE :tu_khoa "
        "OR ma_so LIKE :tu_khoa "
        "ORDER BY id DESC"
    );

    truyVan.bindValue(
        ":tu_khoa",
        "%" + tuKhoa.trimmed() + "%"
    );

    if (!truyVan.exec())
    {
        return danhSach;
    }

    while (truyVan.next())
    {
        danhSach.append(
            docThongTinThe(truyVan)
        );
    }

    return danhSach;
}

bool CoSoDuLieu::timTheTheoUid(
    const QString &uid,
    ThongTinThe &thongTinThe
)
{
    QSqlDatabase coSoDuLieu =
        QSqlDatabase::database("ket_noi_chinh");

    QSqlQuery truyVan(coSoDuLieu);

    truyVan.prepare(
        "SELECT "
        "id,"
        "uid,"
        "ho_ten,"
        "ma_so,"
        "vai_tro,"
        "ngay_dang_ky "
        "FROM the_rfid "
        "WHERE uid = :uid"
    );

    truyVan.bindValue(
        ":uid",
        uid.trimmed().toUpper()
    );

    if (!truyVan.exec())
    {
        return false;
    }

    if (!truyVan.next())
    {
        return false;
    }

    thongTinThe =
        docThongTinThe(truyVan);

    return true;
}

bool CoSoDuLieu::themLichSuRaVao(
    const QString &uid,
    const QString &hoTen,
    const QString &maSo,
    const QString &ketQua,
    const QString &trangThaiCua,
    QString &thongBaoLoi
)
{
    QSqlDatabase coSoDuLieu =
        QSqlDatabase::database("ket_noi_chinh");

    if (!coSoDuLieu.isOpen())
    {
        thongBaoLoi =
            "Co so du lieu chua duoc mo";

        return false;
    }

    QSqlQuery truyVan(coSoDuLieu);

    truyVan.prepare(
        "INSERT INTO lich_su_ra_vao ("
        "thoi_gian,"
        "uid,"
        "ho_ten,"
        "ma_so,"
        "ket_qua,"
        "trang_thai_cua"
        ") VALUES ("
        ":thoi_gian,"
        ":uid,"
        ":ho_ten,"
        ":ma_so,"
        ":ket_qua,"
        ":trang_thai_cua"
        ")"
    );

    truyVan.bindValue(
        ":thoi_gian",
        QDateTime::currentDateTime()
            .toString("yyyy-MM-dd HH:mm:ss")
    );

    truyVan.bindValue(
        ":uid",
        uid.trimmed().toUpper()
    );

    truyVan.bindValue(
        ":ho_ten",
        hoTen.trimmed()
    );

    truyVan.bindValue(
        ":ma_so",
        maSo.trimmed()
    );

    truyVan.bindValue(
        ":ket_qua",
        ketQua.trimmed()
    );

    truyVan.bindValue(
        ":trang_thai_cua",
        trangThaiCua.trimmed()
    );

    if (!truyVan.exec())
    {
        thongBaoLoi =
            truyVan.lastError().text();

        return false;
    }

    return true;
}

QList<ThongTinLichSu>
CoSoDuLieu::layLichSuRaVao()
{
    QList<ThongTinLichSu> danhSach;

    QSqlDatabase coSoDuLieu =
        QSqlDatabase::database("ket_noi_chinh");

    QSqlQuery truyVan(coSoDuLieu);

    if (!truyVan.exec(
            "SELECT "
            "id,"
            "thoi_gian,"
            "uid,"
            "ho_ten,"
            "ma_so,"
            "ket_qua,"
            "trang_thai_cua "
            "FROM lich_su_ra_vao "
            "ORDER BY id DESC"
        ))
    {
        return danhSach;
    }

    while (truyVan.next())
    {
        danhSach.append(
            docThongTinLichSu(truyVan)
        );
    }

    return danhSach;
}

QList<ThongTinLichSu>
CoSoDuLieu::timKiemLichSuRaVao(
    const QString &tuKhoa,
    const QString &ketQua
)
{
    QList<ThongTinLichSu> danhSach;

    QSqlDatabase coSoDuLieu =
        QSqlDatabase::database("ket_noi_chinh");

    if (!coSoDuLieu.isOpen())
    {
        return danhSach;
    }

    const QString tuKhoaChuan =
        tuKhoa.trimmed();

    const QString ketQuaChuan =
        ketQua.trimmed();

    QString cauLenh =
        "SELECT "
        "id,"
        "thoi_gian,"
        "uid,"
        "ho_ten,"
        "ma_so,"
        "ket_qua,"
        "trang_thai_cua "
        "FROM lich_su_ra_vao "
        "WHERE ("
        "uid LIKE :tu_khoa "
        "OR ho_ten LIKE :tu_khoa "
        "OR ma_so LIKE :tu_khoa"
        ") ";

    const bool coLocKetQua =
        !ketQuaChuan.isEmpty() &&
        ketQuaChuan.compare(
            "Tat ca ket qua",
            Qt::CaseInsensitive
        ) != 0;

    if (coLocKetQua)
    {
        cauLenh +=
            "AND ket_qua = :ket_qua ";
    }

    cauLenh +=
        "ORDER BY id DESC";

    QSqlQuery truyVan(coSoDuLieu);

    truyVan.prepare(cauLenh);

    truyVan.bindValue(
        ":tu_khoa",
        "%" + tuKhoaChuan + "%"
    );

    if (coLocKetQua)
    {
        truyVan.bindValue(
            ":ket_qua",
            ketQuaChuan.toUpper()
        );
    }

    if (!truyVan.exec())
    {
        return danhSach;
    }

    while (truyVan.next())
    {
        danhSach.append(
            docThongTinLichSu(truyVan)
        );
    }

    return danhSach;
}

