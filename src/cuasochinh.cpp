#include "cuasochinh.h"
#include "cauhinhhethong.h"
#include "caidathethong.h"
#include "ketnoible.h"
#include "taotaikhoan.h"
#include "ui_cuasochinh.h"

#include <QAbstractItemView>
#include <QDateTime>
#include <QDialog>
#include <QFile>
#include <QFileDialog>
#include <QStringConverter>
#include <QTextStream>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

namespace
{
QString chuanHoaTrangThaiCuaHienThi(
    const QString &trangThai
)
{
    const QString trangThaiChuan =
        trangThai.trimmed().toUpper();

    if (
        trangThaiChuan == "OPEN" ||
        trangThaiChuan == "OPENED" ||
        trangThaiChuan == "DANG MO" ||
        trangThaiChuan == "DA MO"
    )
    {
        return QString::fromUtf8("ĐÃ MỞ");
    }

    if (
        trangThaiChuan == "CLOSE" ||
        trangThaiChuan == "CLOSED" ||
        trangThaiChuan == "DANG DONG" ||
        trangThaiChuan == "DA DONG"
    )
    {
        return QString::fromUtf8("ĐÃ ĐÓNG");
    }

    if (
        trangThaiChuan == "PENDING" ||
        trangThaiChuan == "CHO XAC NHAN"
    )
    {
        return QString::fromUtf8("CHỜ XÁC NHẬN");
    }

    if (trangThaiChuan == "KHONG MO DUOC")
    {
        return QString::fromUtf8("KHÔNG MỞ ĐƯỢC");
    }

    return trangThai.isEmpty()
        ? QString::fromUtf8("KHÔNG XÁC ĐỊNH")
        : trangThai;
}

QString chuanHoaKetQuaHienThi(
    const QString &ketQua
)
{
    const QString ketQuaChuan =
        ketQua.trimmed().toUpper();

    if (
        ketQuaChuan == "GRANTED" ||
        ketQuaChuan == "CHO PHEP"
    )
    {
        return QString::fromUtf8("CHO PHÉP");
    }

    if (
        ketQuaChuan == "DENIED" ||
        ketQuaChuan == "TU CHOI"
    )
    {
        return QString::fromUtf8("TỪ CHỐI");
    }

    return ketQua;
}
}

