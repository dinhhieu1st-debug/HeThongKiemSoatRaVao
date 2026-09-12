#include "dangnhap.h"
#include "cosodulieu.h"

#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

DangNhap::DangNhap(QWidget *cha)
    : QDialog(cha),
      txtUsername(nullptr),
      txtPassword(nullptr),
      btnLogin(nullptr),
      btnCreateAccount(nullptr),
      lblNotification(nullptr)
{
    // Cau hinh cua so dang nhap
    setWindowTitle("Dang nhap he thong");
    setFixedSize(420, 250);

    // Tao tieu de
    QLabel *lblTitle =
        new QLabel(
            "DANG NHAP HE THONG",
            this
        );

    lblTitle->setAlignment(
        Qt::AlignCenter
    );

    QFont phongChuTieuDe =
        lblTitle->font();

    phongChuTieuDe.setPointSize(18);
    phongChuTieuDe.setBold(true);

    lblTitle->setFont(
        phongChuTieuDe
    );

    // Tao cac o nhap
    txtUsername =
        new QLineEdit(this);

    txtPassword =
        new QLineEdit(this);

    txtUsername->setObjectName("txtUsername");
    txtPassword->setObjectName("txtPassword");

    txtUsername->setPlaceholderText(
        "Nhap ten dang nhap"
    );

    txtPassword->setPlaceholderText(
        "Nhap mat khau"
    );

    txtPassword->setEchoMode(
        QLineEdit::Password
    );

    // Tao nut dang nhap
    btnLogin =
        new QPushButton(
            "DANG NHAP",
            this
        );

    btnLogin->setMinimumHeight(40);
    btnLogin->setObjectName("btnLogin");

    // Van khoi tao de phu hop voi khai bao trong header
    // Nhung khong hien nut tao tai khoan truoc dang nhap
    btnCreateAccount =
        new QPushButton(
            "TAO TAI KHOAN",
            this
        );

    btnCreateAccount->setVisible(false);
    btnCreateAccount->setObjectName("btnCreateAccount");

    // Tao nhan thong bao
    lblNotification =
        new QLabel(
            "Vui long nhap tai khoan va mat khau",
            this
        );

    lblNotification->setAlignment(
        Qt::AlignCenter
    );
    lblTitle->setObjectName("lblTitle");
    lblNotification->setObjectName("lblNotification");

    // Tao bo cuc nhap thong tin
    QFormLayout *boCucNhap =
        new QFormLayout;

    boCucNhap->addRow(
        "Ten dang nhap:",
        txtUsername
    );

    boCucNhap->addRow(
        "Mat khau:",
        txtPassword
    );

    // Tao bo cuc chinh
    QVBoxLayout *boCucChinh =
        new QVBoxLayout(this);

    boCucChinh->setContentsMargins(
        30,
        25,
        30,
        25
    );

    boCucChinh->setSpacing(15);

    boCucChinh->addWidget(
        lblTitle
    );

    boCucChinh->addLayout(
        boCucNhap
    );

    boCucChinh->addWidget(
        btnLogin
    );

    boCucChinh->addWidget(
        lblNotification
    );

    // Ket noi nut dang nhap
    connect(
        btnLogin,
        &QPushButton::clicked,
        this,
        &DangNhap::xuLyDangNhap
    );

    // Nhan Enter de dang nhap
    connect(
        txtPassword,
        &QLineEdit::returnPressed,
        this,
        &DangNhap::xuLyDangNhap
    );

    txtUsername->setFocus();
}

QString DangNhap::layTenDangNhap() const
{
    return tenDangNhapDaDangNhap;
}

QString DangNhap::layHoTenNguoiDung() const
{
    return hoTenNguoiDung;
}

QString DangNhap::layQuyenNguoiDung() const
{
    return quyenNguoiDung;
}

void DangNhap::xuLyDangNhap()
{
    dangNhapNguoiDung();
}

void DangNhap::dangNhapNguoiDung()
{
    const QString tenDangNhap =
        txtUsername->text().trimmed();

    const QString matKhau =
        txtPassword->text();

    if (
        tenDangNhap.isEmpty() ||
        matKhau.isEmpty()
    )
    {
        lblNotification->setText(
            "Chua nhap day du thong tin"
        );

        return;
    }

    ThongTinTaiKhoan thongTinTaiKhoan;

    const bool thanhCong =
        CoSoDuLieu::dangNhapVaLayThongTin(
            tenDangNhap,
            matKhau,
            thongTinTaiKhoan
        );

    if (thanhCong)
    {
        tenDangNhapDaDangNhap =
            thongTinTaiKhoan.tenDangNhap;

        hoTenNguoiDung =
            thongTinTaiKhoan.hoTen;

        quyenNguoiDung =
            thongTinTaiKhoan.quyen;

        lblNotification->setText(
            "Dang nhap thanh cong"
        );

        accept();
        return;
    }

    tenDangNhapDaDangNhap.clear();
    hoTenNguoiDung.clear();
    quyenNguoiDung.clear();

    lblNotification->setText(
        "Sai ten dang nhap hoac mat khau"
    );

    txtPassword->clear();
    txtPassword->setFocus();
}

void DangNhap::moTaoTaiKhoan()
{
    // Khong cho tao tai khoan truoc khi dang nhap
    lblNotification->setText(
        "Chi quan tri vien duoc quan ly tai khoan"
    );
}
