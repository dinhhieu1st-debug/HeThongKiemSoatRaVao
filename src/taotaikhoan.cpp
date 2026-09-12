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
      txtUsername(nullptr),
      txtPassword(nullptr),
      txtConfirmPassword(nullptr),
      txtFullName(nullptr),
      cbAccountRole(nullptr),
      tableAccountList(nullptr),
      btnCreateAccount(nullptr),
      btnUpdateAccount(nullptr),
      btnChangePassword(nullptr),
      btnDeleteAccount(nullptr),
      btnReloadList(nullptr),
      btnCancel(nullptr),
      lblNotification(nullptr)
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
    QLabel *lblTitle =
        new QLabel(
            "QUAN LY TAI KHOAN",
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

    txtConfirmPassword =
        new QLineEdit(this);

    txtFullName =
        new QLineEdit(this);

    txtUsername->setObjectName("txtUsername");
    txtPassword->setObjectName("txtPassword");
    txtConfirmPassword->setObjectName("txtConfirmPassword");
    txtFullName->setObjectName("txtFullName");

    txtUsername->setPlaceholderText(
        "Nhap ten dang nhap"
    );

    txtPassword->setPlaceholderText(
        "Nhap mat khau moi"
    );

    txtConfirmPassword->setPlaceholderText(
        "Nhap lai mat khau"
    );

    txtFullName->setPlaceholderText(
        "Nhap ho va ten"
    );

    txtPassword->setEchoMode(
        QLineEdit::Password
    );

    txtConfirmPassword->setEchoMode(
        QLineEdit::Password
    );

    // Tao hop chon quyen
    cbAccountRole =
        new QComboBox(this);

    cbAccountRole->setObjectName("cbAccountRole");

    cbAccountRole->addItem(
        "Quan tri vien"
    );

    cbAccountRole->addItem(
        "Nhan vien"
    );

    // Tao bang danh sach tai khoan
    tableAccountList =
        new QTableWidget(this);

    tableAccountList->setObjectName("tableAccountList");

    tableAccountList->setColumnCount(5);

    tableAccountList->setHorizontalHeaderLabels(
        QStringList()
        << "ID"
        << "Ten dang nhap"
        << "Ho va ten"
        << "Quyen"
        << "Ngay tao"
    );

    tableAccountList
        ->horizontalHeader()
        ->setSectionResizeMode(
            QHeaderView::Stretch
        );

    tableAccountList->setSelectionMode(
        QAbstractItemView::SingleSelection
    );

    tableAccountList->setSelectionBehavior(
        QAbstractItemView::SelectRows
    );

    tableAccountList->setEditTriggers(
        QAbstractItemView::NoEditTriggers
    );

    tableAccountList
        ->verticalHeader()
        ->setVisible(false);

    // Tao cac nut chuc nang
    btnCreateAccount =
        new QPushButton(
            "TAO TAI KHOAN",
            this
        );

    btnUpdateAccount =
        new QPushButton(
            "CAP NHAT",
            this
        );

    btnChangePassword =
        new QPushButton(
            "DOI MAT KHAU",
            this
        );

    btnDeleteAccount =
        new QPushButton(
            "XOA TAI KHOAN",
            this
        );

    btnReloadList =
        new QPushButton(
            "TAI LAI",
            this
        );

    btnCancel =
        new QPushButton(
            "DONG",
            this
        );

    btnCreateAccount->setObjectName("btnCreateAccount");
    btnUpdateAccount->setObjectName("btnUpdateAccount");
    btnChangePassword->setObjectName("btnChangePassword");
    btnDeleteAccount->setObjectName("btnDeleteAccount");
    btnReloadList->setObjectName("btnReloadList");
    btnCancel->setObjectName("btnCancel");

    btnCreateAccount->setMinimumHeight(38);
    btnUpdateAccount->setMinimumHeight(38);
    btnChangePassword->setMinimumHeight(38);
    btnDeleteAccount->setMinimumHeight(38);
    btnReloadList->setMinimumHeight(38);
    btnCancel->setMinimumHeight(38);

    // Tao nhan thong bao
    lblNotification =
        new QLabel(
            "Chon tai khoan trong bang hoac nhap thong tin moi",
            this
        );

    lblNotification->setAlignment(
        Qt::AlignCenter
    );
    lblTitle->setObjectName("lblTitle");
    lblNotification->setObjectName("lblNotification");

    // Bo cuc nhap thong tin
    QFormLayout *boCucNhap =
        new QFormLayout;

    boCucNhap->addRow(
        "Ten dang nhap:",
        txtUsername
    );

    boCucNhap->addRow(
        "Ho va ten:",
        txtFullName
    );

    boCucNhap->addRow(
        "Quyen tai khoan:",
        cbAccountRole
    );

    boCucNhap->addRow(
        "Mat khau:",
        txtPassword
    );

    boCucNhap->addRow(
        "Nhap lai mat khau:",
        txtConfirmPassword
    );

    // Bo cuc cac nut
    QGridLayout *boCucNut =
        new QGridLayout;

    boCucNut->addWidget(
        btnCreateAccount,
        0,
        0
    );

    boCucNut->addWidget(
        btnUpdateAccount,
        0,
        1
    );

    boCucNut->addWidget(
        btnChangePassword,
        0,
        2
    );

    boCucNut->addWidget(
        btnDeleteAccount,
        1,
        0
    );

    boCucNut->addWidget(
        btnReloadList,
        1,
        1
    );

    boCucNut->addWidget(
        btnCancel,
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
        lblTitle
    );

    boCucChinh->addLayout(
        boCucNhap
    );

    boCucChinh->addLayout(
        boCucNut
    );

    boCucChinh->addWidget(
        tableAccountList
    );

    boCucChinh->addWidget(
        lblNotification
    );

    // Ket noi cac nut
    connect(
        btnCreateAccount,
        &QPushButton::clicked,
        this,
        &TaoTaiKhoan::xuLyTaoTaiKhoan
    );

    connect(
        btnUpdateAccount,
        &QPushButton::clicked,
        this,
        &TaoTaiKhoan::xuLyCapNhatTaiKhoan
    );

    connect(
        btnChangePassword,
        &QPushButton::clicked,
        this,
        &TaoTaiKhoan::xuLyDoiMatKhau
    );

    connect(
        btnDeleteAccount,
        &QPushButton::clicked,
        this,
        &TaoTaiKhoan::xuLyXoaTaiKhoan
    );

    connect(
        btnReloadList,
        &QPushButton::clicked,
        this,
        &TaoTaiKhoan::xuLyTaiLaiDanhSach
    );

    connect(
        btnCancel,
        &QPushButton::clicked,
        this,
        &TaoTaiKhoan::huyTaoTaiKhoan
    );

    connect(
        tableAccountList,
        &QTableWidget::cellClicked,
        this,
        &TaoTaiKhoan::xuLyChonTaiKhoan
    );

    connect(
        txtConfirmPassword,
        &QLineEdit::returnPressed,
        this,
        &TaoTaiKhoan::xuLyTaoTaiKhoan
    );

    taiDanhSachTaiKhoan();
    txtUsername->setFocus();
}