CuaSoChinh::CuaSoChinh(
    const QString &tenDangNhap,
    const QString &hoTenNguoiDung,
    const QString &quyenNguoiDung,
    QWidget *cha
)
    : QMainWindow(cha),
      ui(std::make_unique<Ui::CuaSoChinh>()),
      ketNoiBle(new KetNoiBle(this)),
      idLichSuDangChoMoCua(-1),
      tenDangNhapHienTai(tenDangNhap),
      hoTenNguoiDungHienTai(hoTenNguoiDung),
      quyenNguoiDungHienTai(quyenNguoiDung)
{
    ui->setupUi(this);

    // Tao menu he thong
    QMenu *menuHeThong =
        menuBar()->addMenu(
            "He thong"
        );

    QAction *hanhDongQuanLyTaiKhoan =
        new QAction(
            "Quan ly tai khoan",
            this
        );

    menuHeThong->addAction(
        hanhDongQuanLyTaiKhoan
    );

    QAction *hanhDongCaiDatHeThong =
        new QAction(
            "Cai dat he thong",
            this
        );

    menuHeThong->addAction(
        hanhDongCaiDatHeThong
    );

    menuHeThong->addSeparator();

    QAction *hanhDongDangXuat =
        new QAction(
            "Dang xuat",
            this
        );

    menuHeThong->addAction(
        hanhDongDangXuat
    );

    connect(
        hanhDongDangXuat,
        &QAction::triggered,
        this,
        &CuaSoChinh::xuLyDangXuat
    );

    connect(
        hanhDongCaiDatHeThong,
        &QAction::triggered,
        this,
        [this]()
        {
            const bool laQuanTriVien =
                quyenNguoiDungHienTai
                    .trimmed()
                    .compare(
                        "Quan tri vien",
                        Qt::CaseInsensitive
                    ) == 0;

            if (!laQuanTriVien)
            {
                QMessageBox::warning(
                    this,
                    "Phan quyen",
                    "Chi quan tri vien duoc thay doi cau hinh."
                );

                return;
            }

            CaiDatHeThong cuaSoCaiDat(this);
            cuaSoCaiDat.exec();

            ghiNhatKy(
                "Da dong cua so cai dat he thong."
            );
        }
    );

    connect(
        hanhDongQuanLyTaiKhoan,
        &QAction::triggered,
        this,
        &CuaSoChinh::xuLyMoQuanLyTaiKhoan
    );

    // Chi quan tri vien duoc mo quan ly tai khoan
    const bool laQuanTriVien =
        quyenNguoiDungHienTai
            .trimmed()
            .compare(
                "Quan tri vien",
                Qt::CaseInsensitive
            ) == 0;

    hanhDongQuanLyTaiKhoan->setEnabled(
        laQuanTriVien
    );

    hanhDongCaiDatHeThong->setEnabled(
        laQuanTriVien
    );

    // Cau hinh tieu de cua so
    setWindowTitle(
        "He thong kiem soat ra vao - " +
        hoTenNguoiDungHienTai +
        " - " +
        quyenNguoiDungHienTai
    );

    // Cau hinh tableBleDevices danh sach the
    ui->tableCardList
        ->horizontalHeader()
        ->setSectionResizeMode(QHeaderView::Stretch);

    ui->tableCardList
        ->setSelectionMode(
            QAbstractItemView::SingleSelection
        );

    ui->tableCardList
        ->setSelectionBehavior(
            QAbstractItemView::SelectRows
        );

    // Cau hinh tableBleDevices lich su ra vao
    ui->tableAccessHistory
        ->horizontalHeader()
        ->setSectionResizeMode(QHeaderView::Stretch);

    ui->tableAccessHistory
        ->setSelectionMode(
            QAbstractItemView::SingleSelection
        );

    ui->tableAccessHistory
        ->setSelectionBehavior(
            QAbstractItemView::SelectRows
        );

    // Doc cau hinh thoi gian dong cua tu file JSON
    const CauHinhHeThong cauHinhHeThong =
        QuanLyCauHinh::docCauHinh();

    // Ket noi cac nut quan ly the
    connect(
        ui->btnAddCard,
        &QPushButton::clicked,
        this,
        &CuaSoChinh::xuLyThemThe
    );

    connect(
        ui->btnUpdateCard,
        &QPushButton::clicked,
        this,
        &CuaSoChinh::xuLyCapNhatThe
    );

    connect(
        ui->btnDeleteCard,
        &QPushButton::clicked,
        this,
        &CuaSoChinh::xuLyXoaThe
    );

    connect(
        ui->btnSearchCard,
        &QPushButton::clicked,
        this,
        &CuaSoChinh::xuLyTimKiemThe
    );

    connect(
        ui->btnReloadList,
        &QPushButton::clicked,
        this,
        &CuaSoChinh::xuLyTaiLaiDanhSach
    );

    connect(
        ui->btnUseScannedUid,
        &QPushButton::clicked,
        this,
        &CuaSoChinh::xuLyLayUidVuaQuet
    );

    connect(
        ui->tableCardList,
        &QTableWidget::cellClicked,
        this,
        &CuaSoChinh::xuLyChonDongTrongBang
    );

    connect(
        ui->txtCardSearch,
        &QLineEdit::returnPressed,
        this,
        &CuaSoChinh::xuLyTimKiemThe
    );

    // Ket noi cac chuc nang tim kiem lich su
    connect(
        ui->btnSearchHistory,
        &QPushButton::clicked,
        this,
        &CuaSoChinh::xuLyTimKiemLichSu
    );

    connect(
        ui->btnReloadHistory,
        &QPushButton::clicked,
        this,
        &CuaSoChinh::xuLyTaiLaiLichSu
    );

    connect(
        ui->btnExportHistoryCsv,
        &QPushButton::clicked,
        this,
        &CuaSoChinh::xuLyXuatCsvLichSu
    );

    connect(
        ui->txtHistorySearch,
        &QLineEdit::returnPressed,
        this,
        &CuaSoChinh::xuLyTimKiemLichSu
    );

    connect(
        ui->btnConnectBle,
        &QPushButton::clicked,
        this,
        &CuaSoChinh::xuLyYeuCauKetNoiBle
    );

    connect(
        ui->btnOpenDoor,
        &QPushButton::clicked,
        this,
        &CuaSoChinh::xuLyMoCua
    );

    connect(
        ui->btnCloseDoor,
        &QPushButton::clicked,
        this,
        &CuaSoChinh::xuLyDongCua
    );

    connect(
        ketNoiBle,
        &KetNoiBle::nhanDuLieu,
        this,
        &CuaSoChinh::xuLyDuLieuTuThietBi
    );

    connect(
        ketNoiBle,
        &KetNoiBle::daKetNoiBle,
        this,
        &CuaSoChinh::xuLyBleDaKetNoi
    );

    connect(
        ketNoiBle,
        &KetNoiBle::daNgatKetNoiBle,
        this,
        &CuaSoChinh::xuLyBleDaNgatKetNoi
    );

    connect(
        ketNoiBle,
        &KetNoiBle::coLoi,
        this,
        &CuaSoChinh::xuLyLoiBle
    );

    // Trang thai ban dau
    uidVuaQuet.clear();

    ui->lblSensorStatus->setText(
        "Khong co nguoi"
    );

    ui->lblDoorStatus->setText(
        "Dang dong"
    );

    ui->btnConnectBle->setText(
        "KET NOI BLE"
    );

    // Ap dung quyen cua tai khoan
    apDungPhanQuyen();

    // Tai du lieu tu SQLite
    taiDanhSachThe();
    taiLichSuRaVao();

    ghiNhatKy(
        "Nguoi dung: " +
        hoTenNguoiDungHienTai
    );

    ghiNhatKy(
        "Ten dang nhap: " +
        tenDangNhapHienTai
    );

    ghiNhatKy(
        "Quyen: " +
        quyenNguoiDungHienTai
    );

    ghiNhatKy(
        "Da tai danh sach the RFID."
    );

    ghiNhatKy(
        "Da tai lich su ra vao."
    );

    ghiNhatKy(
        "He thong BLE san sang. Hay chon ket noi BLE."
    );

    capNhatTrangThaiKetNoi();
}

CuaSoChinh::~CuaSoChinh()
{
    // Ngat signal truoc khi btnCancel UI de callback bat dong bo khong truy cap UI cu.
    QObject::disconnect(
        ketNoiBle,
        nullptr,
        this,
        nullptr
    );

    ketNoiBle->dung();

}

void CuaSoChinh::xuLyDangXuat()
{
    const QMessageBox::StandardButton luaChon =
        QMessageBox::question(
            this,
            "Dang xuat",
            "Ban co chac muon dang xuat tai khoan hien tai khong?",
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No
        );

    if (luaChon != QMessageBox::Yes)
    {
        return;
    }

    ghiNhatKy(
        "Nguoi dung da dang xuat."
    );

    ketNoiBle->dung();

    emit yeuCauDangXuat();

    close();
}

