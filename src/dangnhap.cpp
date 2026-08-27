#include "dangnhap.h"
#include "cosodulieu.h"

#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

DangNhap::DangNhap(QWidget *cha)
    : QDialog(cha),
      oTenDangNhap(nullptr),
      oMatKhau(nullptr),
      nutDangNhap(nullptr),
      nutTaoTaiKhoan(nullptr),
      nhanThongBao(nullptr)
{
    // Cau hinh cua so dang nhap
    setWindowTitle("Dang nhap he thong");
    setFixedSize(420, 250);

    // Tao tieu de
    QLabel *nhanTieuDe =
        new QLabel(
            "DANG NHAP HE THONG",
            this
        );

    nhanTieuDe->setAlignment(
        Qt::AlignCenter
    );

    QFont phongChuTieuDe =
        nhanTieuDe->font();

    phongChuTieuDe.setPointSize(18);
    phongChuTieuDe.setBold(true);

    nhanTieuDe->setFont(
        phongChuTieuDe
    );

    // Tao cac o nhap
    oTenDangNhap =
        new QLineEdit(this);

    oMatKhau =
        new QLineEdit(this);

    oTenDangNhap->setPlaceholderText(
        "Nhap ten dang nhap"
    );

    oMatKhau->setPlaceholderText(
        "Nhap mat khau"
    );

    oMatKhau->setEchoMode(
        QLineEdit::Password
    );

    // Tao nut dang nhap
    nutDangNhap =
        new QPushButton(
            "DANG NHAP",
            this
        );

    nutDangNhap->setMinimumHeight(40);

    // Van khoi tao de phu hop voi khai bao trong header
    // Nhung khong hien nut tao tai khoan truoc dang nhap
    nutTaoTaiKhoan =
        new QPushButton(
            "TAO TAI KHOAN",
            this
        );

    nutTaoTaiKhoan->setVisible(false);

    // Tao nhan thong bao
    nhanThongBao =
        new QLabel(
            "Vui long nhap tai khoan va mat khau",
            this
        );

    nhanThongBao->setAlignment(
        Qt::AlignCenter
    );

    // Tao bo cuc nhap thong tin
    QFormLayout *boCucNhap =
        new QFormLayout;

    boCucNhap->addRow(
        "Ten dang nhap:",
        oTenDangNhap
    );

    boCucNhap->addRow(
        "Mat khau:",
        oMatKhau
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
        nhanTieuDe
    );

    boCucChinh->addLayout(
        boCucNhap
    );

    boCucChinh->addWidget(
        nutDangNhap
    );

    boCucChinh->addWidget(
        nhanThongBao
    );

    // Ket noi nut dang nhap
    connect(
        nutDangNhap,
        &QPushButton::clicked,
        this,
        &DangNhap::xuLyDangNhap
    );

    // Nhan Enter de dang nhap
    connect(
        oMatKhau,
        &QLineEdit::returnPressed,
        this,
        &DangNhap::xuLyDangNhap
    );

    oTenDangNhap->setFocus();
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
    const QString tenDangNhap =
        oTenDangNhap->text().trimmed();

    const QString matKhau =
        oMatKhau->text();

    if (
        tenDangNhap.isEmpty() ||
        matKhau.isEmpty()
    )
    {
        nhanThongBao->setText(
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

        nhanThongBao->setText(
            "Dang nhap thanh cong"
        );

        accept();
        return;
    }

    tenDangNhapDaDangNhap.clear();
    hoTenNguoiDung.clear();
    quyenNguoiDung.clear();

    nhanThongBao->setText(
        "Sai ten dang nhap hoac mat khau"
    );

    oMatKhau->clear();
    oMatKhau->setFocus();
}

void DangNhap::moTaoTaiKhoan()
{
    // Khong cho tao tai khoan truoc khi dang nhap
    nhanThongBao->setText(
        "Chi quan tri vien duoc quan ly tai khoan"
    );
}
