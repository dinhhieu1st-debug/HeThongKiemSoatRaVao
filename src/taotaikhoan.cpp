#include "taotaikhoan.h"

#include <QAbstractItemView>
#include <QComboBox>
#include <QFormLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

TaoTaiKhoan::TaoTaiKhoan(
    const QString &tenDangNhapDangSuDung,
    QWidget *cha
)
    : QDialog(cha),
      idTaiKhoanDangChon(-1),
      tenDangNhapHienTai(
          tenDangNhapDangSuDung
      ),
      oTenDangNhap(nullptr),
      oMatKhau(nullptr),
      oNhapLaiMatKhau(nullptr),
      oHoTen(nullptr),
      hopQuyenTaiKhoan(nullptr),
      bangDanhSachTaiKhoan(nullptr),
      nutTaoTaiKhoan(nullptr),
      nutCapNhatTaiKhoan(nullptr),
      nutDoiMatKhau(nullptr),
      nutXoaTaiKhoan(nullptr),
      nutTaiLaiDanhSach(nullptr),
      nutHuy(nullptr),
      nhanThongBao(nullptr)
{
    // Cau hinh cua so quan ly tai khoan
    setWindowTitle(
        "Quan ly tai khoan"
    );

    resize(
        900,
        650
    );

    // Tao tieu de
    QLabel *nhanTieuDe =
        new QLabel(
            "QUAN LY TAI KHOAN",
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

    oNhapLaiMatKhau =
        new QLineEdit(this);

    oHoTen =
        new QLineEdit(this);

    oTenDangNhap->setPlaceholderText(
        "Nhap ten dang nhap"
    );

    oMatKhau->setPlaceholderText(
        "Nhap mat khau moi"
    );

    oNhapLaiMatKhau->setPlaceholderText(
        "Nhap lai mat khau"
    );

    oHoTen->setPlaceholderText(
        "Nhap ho va ten"
    );

    oMatKhau->setEchoMode(
        QLineEdit::Password
    );

    oNhapLaiMatKhau->setEchoMode(
        QLineEdit::Password
    );

    // Tao hop chon quyen
    hopQuyenTaiKhoan =
        new QComboBox(this);

    hopQuyenTaiKhoan->addItem(
        "Quan tri vien"
    );

    hopQuyenTaiKhoan->addItem(
        "Nhan vien"
    );

    // Tao bang danh sach tai khoan
    bangDanhSachTaiKhoan =
        new QTableWidget(this);

    bangDanhSachTaiKhoan->setColumnCount(5);

    bangDanhSachTaiKhoan->setHorizontalHeaderLabels(
        QStringList()
        << "ID"
        << "Ten dang nhap"
        << "Ho va ten"
        << "Quyen"
        << "Ngay tao"
    );

    bangDanhSachTaiKhoan
        ->horizontalHeader()
        ->setSectionResizeMode(
            QHeaderView::Stretch
        );

    bangDanhSachTaiKhoan->setSelectionMode(
        QAbstractItemView::SingleSelection
    );

    bangDanhSachTaiKhoan->setSelectionBehavior(
        QAbstractItemView::SelectRows
    );

    bangDanhSachTaiKhoan->setEditTriggers(
        QAbstractItemView::NoEditTriggers
    );

    bangDanhSachTaiKhoan
        ->verticalHeader()
        ->setVisible(false);

    // Tao cac nut chuc nang
    nutTaoTaiKhoan =
        new QPushButton(
            "TAO TAI KHOAN",
            this
        );

    nutCapNhatTaiKhoan =
        new QPushButton(
            "CAP NHAT",
            this
        );

    nutDoiMatKhau =
        new QPushButton(
            "DOI MAT KHAU",
            this
        );

    nutXoaTaiKhoan =
        new QPushButton(
            "XOA TAI KHOAN",
            this
        );

    nutTaiLaiDanhSach =
        new QPushButton(
            "TAI LAI",
            this
        );

    nutHuy =
        new QPushButton(
            "DONG",
            this
        );

    nutTaoTaiKhoan->setMinimumHeight(38);
    nutCapNhatTaiKhoan->setMinimumHeight(38);
    nutDoiMatKhau->setMinimumHeight(38);
    nutXoaTaiKhoan->setMinimumHeight(38);
    nutTaiLaiDanhSach->setMinimumHeight(38);
    nutHuy->setMinimumHeight(38);

    // Tao nhan thong bao
    nhanThongBao =
        new QLabel(
            "Chon tai khoan trong bang hoac nhap thong tin moi",
            this
        );

    nhanThongBao->setAlignment(
        Qt::AlignCenter
    );

    // Bo cuc nhap thong tin
    QFormLayout *boCucNhap =
        new QFormLayout;

    boCucNhap->addRow(
        "Ten dang nhap:",
        oTenDangNhap
    );

    boCucNhap->addRow(
        "Ho va ten:",
        oHoTen
    );

    boCucNhap->addRow(
        "Quyen tai khoan:",
        hopQuyenTaiKhoan
    );

    boCucNhap->addRow(
        "Mat khau:",
        oMatKhau
    );

    boCucNhap->addRow(
        "Nhap lai mat khau:",
        oNhapLaiMatKhau
    );

    // Bo cuc cac nut
    QGridLayout *boCucNut =
        new QGridLayout;

    boCucNut->addWidget(
        nutTaoTaiKhoan,
        0,
        0
    );

    boCucNut->addWidget(
        nutCapNhatTaiKhoan,
        0,
        1
    );

    boCucNut->addWidget(
        nutDoiMatKhau,
        0,
        2
    );

    boCucNut->addWidget(
        nutXoaTaiKhoan,
        1,
        0
    );

    boCucNut->addWidget(
        nutTaiLaiDanhSach,
        1,
        1
    );

    boCucNut->addWidget(
        nutHuy,
        1,
        2
    );

    // Bo cuc chinh
    QVBoxLayout *boCucChinh =
        new QVBoxLayout(this);

    boCucChinh->setContentsMargins(
        25,
        20,
        25,
        20
    );

    boCucChinh->setSpacing(12);

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
        bangDanhSachTaiKhoan
    );

    boCucChinh->addWidget(
        nhanThongBao
    );

    // Ket noi cac nut
    connect(
        nutTaoTaiKhoan,
        &QPushButton::clicked,
        this,
        &TaoTaiKhoan::xuLyTaoTaiKhoan
    );

    connect(
        nutCapNhatTaiKhoan,
        &QPushButton::clicked,
        this,
        &TaoTaiKhoan::xuLyCapNhatTaiKhoan
    );

    connect(
        nutDoiMatKhau,
        &QPushButton::clicked,
        this,
        &TaoTaiKhoan::xuLyDoiMatKhau
    );

    connect(
        nutXoaTaiKhoan,
        &QPushButton::clicked,
        this,
        &TaoTaiKhoan::xuLyXoaTaiKhoan
    );

    connect(
        nutTaiLaiDanhSach,
        &QPushButton::clicked,
        this,
        &TaoTaiKhoan::xuLyTaiLaiDanhSach
    );

    connect(
        nutHuy,
        &QPushButton::clicked,
        this,
        &TaoTaiKhoan::huyTaoTaiKhoan
    );

    connect(
        bangDanhSachTaiKhoan,
        &QTableWidget::cellClicked,
        this,
        &TaoTaiKhoan::xuLyChonTaiKhoan
    );

    connect(
        oNhapLaiMatKhau,
        &QLineEdit::returnPressed,
        this,
        &TaoTaiKhoan::xuLyTaoTaiKhoan
    );

    taiDanhSachTaiKhoan();
    oTenDangNhap->setFocus();
}