void CuaSoChinh::apDungPhanQuyen()
{
    const QString quyenChuan =
        quyenNguoiDungHienTai
            .trimmed()
            .toLower();

    const bool laQuanTriVien =
        quyenChuan == "quan tri vien";

    if (laQuanTriVien)
    {
        ui->txtUid->setEnabled(true);
        ui->txtFullName->setEnabled(true);
        ui->txtId->setEnabled(true);
        ui->cbRole->setEnabled(true);

        ui->btnAddCard->setEnabled(true);
        ui->btnUpdateCard->setEnabled(true);
        ui->btnDeleteCard->setEnabled(true);
        ui->btnUseScannedUid->setEnabled(true);

        ui->btnOpenDoor->setEnabled(true);
        ui->btnCloseDoor->setEnabled(true);

        return;
    }

    // Nhan vien chi duoc giam sat va xem du lieu
    ui->txtUid->setEnabled(false);
    ui->txtFullName->setEnabled(false);
    ui->txtId->setEnabled(false);
    ui->cbRole->setEnabled(false);

    ui->btnAddCard->setEnabled(false);
    ui->btnUpdateCard->setEnabled(false);
    ui->btnDeleteCard->setEnabled(false);
    ui->btnUseScannedUid->setEnabled(false);

    ui->btnOpenDoor->setEnabled(false);
    ui->btnCloseDoor->setEnabled(false);
}

void CuaSoChinh::xuLyThemThe()
{
    themTheTuGiaoDien();
}

void CuaSoChinh::themTheTuGiaoDien()
{
    if (!kiemTraThongTinNhap())
    {
        return;
    }

    const QString uid =
        ui->txtUid->text()
            .trimmed()
            .toUpper();

    const QString hoTen =
        ui->txtFullName->text().trimmed();

    const QString maSo =
        ui->txtId->text().trimmed();

    const QString vaiTro =
        ui->cbRole->currentText();

    QString thongBaoLoi;

    const bool thanhCong =
        CoSoDuLieu::themThe(
            uid,
            hoTen,
            maSo,
            vaiTro,
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
            QMessageBox::warning(
                this,
                "Them the",
                "UID nay da duoc dang ky."
            );
        }
        else
        {
            QMessageBox::critical(
                this,
                "Them the",
                "Khong the them the.\n" +
                    thongBaoLoi
            );
        }

        return;
    }

    QMessageBox::information(
        this,
        "Them the",
        "Them the RFID thanh cong."
    );

    ghiNhatKy(
        "Da them the RFID: " + uid
    );

    xoaNoiDungNhap();
    taiDanhSachThe();
}

void CuaSoChinh::xuLyCapNhatThe()
{
    capNhatTheTuGiaoDien();
}

void CuaSoChinh::capNhatTheTuGiaoDien()
{
    if (!kiemTraThongTinNhap())
    {
        return;
    }

    const QString uid =
        ui->txtUid->text()
            .trimmed()
            .toUpper();

    const QString hoTen =
        ui->txtFullName->text().trimmed();

    const QString maSo =
        ui->txtId->text().trimmed();

    const QString vaiTro =
        ui->cbRole->currentText();

    const QMessageBox::StandardButton luaChon =
        QMessageBox::question(
            this,
            "Cap nhat the",
            "Ban co chac muon cap nhat UID " +
                uid +
                " khong?"
        );

    if (luaChon != QMessageBox::Yes)
    {
        return;
    }

    QString thongBaoLoi;

    const bool thanhCong =
        CoSoDuLieu::capNhatThe(
            uid,
            hoTen,
            maSo,
            vaiTro,
            thongBaoLoi
        );

    if (!thanhCong)
    {
        QMessageBox::warning(
            this,
            "Cap nhat the",
            thongBaoLoi
        );

        return;
    }

    QMessageBox::information(
        this,
        "Cap nhat the",
        "Cap nhat thong tin the thanh cong."
    );

    ghiNhatKy(
        "Da cap nhat the RFID: " + uid
    );

    xoaNoiDungNhap();
    taiDanhSachThe();
}

void CuaSoChinh::xuLyXoaThe()
{
    xoaTheTuGiaoDien();
}

void CuaSoChinh::xoaTheTuGiaoDien()
{
    const QString uid =
        ui->txtUid->text()
            .trimmed()
            .toUpper();

    if (uid.isEmpty())
    {
        QMessageBox::warning(
            this,
            "Xoa the",
            "Hay chon mot dong trong tableBleDevices "
            "hoac nhap UID can xoa."
        );

        return;
    }

    const QMessageBox::StandardButton luaChon =
        QMessageBox::question(
            this,
            "Xoa the",
            "Ban co chac muon xoa UID " +
                uid +
                " khong?"
        );

    if (luaChon != QMessageBox::Yes)
    {
        return;
    }

    QString thongBaoLoi;

    const bool thanhCong =
        CoSoDuLieu::xoaThe(
            uid,
            thongBaoLoi
        );

    if (!thanhCong)
    {
        QMessageBox::warning(
            this,
            "Xoa the",
            thongBaoLoi
        );

        return;
    }

    QMessageBox::information(
        this,
        "Xoa the",
        "Xoa the RFID thanh cong."
    );

    ghiNhatKy(
        "Da xoa the RFID: " + uid
    );

    xoaNoiDungNhap();
    taiDanhSachThe();
}

void CuaSoChinh::xuLyTimKiemThe()
{
    const QString tuKhoa =
        ui->txtCardSearch->text().trimmed();

    if (tuKhoa.isEmpty())
    {
        taiDanhSachThe();
        return;
    }

    const QList<ThongTinThe> danhSach =
        CoSoDuLieu::timKiemThe(tuKhoa);

    hienThiDanhSachThe(danhSach);

    ghiNhatKy(
        "Da tim kiem the voi tu khoa: " +
        tuKhoa
    );
}

void CuaSoChinh::xuLyTaiLaiDanhSach()
{
    ui->txtCardSearch->clear();

    taiDanhSachThe();

    ghiNhatKy(
        "Da tai lai danh sach the RFID."
    );
}

void CuaSoChinh::xuLyLayUidVuaQuet()
{
    if (uidVuaQuet.isEmpty())
    {
        QMessageBox::information(
            this,
            "Lay UID",
            "Chua co UID nao duoc nhan tu ESP32."
        );

        return;
    }

    ui->txtUid->setText(uidVuaQuet);

    ghiNhatKy(
        "Da dua UID vua quet vao o nhap: " +
        uidVuaQuet
    );
}

