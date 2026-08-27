#include "caidathethong.h"
#include "cauhinhhethong.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

CaiDatHeThong::CaiDatHeThong(
    QWidget *cha
)
    : QDialog(cha),
      oThoiGianDongCua(nullptr),
      nutLuu(nullptr),
      nutHuy(nullptr),
      nhanThongBao(nullptr)
{
    setWindowTitle(
        "Cai dat he thong"
    );

    setFixedSize(
        500,
        260
    );

    QLabel *nhanTieuDe =
        new QLabel(
            "CAI DAT HE THONG",
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

    // Tao cac o nhap cau hinh
    oThoiGianDongCua =
        new QSpinBox(this);

    oThoiGianDongCua->setRange(
        1,
        300
    );

    oThoiGianDongCua->setSuffix(
        " giay"
    );

    oThoiGianDongCua->setValue(
        15
    );

    // Tao cac nut
    nutLuu =
        new QPushButton(
            "LUU CAU HINH",
            this
        );

    nutHuy =
        new QPushButton(
            "HUY",
            this
        );

    nutLuu->setMinimumHeight(40);
    nutHuy->setMinimumHeight(40);

    // Tao nhan thong bao
    nhanThongBao =
        new QLabel(
            "Thay doi se co hieu luc sau khi khoi dong lai chuong trinh",
            this
        );

    nhanThongBao->setAlignment(
        Qt::AlignCenter
    );

    nhanThongBao->setWordWrap(true);

    // Bo cuc nhap
    QFormLayout *boCucNhap =
        new QFormLayout;

    boCucNhap->addRow(
        "Thoi gian tu dong dong cua:",
        oThoiGianDongCua
    );

    // Bo cuc nut
    QHBoxLayout *boCucNut =
        new QHBoxLayout;

    boCucNut->addWidget(
        nutLuu
    );

    boCucNut->addWidget(
        nutHuy
    );

    // Bo cuc chinh
    QVBoxLayout *boCucChinh =
        new QVBoxLayout(this);

    boCucChinh->setContentsMargins(
        30,
        25,
        30,
        25
    );

    boCucChinh->setSpacing(18);

    boCucChinh->addWidget(
        nhanTieuDe
    );

    boCucChinh->addLayout(
        boCucNhap
    );

    boCucChinh->addLayout(
        boCucNut
    );

    boCucChinh->addWidget(
        nhanThongBao
    );

    // Ket noi cac nut
    connect(
        nutLuu,
        &QPushButton::clicked,
        this,
        &CaiDatHeThong::xuLyLuuCauHinh
    );

    connect(
        nutHuy,
        &QPushButton::clicked,
        this,
        &CaiDatHeThong::xuLyHuy
    );

    // Tai cau hinh hien tai
    taiCauHinhHienTai();
}

void CaiDatHeThong::taiCauHinhHienTai()
{
    const CauHinhHeThong cauHinh =
        QuanLyCauHinh::docCauHinh();

    oThoiGianDongCua->setValue(
        cauHinh.thoiGianDongCuaGiay
    );
}

void CaiDatHeThong::xuLyLuuCauHinh()
{
    CauHinhHeThong cauHinh;

    cauHinh.thoiGianDongCuaGiay =
        oThoiGianDongCua->value();

    const bool thanhCong =
        QuanLyCauHinh::luuCauHinh(
            cauHinh
        );

    if (!thanhCong)
    {
        QMessageBox::critical(
            this,
            "Luu cau hinh",
            "Khong the luu file cau hinh."
        );

        return;
    }

    QMessageBox::information(
        this,
        "Luu cau hinh",
        "Da luu cau hinh thanh cong.\n"
        "Hay khoi dong lai chuong trinh de ap dung."
    );

    accept();
}

void CaiDatHeThong::xuLyHuy()
{
    reject();
}