void TaoTaiKhoan::xuLyTaoTaiKhoan()
{
    if (!kiemTraThongTinTaoMoi())
    {
        return;
    }

    const QString tenDangNhap =
        oTenDangNhap->text().trimmed();

    const QString matKhau =
        oMatKhau->text();

    const QString hoTen =
        oHoTen->text().trimmed();

    const QString quyen =
        hopQuyenTaiKhoan->currentText();

    QString thongBaoLoi;

    const bool thanhCong =
        CoSoDuLieu::themTaiKhoan(
            tenDangNhap,
            matKhau,
            hoTen,
            quyen,
            thongBaoLoi
        );

    if (!thanhCong)
    {
        if (
            thongBaoLoi.contains(
                "UNIQUE",
                Qt::CaseInsensitive
            )
        )
        {
            nhanThongBao->setText(
                "Ten dang nhap da ton tai"
            );
        }
        else
        {
            nhanThongBao->setText(
                "Khong tao duoc tai khoan: " +
                thongBaoLoi
            );
        }

        return;
    }

    QMessageBox::information(
        this,
        "Tao tai khoan",
        "Tao tai khoan thanh cong."
    );

    xoaNoiDungNhap();
    taiDanhSachTaiKhoan();
}

void TaoTaiKhoan::xuLyCapNhatTaiKhoan()
{
    if (!kiemTraDaChonTaiKhoan())
    {
        return;
    }

    const QString hoTen =
        oHoTen->text().trimmed();

    const QString quyen =
        hopQuyenTaiKhoan->currentText();

    if (hoTen.isEmpty())
    {
        nhanThongBao->setText(
            "Ho va ten khong duoc de trong"
        );

        oHoTen->setFocus();
        return;
    }

    const QMessageBox::StandardButton luaChon =
        QMessageBox::question(
            this,
            "Cap nhat tai khoan",
            "Ban co chac muon cap nhat tai khoan nay khong?"
        );

    if (luaChon != QMessageBox::Yes)
    {
        return;
    }

    QString thongBaoLoi;

    const bool thanhCong =
        CoSoDuLieu::capNhatTaiKhoan(
            idTaiKhoanDangChon,
            hoTen,
            quyen,
            thongBaoLoi
        );

    if (!thanhCong)
    {
        QMessageBox::warning(
            this,
            "Cap nhat tai khoan",
            thongBaoLoi
        );

        return;
    }

    QMessageBox::information(
        this,
        "Cap nhat tai khoan",
        "Cap nhat tai khoan thanh cong."
    );

    xoaNoiDungNhap();
    taiDanhSachTaiKhoan();
}

