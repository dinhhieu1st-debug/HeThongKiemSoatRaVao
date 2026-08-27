#ifndef COSODULIEU_H
#define COSODULIEU_H

#include <QList>
#include <QString>

struct ThongTinTaiKhoan
{
    int id;
    QString tenDangNhap;
    QString hoTen;
    QString quyen;
    QString ngayTao;
};

struct ThongTinThe
{
    int id;
    QString uid;
    QString hoTen;
    QString maSo;
    QString vaiTro;
    QString ngayDangKy;
};

struct ThongTinLichSu
{
    int id;
    QString thoiGian;
    QString uid;
    QString hoTen;
    QString maSo;
    QString ketQua;
    QString trangThaiCua;
};

class CoSoDuLieu
{
public:
    // Khoi tao SQLite va cac bang du lieu
    static bool khoiTao();

    // Tao cac bang du lieu
    static bool taoBangTaiKhoan();
    static bool taoBangTheRfid();
    static bool taoBangLichSuRaVao();

    // Tao tai khoan admin mac dinh
    static bool taoTaiKhoanMacDinh();

    // Kiem tra tai khoan va mat khau
    static bool kiemTraDangNhap(
        const QString &tenDangNhap,
        const QString &matKhau
    );

    // Dang nhap va lay day du thong tin tai khoan
    static bool dangNhapVaLayThongTin(
        const QString &tenDangNhap,
        const QString &matKhau,
        ThongTinTaiKhoan &thongTinTaiKhoan
    );

    // Them tai khoan moi
    static bool themTaiKhoan(
        const QString &tenDangNhap,
        const QString &matKhau,
        const QString &hoTen,
        const QString &quyen,
        QString &thongBaoLoi
    );

    // Lay danh sach tai khoan
    static QList<ThongTinTaiKhoan> layDanhSachTaiKhoan();

    // Cap nhat ho ten va quyen tai khoan
    static bool capNhatTaiKhoan(
        int id,
        const QString &hoTen,
        const QString &quyen,
        QString &thongBaoLoi
    );

    // Doi mat khau tai khoan
    static bool doiMatKhauTaiKhoan(
        int id,
        const QString &matKhauMoi,
        QString &thongBaoLoi
    );

    // Xoa tai khoan
    static bool xoaTaiKhoan(
        int id,
        const QString &tenDangNhapHienTai,
        QString &thongBaoLoi
    );

    // Them the RFID
    static bool themThe(
        const QString &uid,
        const QString &hoTen,
        const QString &maSo,
        const QString &vaiTro,
        QString &thongBaoLoi
    );

    // Cap nhat thong tin the RFID
    static bool capNhatThe(
        const QString &uid,
        const QString &hoTen,
        const QString &maSo,
        const QString &vaiTro,
        QString &thongBaoLoi
    );

    // Xoa the RFID
    static bool xoaThe(
        const QString &uid,
        QString &thongBaoLoi
    );

    // Lay tat ca the RFID
    static QList<ThongTinThe> layDanhSachThe();

    // Tim kiem the RFID
    static QList<ThongTinThe> timKiemThe(
        const QString &tuKhoa
    );

    // Tim mot the theo UID
    static bool timTheTheoUid(
        const QString &uid,
        ThongTinThe &thongTinThe
    );

    // Luu mot lan quet the vao lich su
    static bool themLichSuRaVao(
        const QString &uid,
        const QString &hoTen,
        const QString &maSo,
        const QString &ketQua,
        const QString &trangThaiCua,
        QString &thongBaoLoi
    );

    // Lay tat ca lich su ra vao
    static QList<ThongTinLichSu> layLichSuRaVao();

    // Tim kiem va loc lich su ra vao
    // ketQua rong hoac "Tat ca ket qua" se khong loc
    static QList<ThongTinLichSu> timKiemLichSuRaVao(
        const QString &tuKhoa,
        const QString &ketQua
    );
};

#endif
