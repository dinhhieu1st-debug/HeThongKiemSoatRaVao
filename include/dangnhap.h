#ifndef DANGNHAP_H
#define DANGNHAP_H

#include <QDialog>
#include <QString>

class QLineEdit;
class QPushButton;
class QLabel;

class DangNhap : public QDialog
{
    Q_OBJECT

public:
    explicit DangNhap(QWidget *cha = nullptr);

    // Lay thong tin tai khoan sau khi dang nhap thanh cong
    QString layTenDangNhap() const;
    QString layHoTenNguoiDung() const;
    QString layQuyenNguoiDung() const;

private slots:
    void xuLyDangNhap();
    void moTaoTaiKhoan();

private:
    // Cac thanh phan tren giao dien dang nhap
    QLineEdit *oTenDangNhap;
    QLineEdit *oMatKhau;
    QPushButton *nutDangNhap;
    QPushButton *nutTaoTaiKhoan;
    QLabel *nhanThongBao;

    // Thong tin tai khoan dang nhap thanh cong
    QString tenDangNhapDaDangNhap;
    QString hoTenNguoiDung;
    QString quyenNguoiDung;
};

#endif
