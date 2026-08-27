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

Cuasochinh::Cuasochinh(
    const QString &tenDangNhap,
    const QString &hoTenNguoiDung,
    const QString &quyenNguoiDung,
    QWidget *cha
)
    : QMainWindow(cha),
      ui(new Ui::Cuasochinh),
      ketNoiBle(new KetNoiBle(this)),
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
        &Cuasochinh::xuLyDangXuat
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
        &Cuasochinh::xuLyMoQuanLyTaiKhoan
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

    // Cau hinh bang danh sach the
    ui->bangDanhSachThe
        ->horizontalHeader()
        ->setSectionResizeMode(QHeaderView::Stretch);

    ui->bangDanhSachThe
        ->setSelectionMode(
            QAbstractItemView::SingleSelection
        );

    ui->bangDanhSachThe
        ->setSelectionBehavior(
            QAbstractItemView::SelectRows
        );

    // Cau hinh bang lich su ra vao
    ui->bangLichSuRaVao
        ->horizontalHeader()
        ->setSectionResizeMode(QHeaderView::Stretch);

    ui->bangLichSuRaVao
        ->setSelectionMode(
            QAbstractItemView::SingleSelection
        );

    ui->bangLichSuRaVao
        ->setSelectionBehavior(
            QAbstractItemView::SelectRows
        );

    // Doc cau hinh thoi gian dong cua tu file JSON
    const CauHinhHeThong cauHinhHeThong =
        QuanLyCauHinh::docCauHinh();

    // Ket noi cac nut quan ly the
    connect(
        ui->nutThemThe,
        &QPushButton::clicked,
        this,
        &Cuasochinh::xuLyThemThe
    );

    connect(
        ui->nutCapNhatThe,
        &QPushButton::clicked,
        this,
        &Cuasochinh::xuLyCapNhatThe
    );

    connect(
        ui->nutXoaThe,
        &QPushButton::clicked,
        this,
        &Cuasochinh::xuLyXoaThe
    );

    connect(
        ui->nutTimKiemThe,
        &QPushButton::clicked,
        this,
        &Cuasochinh::xuLyTimKiemThe
    );

    connect(
        ui->nutTaiLaiDanhSach,
        &QPushButton::clicked,
        this,
        &Cuasochinh::xuLyTaiLaiDanhSach
    );

    connect(
        ui->nutLayUidVuaQuet,
        &QPushButton::clicked,
        this,
        &Cuasochinh::xuLyLayUidVuaQuet
    );

    connect(
        ui->bangDanhSachThe,
        &QTableWidget::cellClicked,
        this,
        &Cuasochinh::xuLyChonDongTrongBang
    );

    connect(
        ui->oTimKiemThe,
        &QLineEdit::returnPressed,
        this,
        &Cuasochinh::xuLyTimKiemThe
    );

    // Ket noi cac chuc nang tim kiem lich su
    connect(
        ui->nutTimKiemLichSu,
        &QPushButton::clicked,
        this,
        &Cuasochinh::xuLyTimKiemLichSu
    );

    connect(
        ui->nutTaiLaiLichSu,
        &QPushButton::clicked,
        this,
        &Cuasochinh::xuLyTaiLaiLichSu
    );

    connect(
        ui->nutXuatCsvLichSu,
        &QPushButton::clicked,
        this,
        &Cuasochinh::xuLyXuatCsvLichSu
    );

    connect(
        ui->oTimKiemLichSu,
        &QLineEdit::returnPressed,
        this,
        &Cuasochinh::xuLyTimKiemLichSu
    );

    connect(
        ui->nutKetNoiBle,
        &QPushButton::clicked,
        this,
        &Cuasochinh::xuLyNutKetNoiBle
    );

    connect(
        ui->nutMoCua,
        &QPushButton::clicked,
        this,
        &Cuasochinh::xuLyMoCua
    );

    connect(
        ui->nutDongCua,
        &QPushButton::clicked,
        this,
        &Cuasochinh::xuLyDongCua
    );

    connect(
        ketNoiBle,
        &KetNoiBle::nhanDuLieu,
        this,
        &Cuasochinh::xuLyDuLieuTuThietBi
    );

    connect(
        ketNoiBle,
        &KetNoiBle::daKetNoiBle,
        this,
        &Cuasochinh::xuLyBleDaKetNoi
    );

    connect(
        ketNoiBle,
        &KetNoiBle::daNgatKetNoiBle,
        this,
        &Cuasochinh::xuLyBleDaNgatKetNoi
    );

    connect(
        ketNoiBle,
        &KetNoiBle::coLoi,
        this,
        &Cuasochinh::xuLyLoiBle
    );

    // Trang thai ban dau
    uidVuaQuet.clear();

    ui->nhanTrangThaiCamBien->setText(
        "Khong co nguoi"
    );

    ui->nhanTrangThaiCua->setText(
        "Dang dong"
    );

    ui->nutKetNoiBle->setText(
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

Cuasochinh::~Cuasochinh()
{
    // Ngat signal truoc khi huy UI de callback bat dong bo khong truy cap UI cu.
    QObject::disconnect(
        ketNoiBle,
        nullptr,
        this,
        nullptr
    );

    ketNoiBle->dung();

    delete ui;
}

void Cuasochinh::xuLyDangXuat()
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

void Cuasochinh::apDungPhanQuyen()
{
    const QString quyenChuan =
        quyenNguoiDungHienTai
            .trimmed()
            .toLower();

    const bool laQuanTriVien =
        quyenChuan == "quan tri vien";

    if (laQuanTriVien)
    {
        ui->oNhapUid->setEnabled(true);
        ui->oNhapHoTen->setEnabled(true);
        ui->oNhapMaSo->setEnabled(true);
        ui->hopVaiTro->setEnabled(true);

        ui->nutThemThe->setEnabled(true);
        ui->nutCapNhatThe->setEnabled(true);
        ui->nutXoaThe->setEnabled(true);
        ui->nutLayUidVuaQuet->setEnabled(true);

        ui->nutMoCua->setEnabled(true);
        ui->nutDongCua->setEnabled(true);

        return;
    }

    // Nhan vien chi duoc giam sat va xem du lieu
    ui->oNhapUid->setEnabled(false);
    ui->oNhapHoTen->setEnabled(false);
    ui->oNhapMaSo->setEnabled(false);
    ui->hopVaiTro->setEnabled(false);

    ui->nutThemThe->setEnabled(false);
    ui->nutCapNhatThe->setEnabled(false);
    ui->nutXoaThe->setEnabled(false);
    ui->nutLayUidVuaQuet->setEnabled(false);

    ui->nutMoCua->setEnabled(false);
    ui->nutDongCua->setEnabled(false);
}

void Cuasochinh::xuLyThemThe()
{
    if (!kiemTraThongTinNhap())
    {
        return;
    }

    const QString uid =
        ui->oNhapUid->text()
            .trimmed()
            .toUpper();

    const QString hoTen =
        ui->oNhapHoTen->text().trimmed();

    const QString maSo =
        ui->oNhapMaSo->text().trimmed();

    const QString vaiTro =
        ui->hopVaiTro->currentText();

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

void Cuasochinh::xuLyCapNhatThe()
{
    if (!kiemTraThongTinNhap())
    {
        return;
    }

    const QString uid =
        ui->oNhapUid->text()
            .trimmed()
            .toUpper();

    const QString hoTen =
        ui->oNhapHoTen->text().trimmed();

    const QString maSo =
        ui->oNhapMaSo->text().trimmed();

    const QString vaiTro =
        ui->hopVaiTro->currentText();

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

void Cuasochinh::xuLyXoaThe()
{
    const QString uid =
        ui->oNhapUid->text()
            .trimmed()
            .toUpper();

    if (uid.isEmpty())
    {
        QMessageBox::warning(
            this,
            "Xoa the",
            "Hay chon mot dong trong bang "
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

void Cuasochinh::xuLyTimKiemThe()
{
    const QString tuKhoa =
        ui->oTimKiemThe->text().trimmed();

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

void Cuasochinh::xuLyTaiLaiDanhSach()
{
    ui->oTimKiemThe->clear();

    taiDanhSachThe();

    ghiNhatKy(
        "Da tai lai danh sach the RFID."
    );
}

void Cuasochinh::xuLyLayUidVuaQuet()
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

    ui->oNhapUid->setText(uidVuaQuet);

    ghiNhatKy(
        "Da dua UID vua quet vao o nhap: " +
        uidVuaQuet
    );
}

void Cuasochinh::xuLyChonDongTrongBang(
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
        ui->bangDanhSachThe->item(dong, 0);

    QTableWidgetItem *oHoTen =
        ui->bangDanhSachThe->item(dong, 1);

    QTableWidgetItem *oMaSo =
        ui->bangDanhSachThe->item(dong, 2);

    QTableWidgetItem *oVaiTro =
        ui->bangDanhSachThe->item(dong, 3);

    if (
        oUid == nullptr ||
        oHoTen == nullptr ||
        oMaSo == nullptr ||
        oVaiTro == nullptr
    )
    {
        return;
    }

    ui->oNhapUid->setText(
        oUid->text()
    );

    ui->oNhapHoTen->setText(
        oHoTen->text()
    );

    ui->oNhapMaSo->setText(
        oMaSo->text()
    );

    const int viTriVaiTro =
        ui->hopVaiTro->findText(
            oVaiTro->text()
        );

    if (viTriVaiTro >= 0)
    {
        ui->hopVaiTro->setCurrentIndex(
            viTriVaiTro
        );
    }
}

void Cuasochinh::xuLyTimKiemLichSu()
{
    const QString tuKhoa =
        ui->oTimKiemLichSu
            ->text()
            .trimmed();

    const QString ketQua =
        ui->hopLocKetQuaLichSu
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

void Cuasochinh::xuLyTaiLaiLichSu()
{
    ui->oTimKiemLichSu->clear();

    ui->hopLocKetQuaLichSu
        ->setCurrentIndex(0);

    taiLichSuRaVao();

    ghiNhatKy(
        "Da tai lai toan bo lich su ra vao."
    );
}

void Cuasochinh::xuLyXuatCsvLichSu()
{
    if (ui->bangLichSuRaVao->rowCount() <= 0)
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
            "Tep CSV (*.csv)"
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
        cot < ui->bangLichSuRaVao->columnCount();
        ++cot
    )
    {
        QTableWidgetItem *tieuDe =
            ui->bangLichSuRaVao
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
            ui->bangLichSuRaVao->columnCount() - 1
        )
        {
            luongGhi << ",";
        }
    }

    luongGhi << "\n";

    // Chi xuat cac dong dang hien thi sau khi tim kiem hoac loc
    for (
        int dong = 0;
        dong < ui->bangLichSuRaVao->rowCount();
        ++dong
    )
    {
        for (
            int cot = 0;
            cot < ui->bangLichSuRaVao->columnCount();
            ++cot
        )
        {
            QTableWidgetItem *oDuLieu =
                ui->bangLichSuRaVao->item(
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
                ui->bangLichSuRaVao->columnCount() - 1
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

void Cuasochinh::xuLyMoQuanLyTaiKhoan()
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

void Cuasochinh::xuLyNutKetNoiBle()
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

void Cuasochinh::moHopThoaiChonBle()
{
    QDialog hopThoai(this);
    hopThoai.setWindowTitle("Chon thiet bi BLE");
    hopThoai.resize(720, 430);

    QLabel *huongDan = new QLabel(
        "Dang quet cac thiet bi BLE xung quanh...",
        &hopThoai
    );

    QTableWidget *bang = new QTableWidget(&hopThoai);
    bang->setColumnCount(4);
    bang->setHorizontalHeaderLabels(
        {"Ten thiet bi", "Dia chi", "RSSI", "Phu hop he thong"}
    );
    bang->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    bang->setSelectionBehavior(QAbstractItemView::SelectRows);
    bang->setSelectionMode(QAbstractItemView::SingleSelection);
    bang->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QPushButton *quetLai = new QPushButton("QUET LAI", &hopThoai);
    QPushButton *ketNoi = new QPushButton("KET NOI BLE DA CHON", &hopThoai);
    QPushButton *huy = new QPushButton("HUY", &hopThoai);
    ketNoi->setEnabled(false);

    QHBoxLayout *boCucNut = new QHBoxLayout;
    boCucNut->addWidget(quetLai);
    boCucNut->addWidget(ketNoi);
    boCucNut->addWidget(huy);

    QVBoxLayout *boCuc = new QVBoxLayout(&hopThoai);
    boCuc->addWidget(huongDan);
    boCuc->addWidget(bang);
    boCuc->addLayout(boCucNut);

    connect(
        ketNoiBle,
        &KetNoiBle::timThayThietBi,
        &hopThoai,
        [bang](
            const QString &ten,
            const QString &diaChi,
            int rssi,
            bool dungHeThong
        )
        {
            int dong = -1;

            for (int i = 0; i < bang->rowCount(); ++i)
            {
                if (
                    bang->item(i, 1) != nullptr &&
                    bang->item(i, 1)->text() == diaChi
                )
                {
                    dong = i;
                    break;
                }
            }

            if (dong < 0)
            {
                dong = bang->rowCount();
                bang->insertRow(dong);
            }

            bang->setItem(dong, 0, new QTableWidgetItem(ten));
            bang->setItem(dong, 1, new QTableWidgetItem(diaChi));
            bang->setItem(dong, 2, new QTableWidgetItem(QString::number(rssi) + " dBm"));
            bang->setItem(
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
        &hopThoai,
        [bang, huongDan]()
        {
            huongDan->setText(
                bang->rowCount() > 0
                    ? "Da quet xong. Hay chon HE THONG RA VAO HIEU."
                    : "Khong tim thay thiet bi BLE."
            );
        }
    );

    connect(
        bang,
        &QTableWidget::itemSelectionChanged,
        &hopThoai,
        [bang, ketNoi]()
        {
            ketNoi->setEnabled(bang->currentRow() >= 0);
        }
    );

    connect(
        quetLai,
        &QPushButton::clicked,
        &hopThoai,
        [this, bang, huongDan]()
        {
            ketNoiBle->dung();
            bang->setRowCount(0);
            huongDan->setText("Dang quet cac thiet bi BLE xung quanh...");
            ketNoiBle->quetDeLuaChon();
        }
    );

    bool daBatDauKetNoi = false;

    connect(
        ketNoi,
        &QPushButton::clicked,
        &hopThoai,
        [this, &hopThoai, bang, &daBatDauKetNoi]()
        {
            const int dong = bang->currentRow();

            if (
                dong < 0 ||
                bang->item(dong, 1) == nullptr ||
                !ketNoiBle->ketNoiTheoDiaChi(
                    bang->item(dong, 1)->text()
                )
            )
            {
                QMessageBox::warning(
                    &hopThoai,
                    "Ket noi BLE",
                    "Khong ket noi duoc thiet bi da chon."
                );
                return;
            }

            daBatDauKetNoi = true;
            ghiNhatKy(
                "Dang ket noi BLE toi: " +
                bang->item(dong, 0)->text()
            );
            hopThoai.accept();
        }
    );

    connect(huy, &QPushButton::clicked, &hopThoai, &QDialog::reject);

    if (!ketNoiBle->quetDeLuaChon())
    {
        QMessageBox::warning(
            this,
            "Ket noi BLE",
            "Khong the bat dau quet BLE."
        );
        return;
    }

    hopThoai.exec();

    if (!daBatDauKetNoi)
    {
        ketNoiBle->dung();
    }

    capNhatTrangThaiKetNoi();
}

void Cuasochinh::xuLyBleDaKetNoi()
{
    ghiNhatKy(
        "BLE da ket noi - uu tien BLE lam kenh chinh."
    );

    const CauHinhHeThong cauHinh =
        QuanLyCauHinh::docCauHinh();

    guiThoiGianDongCua(cauHinh.thoiGianDongCuaGiay);
    capNhatTrangThaiKetNoi();
}

void Cuasochinh::xuLyBleDaNgatKetNoi()
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

void Cuasochinh::xuLyLoiBle(
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

void Cuasochinh::xuLyDuLieuTuThietBi(
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

void Cuasochinh::xuLyUidRfid(
    const QString &uid
)
{
    uidVuaQuet =
        uid.trimmed().toUpper();

    if (uidVuaQuet.isEmpty())
    {
        return;
    }

    const QString thoiGian =
        QDateTime::currentDateTime()
            .toString("yyyy-MM-dd HH:mm:ss");

    ui->giaTriUid->setText(
        uidVuaQuet
    );

    ui->giaTriThoiGian->setText(
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
        ui->giaTriChuThe->setText(
            thongTinThe.hoTen
        );

        ui->giaTriKetQua->setText(
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
            ui->nhanTrangThaiCua->setText(
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

        luuLichSuRaVao(
            uidVuaQuet,
            thongTinThe.hoTen,
            thongTinThe.maSo,
            "CHO PHEP",
            trangThaiCua
        );

        return;
    }

    ui->giaTriChuThe->setText(
        "Khong xac dinh"
    );

    ui->giaTriKetQua->setText(
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

void Cuasochinh::xuLyTrangThaiCamBien(
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
        ui->nhanTrangThaiCamBien->setText(
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
        ui->nhanTrangThaiCamBien->setText(
            "Khong co nguoi"
        );

        ghiNhatKy(
            "Cam bien xac nhan khong co nguoi."
        );

        return;
    }

    ui->nhanTrangThaiCamBien->setText(
        duLieu
    );

    ghiNhatKy(
        "Du lieu cam bien: " + duLieu
    );
}

void Cuasochinh::xuLyTrangThaiHeThong(
    const QString &trangThai
)
{
    ghiNhatKy(
        "Trang thai ESP32: " +
        trangThai
    );
}

void Cuasochinh::xuLyTrangThaiCua(
    const QString &trangThai
)
{
    const QString duLieu =
        trangThai.trimmed().toUpper();

    if (duLieu == "OPENED")
    {
        ui->nhanTrangThaiCua->setText(
            "Dang mo"
        );
    }
    else if (duLieu == "CLOSED")
    {
        ui->nhanTrangThaiCua->setText(
            "Dang dong"
        );
    }
    else
    {
        ui->nhanTrangThaiCua->setText(
            duLieu
        );
    }

    ghiNhatKy(
        "Trang thai cua: " + duLieu
    );
}

void Cuasochinh::xuLyMoCua()
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

    ui->nhanTrangThaiCua->setText(
        "Dang cho xac nhan mo"
    );

    ghiNhatKy(
        "Da gui lenh mo cua thu cong."
    );
}

void Cuasochinh::xuLyDongCua()
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

    ui->nhanTrangThaiCua->setText(
        "Dang cho xac nhan dong"
    );

    ghiNhatKy(
        "Da gui lenh dong cua thu cong."
    );
}

bool Cuasochinh::guiDuLieu(
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

void Cuasochinh::capNhatTrangThaiKetNoi()
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

    ui->nhanTrangThaiKetNoi->setText(
        trangThaiBle +
        "\nKenh hien tai: " +
        kenh
    );

    ui->nutKetNoiBle->setText(
        ketNoiBle->daKetNoi()
            ? "NGAT KET NOI BLE"
            : (
                ketNoiBle->dangXuLy()
                    ? "DUNG TIM BLE"
                    : "KET NOI BLE"
            )
    );

}

bool Cuasochinh::guiLenhCua(
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

bool Cuasochinh::guiKetQuaThe(
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

bool Cuasochinh::guiThoiGianDongCua(
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

void Cuasochinh::taiDanhSachThe()
{
    const QList<ThongTinThe> danhSach =
        CoSoDuLieu::layDanhSachThe();

    hienThiDanhSachThe(danhSach);
}

void Cuasochinh::hienThiDanhSachThe(
    const QList<ThongTinThe> &danhSach
)
{
    ui->bangDanhSachThe->setRowCount(
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

        ui->bangDanhSachThe->setItem(
            dong,
            0,
            new QTableWidgetItem(the.uid)
        );

        ui->bangDanhSachThe->setItem(
            dong,
            1,
            new QTableWidgetItem(the.hoTen)
        );

        ui->bangDanhSachThe->setItem(
            dong,
            2,
            new QTableWidgetItem(the.maSo)
        );

        ui->bangDanhSachThe->setItem(
            dong,
            3,
            new QTableWidgetItem(the.vaiTro)
        );

        ui->bangDanhSachThe->setItem(
            dong,
            4,
            new QTableWidgetItem(
                the.ngayDangKy
            )
        );
    }
}

void Cuasochinh::taiLichSuRaVao()
{
    const QList<ThongTinLichSu> danhSach =
        CoSoDuLieu::layLichSuRaVao();

    hienThiLichSuRaVao(danhSach);
}

void Cuasochinh::hienThiLichSuRaVao(
    const QList<ThongTinLichSu> &danhSach
)
{
    ui->bangLichSuRaVao->setRowCount(
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

        ui->bangLichSuRaVao->setItem(
            dong,
            0,
            new QTableWidgetItem(
                lichSu.thoiGian
            )
        );

        ui->bangLichSuRaVao->setItem(
            dong,
            1,
            new QTableWidgetItem(
                lichSu.uid
            )
        );

        ui->bangLichSuRaVao->setItem(
            dong,
            2,
            new QTableWidgetItem(
                lichSu.hoTen
            )
        );

        ui->bangLichSuRaVao->setItem(
            dong,
            3,
            new QTableWidgetItem(
                lichSu.maSo
            )
        );

        ui->bangLichSuRaVao->setItem(
            dong,
            4,
            new QTableWidgetItem(
                lichSu.ketQua
            )
        );

        ui->bangLichSuRaVao->setItem(
            dong,
            5,
            new QTableWidgetItem(
                lichSu.trangThaiCua
            )
        );
    }
}

void Cuasochinh::luuLichSuRaVao(
    const QString &uid,
    const QString &hoTen,
    const QString &maSo,
    const QString &ketQua,
    const QString &trangThaiCua
)
{
    QString thongBaoLoi;

    const bool thanhCong =
        CoSoDuLieu::themLichSuRaVao(
            uid,
            hoTen,
            maSo,
            ketQua,
            trangThaiCua,
            thongBaoLoi
        );

    if (!thanhCong)
    {
        ghiNhatKy(
            "Khong luu duoc lich su: " +
            thongBaoLoi
        );

        return;
    }

    ghiNhatKy(
        "Da luu lan quet vao lich su."
    );

    taiLichSuRaVao();
}

bool Cuasochinh::kiemTraThongTinNhap()
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
        ui->oNhapUid->text().trimmed();

    const QString hoTen =
        ui->oNhapHoTen->text().trimmed();

    const QString maSo =
        ui->oNhapMaSo->text().trimmed();

    if (uid.isEmpty())
    {
        QMessageBox::warning(
            this,
            "Thong tin the",
            "Chua nhap UID the."
        );

        ui->oNhapUid->setFocus();
        return false;
    }

    if (hoTen.isEmpty())
    {
        QMessageBox::warning(
            this,
            "Thong tin the",
            "Chua nhap ho va ten."
        );

        ui->oNhapHoTen->setFocus();
        return false;
    }

    if (maSo.isEmpty())
    {
        QMessageBox::warning(
            this,
            "Thong tin the",
            "Chua nhap ma so."
        );

        ui->oNhapMaSo->setFocus();
        return false;
    }

    return true;
}

void Cuasochinh::xoaNoiDungNhap()
{
    ui->oNhapUid->clear();
    ui->oNhapHoTen->clear();
    ui->oNhapMaSo->clear();
    ui->hopVaiTro->setCurrentIndex(0);

    ui->bangDanhSachThe
        ->clearSelection();

    ui->oNhapUid->setFocus();
}

void Cuasochinh::ghiNhatKy(
    const QString &noiDung
)
{
    const QString thoiGian =
        QDateTime::currentDateTime()
            .toString("HH:mm:ss");

    ui->oNhatKy->appendPlainText(
        "[" +
        thoiGian +
        "] " +
        noiDung
    );
}