void CuaSoChinh::xuLyChonDongTrongBang(
    int dong,
    int cot
)
{
    Q_UNUSED(cot);

    const QString quyenChuan =
        quyenNguoiDungHienTai
            .trimmed()
            .toLower();

    if (quyenChuan != "quan tri vien")
    {
        return;
    }

    if (dong < 0)
    {
        return;
    }

    QTableWidgetItem *oUid =
        ui->tableCardList->item(dong, 0);

    QTableWidgetItem *oHoTen =
        ui->tableCardList->item(dong, 1);

    QTableWidgetItem *oMaSo =
        ui->tableCardList->item(dong, 2);

    QTableWidgetItem *oVaiTro =
        ui->tableCardList->item(dong, 3);

    if (
        oUid == nullptr ||
        oHoTen == nullptr ||
        oMaSo == nullptr ||
        oVaiTro == nullptr
    )
    {
        return;
    }

    ui->txtUid->setText(
        oUid->text()
    );

    ui->txtFullName->setText(
        oHoTen->text()
    );

    ui->txtId->setText(
        oMaSo->text()
    );

    const int viTriVaiTro =
        ui->cbRole->findText(
            oVaiTro->text()
        );

    if (viTriVaiTro >= 0)
    {
        ui->cbRole->setCurrentIndex(
            viTriVaiTro
        );
    }
}

void CuaSoChinh::xuLyTimKiemLichSu()
{
    const QString tuKhoa =
        ui->txtHistorySearch
            ->text()
            .trimmed();

    const QString ketQua =
        ui->cbHistoryResult
            ->currentText();

    const QList<ThongTinLichSu> danhSach =
        CoSoDuLieu::timKiemLichSuRaVao(
            tuKhoa,
            ketQua
        );

    hienThiLichSuRaVao(
        danhSach
    );

    ghiNhatKy(
        "Da tim kiem lich su - Tu khoa: " +
        tuKhoa +
        " - Bo loc: " +
        ketQua
    );
}

void CuaSoChinh::xuLyTaiLaiLichSu()
{
    ui->txtHistorySearch->clear();

    ui->cbHistoryResult
        ->setCurrentIndex(0);

    taiLichSuRaVao();

    ghiNhatKy(
        "Da tai lai toan bo lich su ra vao."
    );
}

void CuaSoChinh::xuLyXuatCsvLichSu()
{
    xuatCsvLichSu();
}

void CuaSoChinh::xuatCsvLichSu()
{
    if (ui->tableAccessHistory->rowCount() <= 0)
    {
        QMessageBox::information(
            this,
            "Xuat CSV",
            "Bang lich su khong co du lieu de xuat."
        );

        return;
    }

    const QString tenFileMacDinh =
        "lich_su_ra_vao_" +
        QDateTime::currentDateTime()
            .toString("yyyyMMdd_HHmmss") +
        ".csv";

    const QString duongDanFile =
        QFileDialog::getSaveFileName(
            this,
            "Chon noi luu lich su",
            tenFileMacDinh,
            "Tep CSV (*.csv)",
            nullptr,
            QFileDialog::DontUseNativeDialog
        );

    if (duongDanFile.isEmpty())
    {
        return;
    }

    QString duongDanChuan =
        duongDanFile;

    if (
        !duongDanChuan.endsWith(
            ".csv",
            Qt::CaseInsensitive
        )
    )
    {
        duongDanChuan += ".csv";
    }

    QFile fileCsv(duongDanChuan);

    if (
        !fileCsv.open(
            QIODevice::WriteOnly |
            QIODevice::Text
        )
    )
    {
        QMessageBox::critical(
            this,
            "Xuat CSV",
            "Khong the tao file CSV."
        );

        return;
    }

    QTextStream luongGhi(&fileCsv);
    luongGhi.setEncoding(
        QStringConverter::Utf8
    );

    // Them BOM de Excel doc dung UTF-8
    luongGhi << QChar(0xFEFF);

    auto chuanHoaCsv =
        [](QString giaTri) -> QString
    {
        giaTri.replace(
            "\"",
            "\"\""
        );

        return "\"" + giaTri + "\"";
    };

    // Ghi ten cac cot
    for (
        int cot = 0;
        cot < ui->tableAccessHistory->columnCount();
        ++cot
    )
    {
        QTableWidgetItem *tieuDe =
            ui->tableAccessHistory
                ->horizontalHeaderItem(cot);

        const QString noiDungTieuDe =
            tieuDe != nullptr
                ? tieuDe->text()
                : QString();

        luongGhi << chuanHoaCsv(
            noiDungTieuDe
        );

        if (
            cot <
            ui->tableAccessHistory->columnCount() - 1
        )
        {
            luongGhi << ",";
        }
    }

    luongGhi << "\n";

    // Chi xuat cac dong dang hien thi sau khi tim kiem hoac loc
    for (
        int dong = 0;
        dong < ui->tableAccessHistory->rowCount();
        ++dong
    )
    {
        for (
            int cot = 0;
            cot < ui->tableAccessHistory->columnCount();
            ++cot
        )
        {
            QTableWidgetItem *oDuLieu =
                ui->tableAccessHistory->item(
                    dong,
                    cot
                );

            const QString noiDungO =
                oDuLieu != nullptr
                    ? oDuLieu->text()
                    : QString();

            luongGhi << chuanHoaCsv(
                noiDungO
            );

            if (
                cot <
                ui->tableAccessHistory->columnCount() - 1
            )
            {
                luongGhi << ",";
            }
        }

        luongGhi << "\n";
    }

    fileCsv.close();

    ghiNhatKy(
        "Da xuat lich su CSV: " +
        duongDanChuan
    );

    QMessageBox::information(
        this,
        "Xuat CSV",
        "Da xuat lich su thanh cong.\n" +
        duongDanChuan
    );
}