void TaoTaiKhoan::xuLyDoiMatKhau()
{
    if (!kiemTraDaChonTaiKhoan())
    {
        return;
    }

    const QString matKhauMoi =
        oMatKhau->text();

    const QString nhapLaiMatKhau =
        oNhapLaiMatKhau->text();

    if (
        matKhauMoi.isEmpty() ||
        nhapLaiMatKhau.isEmpty()
    )
    {
        nhanThongBao->setText(
            "Chua nhap mat khau moi"
        );

        return;
    }

    if (matKhauMoi.length() < 6)
    {
        nhanThongBao->setText(
            "Mat khau moi phai co it nhat 6 ky tu"
        );

        return;
    }

    if (matKhauMoi != nhapLaiMatKhau)
    {
        nhanThongBao->setText(
            "Hai mat khau khong giong nhau"
        );

        oNhapLaiMatKhau->clear();
        oNhapLaiMatKhau->setFocus();
        return;
    }

    const QMessageBox::StandardButton luaChon =
        QMessageBox::question(
            this,
            "Doi mat khau",
            "Ban co chac muon doi mat khau tai khoan nay khong?"
        );

    if (luaChon != QMessageBox::Yes)
    {
        return;
    }

    QString thongBaoLoi;

    const bool thanhCong =
        CoSoDuLieu::doiMatKhauTaiKhoan(
            idTaiKhoanDangChon,
            matKhauMoi,
            thongBaoLoi
        );

    if (!thanhCong)
    {
        QMessageBox::warning(
            this,
            "Doi mat khau",
            thongBaoLoi
        );

        return;
    }

    QMessageBox::information(
        this,
        "Doi mat khau",
        "Doi mat khau thanh cong."
    );

    oMatKhau->clear();
    oNhapLaiMatKhau->clear();

    nhanThongBao->setText(
        "Da doi mat khau tai khoan"
    );
}

void TaoTaiKhoan::xuLyXoaTaiKhoan()
{
    if (!kiemTraDaChonTaiKhoan())
    {
        return;
    }

    const QString tenDangNhapCanXoa =
        oTenDangNhap->text().trimmed();

    const QMessageBox::StandardButton luaChon =
        QMessageBox::question(
            this,
            "Xoa tai khoan",
            "Ban co chac muon xoa tai khoan " +
                tenDangNhapCanXoa +
                " khong?"
        );

    if (luaChon != QMessageBox::Yes)
    {
        return;
    }

    QString thongBaoLoi;

    const bool thanhCong =
        CoSoDuLieu::xoaTaiKhoan(
            idTaiKhoanDangChon,
            tenDangNhapHienTai,
            thongBaoLoi
        );

    if (!thanhCong)
    {
        QMessageBox::warning(
            this,
            "Xoa tai khoan",
            thongBaoLoi
        );

        return;
    }

    QMessageBox::information(
        this,
        "Xoa tai khoan",
        "Xoa tai khoan thanh cong."
    );

    xoaNoiDungNhap();
    taiDanhSachTaiKhoan();
}

void TaoTaiKhoan::xuLyTaiLaiDanhSach()
{
    xoaNoiDungNhap();
    taiDanhSachTaiKhoan();

    nhanThongBao->setText(
        "Da tai lai danh sach tai khoan"
    );
}

