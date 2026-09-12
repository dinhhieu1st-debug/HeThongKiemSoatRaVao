#ifndef TAOTAIKHOAN_H
#define TAOTAIKHOAN_H

#include "cosodulieu.h"

#include <QDialog>
#include <QList>

class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QTableWidget;

class TaoTaiKhoan : public QDialog
{
    Q_OBJECT

public:
    explicit TaoTaiKhoan(
        const QString &tenDangNhapHienTai,
        QWidget *cha = nullptr
    );

private slots:
    // Xu ly tao tai khoan moi
    void xuLyTaoTaiKhoan();

    // Xu ly cap nhat ho ten va quyen
    void xuLyCapNhatTaiKhoan();

    // Xu ly doi mat khau
    void xuLyDoiMatKhau();

    // Xu ly xoa tai khoan
    void xuLyXoaTaiKhoan();

    // Tai lai danh sach tai khoan
    void xuLyTaiLaiDanhSach();

    // Dua thong tin dong duoc chon vao cac o nhap
    void xuLyChonTaiKhoan(
        int dong,
        int cot
    );

    // Dong cua so
    void huyTaoTaiKhoan();

private:
    void taoTaiKhoanTuGiaoDien();
    void capNhatTaiKhoanTuGiaoDien();
    void doiMatKhauTuGiaoDien();
    void xoaTaiKhoanTuGiaoDien();

    // Tai danh sach tai khoan tu SQLite
    void taiDanhSachTaiKhoan();

    // Hien thi danh sach tai khoan len bang
    void hienThiDanhSachTaiKhoan(
        const QList<ThongTinTaiKhoan> &danhSach
    );

    // Kiem tra thong tin tao tai khoan
    bool kiemTraThongTinTaoMoi();

    // Kiem tra da chon tai khoan
    bool kiemTraDaChonTaiKhoan();

    // Xoa noi dung cac o nhap
    void xoaNoiDungNhap();

    // ID tai khoan dang duoc chon
    int idTaiKhoanDangChon;

    // Tai khoan quan tri dang su dung chuong trinh
    QString tenDangNhapHienTai;

    // Cac o nhap thong tin
    QLineEdit *txtUsername;
    QLineEdit *txtPassword;
    QLineEdit *txtConfirmPassword;
    QLineEdit *txtFullName;

    // Hop chon quyen
    QComboBox *cbAccountRole;

    // Bang danh sach tai khoan
    QTableWidget *tableAccountList;

    // Cac nut chuc nang
    QPushButton *btnCreateAccount;
    QPushButton *btnUpdateAccount;
    QPushButton *btnChangePassword;
    QPushButton *btnDeleteAccount;
    QPushButton *btnReloadList;
    QPushButton *btnCancel;

    // Nhan hien thi thong bao
    QLabel *lblNotification;
};

#endif