void CuaSoChinh::xuLyMoQuanLyTaiKhoan()
{
    const bool laQuanTriVien =
        quyenNguoiDungHienTai
            .trimmed()
            .compare(
                "Quan tri vien",
                Qt::CaseInsensitive
            ) == 0;

    if (!laQuanTriVien)
    {
        QMessageBox::warning(
            this,
            "Phan quyen",
            "Chi quan tri vien duoc quan ly tai khoan."
        );

        return;
    }

    TaoTaiKhoan cuaSoQuanLyTaiKhoan(
        tenDangNhapHienTai,
        this
    );

    cuaSoQuanLyTaiKhoan.exec();

    ghiNhatKy(
        "Da dong cua so quan ly tai khoan."
    );
}

void CuaSoChinh::xuLyYeuCauKetNoiBle()
{
    if (ketNoiBle->dangXuLy())
    {
        ghiNhatKy("Nguoi dung dung ket noi BLE.");
        ketNoiBle->dung();
        capNhatTrangThaiKetNoi();
        return;
    }

    moHopThoaiChonBle();
}

void CuaSoChinh::moHopThoaiChonBle()
{
    QDialog dlgBleDeviceSelection(this);
    dlgBleDeviceSelection.setWindowTitle("Chon thiet bi BLE");
    dlgBleDeviceSelection.resize(720, 430);

    QLabel *lblBleSelectionInstructions = new QLabel(
        "Dang quet cac thiet bi BLE xung quanh...",
        &dlgBleDeviceSelection
    );
    lblBleSelectionInstructions->setObjectName(
        "lblBleSelectionInstructions"
    );

    QTableWidget *tableBleDevices = new QTableWidget(&dlgBleDeviceSelection);
    tableBleDevices->setObjectName("tableBleDevices");
    tableBleDevices->setColumnCount(4);
    tableBleDevices->setHorizontalHeaderLabels(
        {"Ten thiet bi", "Dia chi", "RSSI", "Phu hop he thong"}
    );
    tableBleDevices->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableBleDevices->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableBleDevices->setSelectionMode(QAbstractItemView::SingleSelection);
    tableBleDevices->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QPushButton *btnRescan = new QPushButton("QUET LAI", &dlgBleDeviceSelection);
    QPushButton *btnConnectSelected = new QPushButton("KET NOI BLE DA CHON", &dlgBleDeviceSelection);
    QPushButton *btnCancel = new QPushButton("HUY", &dlgBleDeviceSelection);
    btnRescan->setObjectName("btnRescan");
    btnConnectSelected->setObjectName("btnConnectSelected");
    btnCancel->setObjectName("btnCancel");
    btnConnectSelected->setEnabled(false);

    QHBoxLayout *boCucNut = new QHBoxLayout;
    boCucNut->addWidget(btnRescan);
    boCucNut->addWidget(btnConnectSelected);
    boCucNut->addWidget(btnCancel);

    QVBoxLayout *boCuc = new QVBoxLayout(&dlgBleDeviceSelection);
    boCuc->addWidget(lblBleSelectionInstructions);
    boCuc->addWidget(tableBleDevices);
    boCuc->addLayout(boCucNut);

    connect(
        ketNoiBle,
        &KetNoiBle::timThayThietBi,
        &dlgBleDeviceSelection,
        [tableBleDevices](
            const QString &ten,
            const QString &diaChi,
            int rssi,
            bool dungHeThong
        )
        {
            int dong = -1;

            for (int i = 0; i < tableBleDevices->rowCount(); ++i)
            {
                if (
                    tableBleDevices->item(i, 1) != nullptr &&
                    tableBleDevices->item(i, 1)->text() == diaChi
                )
                {
                    dong = i;
                    break;
                }
            }

            if (dong < 0)
            {
                dong = tableBleDevices->rowCount();
                tableBleDevices->insertRow(dong);
            }

            tableBleDevices->setItem(dong, 0, new QTableWidgetItem(ten));
            tableBleDevices->setItem(dong, 1, new QTableWidgetItem(diaChi));
            tableBleDevices->setItem(dong, 2, new QTableWidgetItem(QString::number(rssi) + " dBm"));
            tableBleDevices->setItem(
                dong,
                3,
                new QTableWidgetItem(
                    dungHeThong ? "Dung he thong" : "Khong xac dinh"
                )
            );
        }
    );

    connect(
        ketNoiBle,
        &KetNoiBle::ketThucQuet,
        &dlgBleDeviceSelection,
        [tableBleDevices, lblBleSelectionInstructions]()
        {
            lblBleSelectionInstructions->setText(
                tableBleDevices->rowCount() > 0
                    ? "Da quet xong. Hay chon HE THONG RA VAO HIEU."
                    : "Khong tim thay thiet bi BLE."
            );
        }
    );

    connect(
        tableBleDevices,
        &QTableWidget::itemSelectionChanged,
        &dlgBleDeviceSelection,
        [tableBleDevices, btnConnectSelected]()
        {
            btnConnectSelected->setEnabled(tableBleDevices->currentRow() >= 0);
        }
    );

    connect(
        btnRescan,
        &QPushButton::clicked,
        &dlgBleDeviceSelection,
        [this, tableBleDevices, lblBleSelectionInstructions]()
        {
            ketNoiBle->dung();
            tableBleDevices->setRowCount(0);
            lblBleSelectionInstructions->setText("Dang quet cac thiet bi BLE xung quanh...");
            ketNoiBle->quetDeLuaChon();
        }
    );

    bool daBatDauKetNoi = false;

    connect(
        btnConnectSelected,
        &QPushButton::clicked,
        &dlgBleDeviceSelection,
        [this, &dlgBleDeviceSelection, tableBleDevices, &daBatDauKetNoi]()
        {
            const int dong = tableBleDevices->currentRow();

            if (
                dong < 0 ||
                tableBleDevices->item(dong, 1) == nullptr ||
                !ketNoiBle->ketNoiTheoDiaChi(
                    tableBleDevices->item(dong, 1)->text()
                )
            )
            {
                QMessageBox::warning(
                    &dlgBleDeviceSelection,
                    "Ket noi BLE",
                    "Khong ket noi duoc thiet bi da chon."
                );
                return;
            }

            daBatDauKetNoi = true;
            ghiNhatKy(
                "Dang ket noi BLE toi: " +
                tableBleDevices->item(dong, 0)->text()
            );
            dlgBleDeviceSelection.accept();
        }
    );

    connect(btnCancel, &QPushButton::clicked, &dlgBleDeviceSelection, &QDialog::reject);

    if (!ketNoiBle->quetDeLuaChon())
    {
        QMessageBox::warning(
            this,
            "Ket noi BLE",
            "Khong the bat dau quet BLE."
        );
        return;
    }

    dlgBleDeviceSelection.exec();

    if (!daBatDauKetNoi)
    {
        ketNoiBle->dung();
    }

    capNhatTrangThaiKetNoi();
}

