#include "caidathethong.h"
#include "cauhinhhethong.h"
#include "ketnoible.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

CaiDatHeThong::CaiDatHeThong(
    KetNoiBle *ketNoiBle,
    QWidget *cha
)
    : QDialog(cha),
      ketNoiBle(ketNoiBle),
      spnDoorCloseTime(nullptr),
      btnSave(nullptr),
      btnCancel(nullptr),
      lblNotification(nullptr)
{
    setWindowTitle(
        "Cai dat he thong"
    );

    setFixedSize(
        500,
        260
    );

    QLabel *lblTitle =
        new QLabel(
            "CAI DAT HE THONG",
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

    // Tao cac o nhap cau hinh
    spnDoorCloseTime =
        new QSpinBox(this);

    spnDoorCloseTime->setObjectName("spnDoorCloseTime");

    spnDoorCloseTime->setRange(
        1,
        300
    );

    spnDoorCloseTime->setSuffix(
        " giay"
    );

    spnDoorCloseTime->setValue(
        15
    );

    // Tao cac nut
    btnSave =
        new QPushButton(
            "LUU CAU HINH",
            this
        );

    btnCancel =
        new QPushButton(
            "HUY",
            this
        );

    btnSave->setMinimumHeight(40);
    btnCancel->setMinimumHeight(40);
    btnSave->setObjectName("btnSave");
    btnCancel->setObjectName("btnCancel");

    // Tao nhan thong bao
    lblNotification =
        new QLabel(
            "Cau hinh se duoc luu va dong bo ngay voi ESP32 neu dang ket noi BLE",
            this
        );

    lblNotification->setAlignment(
        Qt::AlignCenter
    );

    lblNotification->setWordWrap(true);
    lblTitle->setObjectName("lblTitle");
    lblNotification->setObjectName("lblNotification");

    // Bo cuc nhap
    QFormLayout *boCucNhap =
        new QFormLayout;

    boCucNhap->addRow(
        "Thoi gian tu dong dong cua:",
        spnDoorCloseTime
    );

    // Bo cuc nut
    QHBoxLayout *boCucNut =
        new QHBoxLayout;

    boCucNut->addWidget(
        btnSave
    );

    boCucNut->addWidget(
        btnCancel
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
        lblTitle
    );

    boCucChinh->addLayout(
        boCucNhap
    );

    boCucChinh->addLayout(
        boCucNut
    );

    boCucChinh->addWidget(
        lblNotification
    );

    // Ket noi cac nut
    connect(
        btnSave,
        &QPushButton::clicked,
        this,
        &CaiDatHeThong::xuLyLuuCauHinh
    );

    connect(
        btnCancel,
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

    spnDoorCloseTime->setValue(
        cauHinh.thoiGianDongCuaGiay
    );
}

void CaiDatHeThong::xuLyLuuCauHinh()
{
    luuCauHinhTuGiaoDien();
}

void CaiDatHeThong::luuCauHinhTuGiaoDien()
{
    CauHinhHeThong cauHinh;

    cauHinh.thoiGianDongCuaGiay =
        spnDoorCloseTime->value();

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

    if (ketNoiBle != nullptr && ketNoiBle->daKetNoi())
    {
        ketNoiBle->guiDuLieu(
            "access/config/door_timeout",
            QString::number(cauHinh.thoiGianDongCuaGiay)
        );

        QMessageBox::information(
            this,
            "Luu cau hinh",
            "Da luu cau hinh va dong bo ngay " +
            QString::number(cauHinh.thoiGianDongCuaGiay) +
            " giay sang ESP32 thanh cong!"
        );
    }
    else
    {
        QMessageBox::information(
            this,
            "Luu cau hinh",
            "Da luu cau hinh (" +
            QString::number(cauHinh.thoiGianDongCuaGiay) +
            " giay). Cau hinh se tu dong gui toi ESP32 ngay khi ket noi BLE."
        );
    }

    accept();
}

void CaiDatHeThong::xuLyHuy()
{
    reject();
}