void TaoTaiKhoan::xuLyTaoTaiKhoan()
{
    taoTaiKhoanTuGiaoDien();
}

void TaoTaiKhoan::taoTaiKhoanTuGiaoDien()
{
    if (!kiemTraThongTinTaoMoi())
    {
        return;
    }

    const QString tenDangNhap =
        txtUsername->text().trimmed();

    const QString matKhau =
        txtPassword->text();

    const QString hoTen =
        txtFullName->text().trimmed();

    const QString quyen =
        cbAccountRole->currentText();

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
            lblNotification->setText(
                "Ten dang nhap da ton tai"
            );
        }
        else
        {
            lblNotification->setText(
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
    capNhatTaiKhoanTuGiaoDien();
}

void TaoTaiKhoan::capNhatTaiKhoanTuGiaoDien()
{
    if (!kiemTraDaChonTaiKhoan())
    {
        return;
    }

    const QString hoTen =
        txtFullName->text().trimmed();

    const QString quyen =
        cbAccountRole->currentText();

    if (hoTen.isEmpty())
    {
        lblNotification->setText(
            "Ho va ten khong duoc de trong"
        );

        txtFullName->setFocus();
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
    doiMatKhauTuGiaoDien();
}

void TaoTaiKhoan::doiMatKhauTuGiaoDien()
{
    if (!kiemTraDaChonTaiKhoan())
    {
        return;
    }

    const QString matKhauMoi =
        txtPassword->text();

    const QString nhapLaiMatKhau =
        txtConfirmPassword->text();

    if (
        matKhauMoi.isEmpty() ||
        nhapLaiMatKhau.isEmpty()
    )
    {
        lblNotification->setText(
            "Chua nhap mat khau moi"
        );

        return;
    }

    if (matKhauMoi.length() < 6)
    {
        lblNotification->setText(
            "Mat khau moi phai co it nhat 6 ky tu"
        );

        return;
    }

    if (matKhauMoi != nhapLaiMatKhau)
    {
        lblNotification->setText(
            "Hai mat khau khong giong nhau"
        );

        txtConfirmPassword->clear();
        txtConfirmPassword->setFocus();
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

    txtPassword->clear();
    txtConfirmPassword->clear();

    lblNotification->setText(
        "Da doi mat khau tai khoan"
    );
}

void TaoTaiKhoan::xuLyXoaTaiKhoan()
{
    xoaTaiKhoanTuGiaoDien();
}

void TaoTaiKhoan::xoaTaiKhoanTuGiaoDien()
{
    if (!kiemTraDaChonTaiKhoan())
    {
        return;
    }

    const QString tenDangNhapCanXoa =
        txtUsername->text().trimmed();

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

    lblNotification->setText(
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
        tableAccountList->item(
            dong,
            0
        );

    QTableWidgetItem *oTen =
        tableAccountList->item(
            dong,
            1
        );

    QTableWidgetItem *oHoTenBang =
        tableAccountList->item(
            dong,
            2
        );

    QTableWidgetItem *oQuyen =
        tableAccountList->item(
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

    txtUsername->setText(
        oTen->text()
    );

    txtUsername->setEnabled(false);

    txtFullName->setText(
        oHoTenBang->text()
    );

    const int viTriQuyen =
        cbAccountRole->findText(
            oQuyen->text()
        );

    if (viTriQuyen >= 0)
    {
        cbAccountRole->setCurrentIndex(
            viTriQuyen
        );
    }

    txtPassword->clear();
    txtConfirmPassword->clear();

    lblNotification->setText(
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
    tableAccountList->setRowCount(
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

        tableAccountList->setItem(
            dong,
            0,
            new QTableWidgetItem(
                QString::number(
                    taiKhoan.id
                )
            )
        );

        tableAccountList->setItem(
            dong,
            1,
            new QTableWidgetItem(
                taiKhoan.tenDangNhap
            )
        );

        tableAccountList->setItem(
            dong,
            2,
            new QTableWidgetItem(
                taiKhoan.hoTen
            )
        );

        tableAccountList->setItem(
            dong,
            3,
            new QTableWidgetItem(
                taiKhoan.quyen
            )
        );

        tableAccountList->setItem(
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
        txtUsername->text().trimmed();

    const QString matKhau =
        txtPassword->text();

    const QString nhapLaiMatKhau =
        txtConfirmPassword->text();

    const QString hoTen =
        txtFullName->text().trimmed();

    if (idTaiKhoanDangChon > 0)
    {
        lblNotification->setText(
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
        lblNotification->setText(
            "Chua nhap day du thong tin"
        );

        return false;
    }

    if (tenDangNhap.length() < 4)
    {
        lblNotification->setText(
            "Ten dang nhap phai co it nhat 4 ky tu"
        );

        return false;
    }

    if (matKhau.length() < 6)
    {
        lblNotification->setText(
            "Mat khau phai co it nhat 6 ky tu"
        );

        return false;
    }

    if (matKhau != nhapLaiMatKhau)
    {
        lblNotification->setText(
            "Hai mat khau khong giong nhau"
        );

        txtConfirmPassword->clear();
        txtConfirmPassword->setFocus();

        return false;
    }

    return true;
}

bool TaoTaiKhoan::kiemTraDaChonTaiKhoan()
{
    if (idTaiKhoanDangChon <= 0)
    {
        lblNotification->setText(
            "Chua chon tai khoan trong bang"
        );

        return false;
    }

    return true;
}

void TaoTaiKhoan::xoaNoiDungNhap()
{
    idTaiKhoanDangChon = -1;

    txtUsername->clear();
    txtUsername->setEnabled(true);

    txtPassword->clear();
    txtConfirmPassword->clear();
    txtFullName->clear();

    cbAccountRole->setCurrentIndex(0);

    tableAccountList
        ->clearSelection();

    lblNotification->setText(
        "Nhap thong tin tai khoan moi"
    );

    txtUsername->setFocus();
}