void CuaSoChinh::xuLyBleDaKetNoi()
{
    ghiNhatKy(
        "BLE da ket noi - uu tien BLE lam kenh chinh."
    );

    const CauHinhHeThong cauHinh =
        QuanLyCauHinh::docCauHinh();

    guiThoiGianDongCua(cauHinh.thoiGianDongCuaGiay);
    capNhatTrangThaiKetNoi();
}

void CuaSoChinh::xuLyBleDaNgatKetNoi()
{
    if (ketNoiBle->dangXuLy())
    {
        ghiNhatKy(
            "BLE bi mat ket noi - he thong se tu dong thu ket noi lai."
        );
    }
    else
    {
        ghiNhatKy(
            "BLE da ngat theo yeu cau nguoi dung."
        );
    }

    capNhatTrangThaiKetNoi();
}

void CuaSoChinh::xuLyLoiBle(
    const QString &noiDungLoi
)
{
    if (
        noiDungLoi.startsWith("Bat tay BLE HCI") ||
        noiDungLoi.startsWith("Da co 3 lan bat tay HCI") ||
        noiDungLoi.startsWith("Adapter Bluetooth Pi da khoi phuc")
    )
    {
        ghiNhatKy(noiDungLoi);
    }
    else
    {
        ghiNhatKy("Loi BLE: " + noiDungLoi);
    }

    capNhatTrangThaiKetNoi();
}

void CuaSoChinh::xuLyDuLieuTuThietBi(
    const QString &topic,
    const QString &payload
)
{
    const QString topicChuan =
        topic.trimmed();

    const QString duLieu =
        payload.trimmed().toUpper();

    if (topicChuan == "access/rfid")
    {
        xuLyUidRfid(duLieu);
    }
    else if (topicChuan == "access/sensor")
    {
        xuLyTrangThaiCamBien(duLieu);
    }
    else if (topicChuan == "access/status")
    {
        xuLyTrangThaiHeThong(duLieu);
    }
    else if (topicChuan == "access/door/status")
    {
        xuLyTrangThaiCua(duLieu);
    }
    else
    {
        ghiNhatKy(
            "Bo qua topic khong ho tro: " +
            topicChuan
        );
    }
}

void CuaSoChinh::xuLyUidRfid(
    const QString &uid
)
{
    idLichSuDangChoMoCua = -1;

    uidVuaQuet =
        uid.trimmed().toUpper();

    if (uidVuaQuet.isEmpty())
    {
        return;
    }

    const QString thoiGian =
        QDateTime::currentDateTime()
            .toString("yyyy-MM-dd HH:mm:ss");

    ui->lblUidValue->setText(
        uidVuaQuet
    );

    ui->lblTimeValue->setText(
        thoiGian
    );

    ghiNhatKy(
        "Nhan UID RFID: " + uidVuaQuet
    );

    ThongTinThe thongTinThe;

    const bool theHopLe =
        CoSoDuLieu::timTheTheoUid(
            uidVuaQuet,
            thongTinThe
        );

    if (theHopLe)
    {
        ui->lblCardHolderValue->setText(
            thongTinThe.hoTen
        );

        ui->lblResultValue->setText(
            "The hop le"
        );

        ghiNhatKy(
            "The hop le - Chu the: " +
            thongTinThe.hoTen
        );

        if (guiKetQuaThe("GRANTED"))
        {
            ghiNhatKy(
                "Da gui ket qua GRANTED toi ESP32."
            );
        }
        else
        {
            ghiNhatKy(
                "Khong gui duoc ket qua GRANTED."
            );
        }

        QString trangThaiCua =
            "Khong mo duoc";

        if (guiLenhCua("OPEN"))
        {
            ui->lblDoorStatus->setText(
                "Dang cho xac nhan mo"
            );

            trangThaiCua = "CHO XAC NHAN";

            ghiNhatKy(
                "Da gui lenh OPEN do the hop le."
            );
        }
        else
        {
            ghiNhatKy(
                "Khong gui duoc lenh OPEN."
            );
        }

        const int idLichSuMoi =
            luuLichSuRaVao(
                uidVuaQuet,
                thongTinThe.hoTen,
                thongTinThe.maSo,
                "CHO PHEP",
                trangThaiCua
            );

        if (trangThaiCua == "CHO XAC NHAN")
        {
            idLichSuDangChoMoCua =
                idLichSuMoi;
        }

        return;
    }

    ui->lblCardHolderValue->setText(
        "Khong xac dinh"
    );

    ui->lblResultValue->setText(
        "The khong hop le"
    );

    ghiNhatKy(
        "Tu choi truy cap: UID chua dang ky."
    );

    if (guiKetQuaThe("DENIED"))
    {
        ghiNhatKy(
            "Da gui ket qua DENIED toi ESP32."
        );
    }
    else
    {
        ghiNhatKy(
            "Khong gui duoc ket qua DENIED."
        );
    }

    luuLichSuRaVao(
        uidVuaQuet,
        "Khong xac dinh",
        "",
        "TU CHOI",
        "CLOSED"
    );
}