void TaoTaiKhoan::xuLyChonTaiKhoan(
    int dong,
    int cot
)
{
    Q_UNUSED(cot);

    if (dong < 0)
    {
        return;
    }

    QTableWidgetItem *oId =
        bangDanhSachTaiKhoan->item(
            dong,
            0
        );

    QTableWidgetItem *oTen =
        bangDanhSachTaiKhoan->item(
            dong,
            1
        );

    QTableWidgetItem *oHoTenBang =
        bangDanhSachTaiKhoan->item(
            dong,
            2
        );

    QTableWidgetItem *oQuyen =
        bangDanhSachTaiKhoan->item(
            dong,
            3
        );

    if (
        oId == nullptr ||
        oTen == nullptr ||
        oHoTenBang == nullptr ||
        oQuyen == nullptr
    )
    {
        return;
    }

    idTaiKhoanDangChon =
        oId->text().toInt();

    oTenDangNhap->setText(
        oTen->text()
    );

    oTenDangNhap->setEnabled(false);

    oHoTen->setText(
        oHoTenBang->text()
    );

    const int viTriQuyen =
        hopQuyenTaiKhoan->findText(
            oQuyen->text()
        );

    if (viTriQuyen >= 0)
    {
        hopQuyenTaiKhoan->setCurrentIndex(
            viTriQuyen
        );
    }

    oMatKhau->clear();
    oNhapLaiMatKhau->clear();

    nhanThongBao->setText(
        "Da chon tai khoan: " +
        oTen->text()
    );
}

void TaoTaiKhoan::huyTaoTaiKhoan()
{
    accept();
}

void TaoTaiKhoan::taiDanhSachTaiKhoan()
{
    const QList<ThongTinTaiKhoan> danhSach =
        CoSoDuLieu::layDanhSachTaiKhoan();

    hienThiDanhSachTaiKhoan(
        danhSach
    );
}

void TaoTaiKhoan::hienThiDanhSachTaiKhoan(
    const QList<ThongTinTaiKhoan> &danhSach
)
{
    bangDanhSachTaiKhoan->setRowCount(
        danhSach.size()
    );

    for (
        int dong = 0;
        dong < danhSach.size();
        ++dong
    )
    {
        const ThongTinTaiKhoan &taiKhoan =
            danhSach.at(dong);

        bangDanhSachTaiKhoan->setItem(
            dong,
            0,
            new QTableWidgetItem(
                QString::number(
                    taiKhoan.id
                )
            )
        );

        bangDanhSachTaiKhoan->setItem(
            dong,
            1,
            new QTableWidgetItem(
                taiKhoan.tenDangNhap
            )
        );

        bangDanhSachTaiKhoan->setItem(
            dong,
            2,
            new QTableWidgetItem(
                taiKhoan.hoTen
            )
        );

        bangDanhSachTaiKhoan->setItem(
            dong,
            3,
            new QTableWidgetItem(
                taiKhoan.quyen
            )
        );

        bangDanhSachTaiKhoan->setItem(
            dong,
            4,
            new QTableWidgetItem(
                taiKhoan.ngayTao
            )
        );
    }
}

bool TaoTaiKhoan::kiemTraThongTinTaoMoi()
{
    const QString tenDangNhap =
        oTenDangNhap->text().trimmed();

    const QString matKhau =
        oMatKhau->text();

    const QString nhapLaiMatKhau =
        oNhapLaiMatKhau->text();

    const QString hoTen =
        oHoTen->text().trimmed();

    if (idTaiKhoanDangChon > 0)
    {
        nhanThongBao->setText(
            "Nhan TAI LAI truoc khi tao tai khoan moi"
        );

        return false;
    }

    if (
        tenDangNhap.isEmpty() ||
        matKhau.isEmpty() ||
        nhapLaiMatKhau.isEmpty() ||
        hoTen.isEmpty()
    )
    {
        nhanThongBao->setText(
            "Chua nhap day du thong tin"
        );

        return false;
    }

    if (tenDangNhap.length() < 4)
    {
        nhanThongBao->setText(
            "Ten dang nhap phai co it nhat 4 ky tu"
        );

        return false;
    }

    if (matKhau.length() < 6)
    {
        nhanThongBao->setText(
            "Mat khau phai co it nhat 6 ky tu"
        );

        return false;
    }

    if (matKhau != nhapLaiMatKhau)
    {
        nhanThongBao->setText(
            "Hai mat khau khong giong nhau"
        );

        oNhapLaiMatKhau->clear();
        oNhapLaiMatKhau->setFocus();

        return false;
    }

    return true;
}

bool TaoTaiKhoan::kiemTraDaChonTaiKhoan()
{
    if (idTaiKhoanDangChon <= 0)
    {
        nhanThongBao->setText(
            "Chua chon tai khoan trong bang"
        );

        return false;
    }

    return true;
}

void TaoTaiKhoan::xoaNoiDungNhap()
{
    idTaiKhoanDangChon = -1;

    oTenDangNhap->clear();
    oTenDangNhap->setEnabled(true);

    oMatKhau->clear();
    oNhapLaiMatKhau->clear();
    oHoTen->clear();

    hopQuyenTaiKhoan->setCurrentIndex(0);

    bangDanhSachTaiKhoan
        ->clearSelection();

    nhanThongBao->setText(
        "Nhap thong tin tai khoan moi"
    );

    oTenDangNhap->setFocus();
}