void CuaSoChinh::xuLyTrangThaiCamBien(
    const QString &trangThai
)
{
    const QString duLieu =
        trangThai.trimmed().toUpper();

    if (
        duLieu == "OBSTACLE" ||
        duLieu == "DETECTED" ||
        duLieu == "PERSON"
    )
    {
        ui->lblSensorStatus->setText(
            "Co nguoi"
        );

        ghiNhatKy(
            "Cam bien phat hien co nguoi."
        );

        return;
    }

    if (
        duLieu == "CLEAR" ||
        duLieu == "NO_OBSTACLE" ||
        duLieu == "NO_PERSON"
    )
    {
        ui->lblSensorStatus->setText(
            "Khong co nguoi"
        );

        ghiNhatKy(
            "Cam bien xac nhan khong co nguoi."
        );

        return;
    }

    ui->lblSensorStatus->setText(
        duLieu
    );

    ghiNhatKy(
        "Du lieu cam bien: " + duLieu
    );
}

void CuaSoChinh::xuLyTrangThaiHeThong(
    const QString &trangThai
)
{
    ghiNhatKy(
        "Trang thai ESP32: " +
        trangThai
    );
}

void CuaSoChinh::xuLyTrangThaiCua(
    const QString &trangThai
)
{
    const QString duLieu =
        trangThai.trimmed().toUpper();

    ui->lblDoorStatus->setText(
        chuanHoaTrangThaiCuaHienThi(duLieu)
    );

    if (
        duLieu == "OPENED" &&
        idLichSuDangChoMoCua >= 0
    )
    {
        QString thongBaoLoi;

        if (
            CoSoDuLieu::capNhatTrangThaiCuaLichSu(
                idLichSuDangChoMoCua,
                duLieu,
                thongBaoLoi
            )
        )
        {
            idLichSuDangChoMoCua = -1;
            taiLichSuRaVao();
        }
        else
        {
            ghiNhatKy(
                "Khong cap nhat duoc trang thai cua trong lich su: " +
                thongBaoLoi
            );
        }
    }

    ghiNhatKy(
        "Trang thai cua: " +
        chuanHoaTrangThaiCuaHienThi(duLieu)
    );
}

void CuaSoChinh::xuLyMoCua()
{
    const QString quyenChuan =
        quyenNguoiDungHienTai
            .trimmed()
            .toLower();

    if (quyenChuan != "quan tri vien")
    {
        QMessageBox::warning(
            this,
            "Phan quyen",
            "Tai khoan khong co quyen mo cua thu cong."
        );

        return;
    }

    if (
        !ketNoiBle->daKetNoi()
    )
    {
        QMessageBox::warning(
            this,
            "Mo cua",
            "BLE chua ket noi."
        );

        return;
    }

    if (!guiLenhCua("OPEN"))
    {
        QMessageBox::warning(
            this,
            "Mo cua",
            "Khong gui duoc lenh OPEN."
        );

        return;
    }

    ui->lblDoorStatus->setText(
        "Dang cho xac nhan mo"
    );

    ghiNhatKy(
        "Da gui lenh mo cua thu cong."
    );
}

void CuaSoChinh::xuLyDongCua()
{
    const QString quyenChuan =
        quyenNguoiDungHienTai
            .trimmed()
            .toLower();

    if (quyenChuan != "quan tri vien")
    {
        QMessageBox::warning(
            this,
            "Phan quyen",
            "Tai khoan khong co quyen dong cua thu cong."
        );

        return;
    }

    if (
        !ketNoiBle->daKetNoi()
    )
    {
        QMessageBox::warning(
            this,
            "Dong cua",
            "BLE chua ket noi."
        );

        return;
    }

    if (!guiLenhCua("CLOSE"))
    {
        QMessageBox::warning(
            this,
            "Dong cua",
            "Khong gui duoc lenh CLOSE."
        );

        return;
    }

    ui->lblDoorStatus->setText(
        "Dang cho xac nhan dong"
    );

    ghiNhatKy(
        "Da gui lenh dong cua thu cong."
    );
}

bool CuaSoChinh::guiDuLieu(
    const QString &topic,
    const QString &payload
)
{
    if (ketNoiBle->daKetNoi())
    {
        return ketNoiBle->guiDuLieu(
            topic,
            payload
        );
    }

    return false;
}

void CuaSoChinh::capNhatTrangThaiKetNoi()
{
    QString trangThaiBle;

    if (ketNoiBle->daKetNoi())
    {
        trangThaiBle = "BLE: Da ket noi - Kenh chinh";
    }
    else if (ketNoiBle->dangXuLy())
    {
        trangThaiBle = "BLE: Dang tim/ket noi";
    }
    else
    {
        trangThaiBle = "BLE: Chua ket noi";
    }

    QString kenh = "Cho nguoi dung chon";

    if (ketNoiBle->daKetNoi())
    {
        kenh = "BLE";
    }
    else if (ketNoiBle->dangXuLy())
    {
        kenh = "Dang ket noi BLE";
    }

    ui->lblConnectionStatus->setText(
        trangThaiBle +
        "\nKenh hien tai: " +
        kenh
    );

    ui->btnConnectBle->setText(
        ketNoiBle->daKetNoi()
            ? "NGAT KET NOI BLE"
            : (
                ketNoiBle->dangXuLy()
                    ? "DUNG TIM BLE"
                    : "KET NOI BLE"
            )
    );

}

bool CuaSoChinh::guiLenhCua(
    const QString &lenh
)
{
    const QString lenhChuan =
        lenh.trimmed().toUpper();

    if (
        lenhChuan != "OPEN" &&
        lenhChuan != "CLOSE"
    )
    {
        return false;
    }

    return guiDuLieu(
        "access/door/command",
        lenhChuan
    );
}

bool CuaSoChinh::guiKetQuaThe(
    const QString &ketQua
)
{
    const QString ketQuaChuan =
        ketQua.trimmed().toUpper();

    if (
        ketQuaChuan != "GRANTED" &&
        ketQuaChuan != "DENIED"
    )
    {
        return false;
    }

    return guiDuLieu(
        "access/result",
        ketQuaChuan
    );
}

bool CuaSoChinh::guiThoiGianDongCua(
    int soGiay
)
{
    if (
        soGiay < 1 ||
        soGiay > 300
    )
    {
        return false;
    }

    return guiDuLieu(
        "access/config/door_timeout",
        QString::number(soGiay)
    );
}

void CuaSoChinh::taiDanhSachThe()
{
    const QList<ThongTinThe> danhSach =
        CoSoDuLieu::layDanhSachThe();

    hienThiDanhSachThe(danhSach);
}

void CuaSoChinh::hienThiDanhSachThe(
    const QList<ThongTinThe> &danhSach
)
{
    ui->tableCardList->setRowCount(
        danhSach.size()
    );

    for (
        int dong = 0;
        dong < danhSach.size();
        ++dong
    )
    {
        const ThongTinThe &the =
            danhSach.at(dong);

        ui->tableCardList->setItem(
            dong,
            0,
            new QTableWidgetItem(the.uid)
        );

        ui->tableCardList->setItem(
            dong,
            1,
            new QTableWidgetItem(the.hoTen)
        );

        ui->tableCardList->setItem(
            dong,
            2,
            new QTableWidgetItem(the.maSo)
        );

        ui->tableCardList->setItem(
            dong,
            3,
            new QTableWidgetItem(the.vaiTro)
        );

        ui->tableCardList->setItem(
            dong,
            4,
            new QTableWidgetItem(
                the.ngayDangKy
            )
        );
    }
}

void CuaSoChinh::taiLichSuRaVao()
{
    const QList<ThongTinLichSu> danhSach =
        CoSoDuLieu::layLichSuRaVao();

    hienThiLichSuRaVao(danhSach);
}

void CuaSoChinh::hienThiLichSuRaVao(
    const QList<ThongTinLichSu> &danhSach
)
{
    ui->tableAccessHistory->setRowCount(
        danhSach.size()
    );

    for (
        int dong = 0;
        dong < danhSach.size();
        ++dong
    )
    {
        const ThongTinLichSu &lichSu =
            danhSach.at(dong);

        ui->tableAccessHistory->setItem(
            dong,
            0,
            new QTableWidgetItem(
                lichSu.thoiGian
            )
        );

        ui->tableAccessHistory->setItem(
            dong,
            1,
            new QTableWidgetItem(
                lichSu.uid
            )
        );

        ui->tableAccessHistory->setItem(
            dong,
            2,
            new QTableWidgetItem(
                lichSu.hoTen
            )
        );

        ui->tableAccessHistory->setItem(
            dong,
            3,
            new QTableWidgetItem(
                lichSu.maSo
            )
        );

        ui->tableAccessHistory->setItem(
            dong,
            4,
            new QTableWidgetItem(
                chuanHoaKetQuaHienThi(
                    lichSu.ketQua
                )
            )
        );

        ui->tableAccessHistory->setItem(
            dong,
            5,
            new QTableWidgetItem(
                chuanHoaTrangThaiCuaHienThi(
                    lichSu.trangThaiCua
                )
            )
        );
    }
}

int CuaSoChinh::luuLichSuRaVao(
    const QString &uid,
    const QString &hoTen,
    const QString &maSo,
    const QString &ketQua,
    const QString &trangThaiCua
)
{
    QString thongBaoLoi;
    int idLichSuMoi = -1;

    const bool thanhCong =
        CoSoDuLieu::themLichSuRaVao(
            uid,
            hoTen,
            maSo,
            ketQua,
            trangThaiCua,
            thongBaoLoi,
            &idLichSuMoi
        );

    if (!thanhCong)
    {
        ghiNhatKy(
            "Khong luu duoc lich su: " +
            thongBaoLoi
        );

        return -1;
    }

    ghiNhatKy(
        "Da luu lan quet vao lich su."
    );

    taiLichSuRaVao();

    return idLichSuMoi;
}

bool CuaSoChinh::kiemTraThongTinNhap()
{
    const QString quyenChuan =
        quyenNguoiDungHienTai
            .trimmed()
            .toLower();

    if (quyenChuan != "quan tri vien")
    {
        QMessageBox::warning(
            this,
            "Phan quyen",
            "Tai khoan khong co quyen thay doi the RFID."
        );

        return false;
    }

    const QString uid =
        ui->txtUid->text().trimmed();

    const QString hoTen =
        ui->txtFullName->text().trimmed();

    const QString maSo =
        ui->txtId->text().trimmed();

    if (uid.isEmpty())
    {
        QMessageBox::warning(
            this,
            "Thong tin the",
            "Chua nhap UID the."
        );

        ui->txtUid->setFocus();
        return false;
    }

    if (hoTen.isEmpty())
    {
        QMessageBox::warning(
            this,
            "Thong tin the",
            "Chua nhap ho va ten."
        );

        ui->txtFullName->setFocus();
        return false;
    }

    if (maSo.isEmpty())
    {
        QMessageBox::warning(
            this,
            "Thong tin the",
            "Chua nhap ma so."
        );

        ui->txtId->setFocus();
        return false;
    }

    return true;
}

void CuaSoChinh::xoaNoiDungNhap()
{
    ui->txtUid->clear();
    ui->txtFullName->clear();
    ui->txtId->clear();
    ui->cbRole->setCurrentIndex(0);

    ui->tableCardList
        ->clearSelection();

    ui->txtUid->setFocus();
}

void CuaSoChinh::ghiNhatKy(
    const QString &noiDung
)
{
    const QString thoiGian =
        QDateTime::currentDateTime()
            .toString("HH:mm:ss");

    ui->txtLog->appendPlainText(
        "[" +
        thoiGian +
        "] " +
        noiDung
    );
}
