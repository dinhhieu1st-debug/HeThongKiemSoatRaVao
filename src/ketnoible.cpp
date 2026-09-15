#include "ketnoible.h"

#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothHostInfo>
#include <QBluetoothLocalDevice>
#include <QBluetoothUuid>
#include <QLowEnergyController>
#include <QLowEnergyDescriptor>
#include <QLowEnergyService>
#include <QProcess>
#include <QTimer>

#include <utility>

namespace
{

const QString TEN_THIET_BI =
    "HE THONG RA VAO HIEU";

const QBluetoothUuid UUID_DICH_VU(
    QString("7d9a0001-6f71-4b88-9a5e-40cba20d1000")
);

const QBluetoothUuid UUID_ESP32_GUI_PI(
    QString("7d9a0002-6f71-4b88-9a5e-40cba20d1000")
);

const QBluetoothUuid UUID_PI_GUI_ESP32(
    QString("7d9a0003-6f71-4b88-9a5e-40cba20d1000")
);

const int THOI_GIAN_QUET_MS = 10000;
const int THOI_GIAN_ON_DINH_SAU_QUET_MS = 1200;
const int THOI_GIAN_THU_LAI_MS = 3000;
const int THOI_GIAN_THU_LAI_SAU_NGAT_MS = 8000;

QBluetoothAddress timDiaChiAdapterCucBo()
{
    const QList<QBluetoothHostInfo> cacAdapter =
        QBluetoothLocalDevice::allDevices();

    if (cacAdapter.isEmpty())
    {
        return QBluetoothAddress();
    }

    return cacAdapter.first().address();
}

}

KetNoiBle::KetNoiBle(QObject *cha)
    : QObject(cha),
      diaChiAdapterCucBo(timDiaChiAdapterCucBo()),
      boQuet(nullptr),
      controller(nullptr),
      dichVu(nullptr),
      timerConnectAfterScan(new QTimer(this)),
      timerReconnect(new QTimer(this)),
      dangGhi(false),
      dangHoatDong(false),
      dangQuetLuaChon(false),
      dangChoQuetDung(false),
      sanSang(false),
      daBaoLoiLanNay(false),
      dangKhoiPhucAdapter(false),
      soLanThuKetNoi(0)
{
    taoLaiBoQuet();

    timerConnectAfterScan->setSingleShot(true);
    timerConnectAfterScan->setInterval(
        THOI_GIAN_ON_DINH_SAU_QUET_MS
    );

    timerReconnect->setSingleShot(true);
    timerReconnect->setInterval(
        THOI_GIAN_THU_LAI_MS
    );

    connect(
        timerConnectAfterScan,
        &QTimer::timeout,
        this,
        &KetNoiBle::ketNoiSauKhiQuetDung
    );

    connect(
        timerReconnect,
        &QTimer::timeout,
        this,
        &KetNoiBle::batDauQuetKetNoiLai
    );
}

void KetNoiBle::taoLaiBoQuet()
{
    if (boQuet != nullptr)
    {
        boQuet->disconnect(this);

        if (boQuet->isActive())
        {
            boQuet->stop();
        }

        boQuet->deleteLater();
        boQuet = nullptr;
    }

    // Sau khi hci0 bi power-cycle, backend BlueZ cua discovery agent cu
    // khong con hop le. Lay lai adapter va tao agent moi truoc khi quet.
    diaChiAdapterCucBo = timDiaChiAdapterCucBo();
    boQuet = new QBluetoothDeviceDiscoveryAgent(
        diaChiAdapterCucBo,
        this
    );
    boQuet->setLowEnergyDiscoveryTimeout(
        THOI_GIAN_QUET_MS
    );

    connect(
        boQuet,
        &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
        this,
        &KetNoiBle::xuLyTimThayThietBi
    );

    connect(
        boQuet,
        &QBluetoothDeviceDiscoveryAgent::deviceUpdated,
        this,
        [this](
            const QBluetoothDeviceInfo &thongTin,
            QBluetoothDeviceInfo::Fields
        )
        {
            xuLyTimThayThietBi(thongTin);
        }
    );

    connect(
        boQuet,
        &QBluetoothDeviceDiscoveryAgent::finished,
        this,
        &KetNoiBle::xuLyQuetDaDung
    );

    connect(
        boQuet,
        &QBluetoothDeviceDiscoveryAgent::canceled,
        this,
        &KetNoiBle::xuLyQuetDaDung
    );

    connect(
        boQuet,
        &QBluetoothDeviceDiscoveryAgent::errorOccurred,
        this,
        &KetNoiBle::xuLyLoiQuet
    );
}

KetNoiBle::~KetNoiBle()
{
    dung();
}

bool KetNoiBle::quetDeLuaChon()
{
    if (
        sanSang ||
        controller != nullptr ||
        boQuet->isActive()
    )
    {
        return false;
    }

    timerReconnect->stop();
    timerConnectAfterScan->stop();

    dangHoatDong = true;
    dangQuetLuaChon = true;
    dangChoQuetDung = false;
    daBaoLoiLanNay = false;
    dangKhoiPhucAdapter = false;
    soLanThuKetNoi = 0;
    diaChiDaChon.clear();
    danhSachThietBi.clear();
    thietBiDaChon = QBluetoothDeviceInfo();
    thietBiChoKetNoi = QBluetoothDeviceInfo();

    emit batDauQuet();

    boQuet->start(
        QBluetoothDeviceDiscoveryAgent::LowEnergyMethod
    );

    return true;
}

bool KetNoiBle::ketNoiTheoDiaChi(
    const QString &diaChi
)
{
    const QString diaChiChuan =
        diaChi.trimmed().toUpper();

    for (const QBluetoothDeviceInfo &thongTin :
         std::as_const(danhSachThietBi))
    {
        if (
            thongTin.address()
                .toString()
                .toUpper() != diaChiChuan
        )
        {
            continue;
        }

        diaChiDaChon = diaChiChuan;
        thietBiDaChon = thongTin;
        dangQuetLuaChon = false;
        dangHoatDong = true;
        daBaoLoiLanNay = false;
        soLanThuKetNoi = 0;
        henKetNoiThietBi(thongTin);
        return true;
    }

    return false;
}

void KetNoiBle::dung()
{
    dangHoatDong = false;
    dangQuetLuaChon = false;
    dangChoQuetDung = false;
    daBaoLoiLanNay = false;
    soLanThuKetNoi = 0;

    timerReconnect->stop();
    timerConnectAfterScan->stop();

    if (boQuet->isActive())
    {
        boQuet->stop();
    }

    QLowEnergyController *controllerDangDong = controller;

    if (
        controllerDangDong != nullptr &&
        controllerDangDong->state() !=
            QLowEnergyController::UnconnectedState
    )
    {
        // Khong xoa controller ngay sau disconnectFromDevice(). Neu xoa
        // khi Qt con o ClosingState, BlueZ/Cypress co the giu lien ket HCI
        // cu va cac lan ket noi sau se lien tuc gap 0x3e.
        controllerDangDong->disconnectFromDevice();

        QTimer::singleShot(
            3000,
            this,
            [this, controllerDangDong]()
            {
                if (controller == controllerDangDong)
                {
                    xoaController();
                }
            }
        );
    }
    else
    {
        xoaController();
    }

    datSanSang(false);

    diaChiDaChon.clear();
    danhSachThietBi.clear();
    thietBiDaChon = QBluetoothDeviceInfo();
    thietBiChoKetNoi = QBluetoothDeviceInfo();
}

bool KetNoiBle::daKetNoi() const
{
    return sanSang;
}

bool KetNoiBle::dangXuLy() const
{
    return dangHoatDong;
}

bool KetNoiBle::guiDuLieu(
    const QString &topic,
    const QString &payload
)
{
    if (
        !sanSang ||
        dichVu == nullptr ||
        !characteristicGui.isValid() ||
        topic.trimmed().isEmpty() ||
        payload.contains('|') ||
        payload.contains('\n') ||
        payload.contains('\r')
    )
    {
        return false;
    }

    const QByteArray frame =
        topic.trimmed().toUtf8() +
        '|' +
        payload.trimmed().toUtf8();

    hangDoiGhi.enqueue(frame);

    if (!dangGhi)
    {
        guiGoiTiepTheo();
    }

    return true;
}

void KetNoiBle::guiGoiTiepTheo()
{
    if (
        hangDoiGhi.isEmpty() ||
        !sanSang ||
        dichVu == nullptr ||
        !characteristicGui.isValid()
    )
    {
        dangGhi = false;
        return;
    }

    dangGhi = true;
    const QByteArray frame = hangDoiGhi.dequeue();

    const auto thuocTinh =
        characteristicGui.properties();

    if (
        thuocTinh.testFlag(
            QLowEnergyCharacteristic::Write
        )
    )
    {
        dichVu->writeCharacteristic(
            characteristicGui,
            frame,
            QLowEnergyService::WriteWithResponse
        );
    }
    else if (
        thuocTinh.testFlag(
            QLowEnergyCharacteristic::WriteNoResponse
        )
    )
    {
        dichVu->writeCharacteristic(
            characteristicGui,
            frame,
            QLowEnergyService::WriteWithoutResponse
        );

        dangGhi = false;

        if (!hangDoiGhi.isEmpty())
        {
            QTimer::singleShot(
                20,
                this,
                &KetNoiBle::guiGoiTiepTheo
            );
        }
    }
    else
    {
        dangGhi = false;
    }
}

void KetNoiBle::xuLyCharacteristicDaGhi(
    const QLowEnergyCharacteristic &characteristic,
    const QByteArray &giaTri
)
{
    Q_UNUSED(characteristic);
    Q_UNUSED(giaTri);

    dangGhi = false;

    if (!hangDoiGhi.isEmpty())
    {
        guiGoiTiepTheo();
    }
}

void KetNoiBle::xuLyTimThayThietBi(
    const QBluetoothDeviceInfo &thongTin
)
{
    const bool dungTen =
        thongTin.name().trimmed().compare(
            TEN_THIET_BI,
            Qt::CaseInsensitive
        ) == 0;

    const bool dungUuid =
        thongTin.serviceUuids().contains(
            UUID_DICH_VU
        );

    const QString diaChi =
        thongTin.address().toString().toUpper();

    // BlueZ co the con giu ten cu trong cache. UUID dich vu la dau hieu
    // chinh xac cua ESP32, vi vay luon hien thi ten he thong hien tai.
    const QString tenHienThi =
        dungUuid
            ? TEN_THIET_BI
            : (
                thongTin.name().trimmed().isEmpty()
                    ? "Khong ro ten"
                    : thongTin.name().trimmed()
            );

    if (dangQuetLuaChon)
    {
        bool daCo = false;

        for (QBluetoothDeviceInfo &daLuu :
             danhSachThietBi)
        {
            if (daLuu.address() == thongTin.address())
            {
                daLuu = thongTin;
                daCo = true;
                break;
            }
        }

        if (!daCo)
        {
            danhSachThietBi.append(thongTin);
        }

        emit timThayThietBi(
            tenHienThi,
            diaChi,
            static_cast<int>(thongTin.rssi()),
            dungTen || dungUuid
        );
        return;
    }

    if (
        diaChiDaChon.isEmpty() ||
        diaChi != diaChiDaChon
    )
    {
        return;
    }

    henKetNoiThietBi(thongTin);
}

void KetNoiBle::xuLyQuetDaDung()
{
    if (dangChoQuetDung)
    {
        dangChoQuetDung = false;

        if (!timerConnectAfterScan->isActive())
        {
            timerConnectAfterScan->start();
        }
        return;
    }

    if (timerConnectAfterScan->isActive())
    {
        return;
    }

    if (dangQuetLuaChon)
    {
        emit ketThucQuet();
        return;
    }

    if (
        dangHoatDong &&
        controller == nullptr &&
        !sanSang
    )
    {
        henThuLai();
    }
}

void KetNoiBle::xuLyLoiQuet()
{
    emit coLoi(
        "Khong the quet BLE: " +
        boQuet->errorString()
    );

    if (!dangQuetLuaChon)
    {
        henThuLai();
    }
}

void KetNoiBle::henKetNoiThietBi(
    const QBluetoothDeviceInfo &thongTin
)
{
    if (
        controller != nullptr ||
        timerConnectAfterScan->isActive()
    )
    {
        return;
    }

    thietBiChoKetNoi = thongTin;

    if (boQuet->isActive())
    {
        dangChoQuetDung = true;
        boQuet->stop();
        return;
    }

    timerConnectAfterScan->start();
}

void KetNoiBle::ketNoiSauKhiQuetDung()
{
    if (
        !dangHoatDong ||
        sanSang ||
        boQuet->isActive() ||
        controller != nullptr ||
        !thietBiChoKetNoi.isValid()
    )
    {
        return;
    }

    const QBluetoothDeviceInfo thongTin =
        thietBiChoKetNoi;

    thietBiChoKetNoi = QBluetoothDeviceInfo();
    taoControllerVaKetNoi(thongTin);
}

void KetNoiBle::taoControllerVaKetNoi(
    const QBluetoothDeviceInfo &thongTin
)
{
    xoaController();
    daBaoLoiLanNay = false;
    soLanThuKetNoi++;

    // Adapter co the vua duoc power-cycle. Luon lay lai dia chi local
    // truoc khi tao controller moi.
    diaChiAdapterCucBo = timDiaChiAdapterCucBo();

    controller =
        QLowEnergyController::createCentral(
            thongTin,
            diaChiAdapterCucBo,
            this
        );

    // ESP32-D0WD-V3 dung dia chi public.
    controller->setRemoteAddressType(
        QLowEnergyController::PublicAddress
    );

    connect(
        controller,
        &QLowEnergyController::connected,
        this,
        &KetNoiBle::xuLyControllerDaKetNoi
    );

    connect(
        controller,
        &QLowEnergyController::disconnected,
        this,
        &KetNoiBle::xuLyControllerDaNgat
    );

    connect(
        controller,
        &QLowEnergyController::errorOccurred,
        this,
        &KetNoiBle::xuLyLoiController
    );

    connect(
        controller,
        &QLowEnergyController::discoveryFinished,
        this,
        &KetNoiBle::xuLyTimDichVuXong
    );

    controller->connectToDevice();
}

void KetNoiBle::xuLyControllerDaKetNoi()
{
    if (controller != nullptr)
    {
        controller->discoverServices();
    }
}

void KetNoiBle::xuLyControllerDaNgat()
{
    const bool daTungKetNoiOnDinh = sanSang;

    datSanSang(false);
    xoaController();

    if (daTungKetNoiOnDinh)
    {
        soLanThuKetNoi = 0;
        timerReconnect->setInterval(
            THOI_GIAN_THU_LAI_SAU_NGAT_MS
        );
    }

    henThuLai();
}

void KetNoiBle::xuLyLoiController()
{
    bool canKhoiPhucAdapter = false;

    if (
        controller != nullptr &&
        !daBaoLoiLanNay
    )
    {
        daBaoLoiLanNay = true;

        if (
            controller->error() ==
                QLowEnergyController::UnknownError
        )
        {
            canKhoiPhucAdapter =
                soLanThuKetNoi > 0 &&
                soLanThuKetNoi % 3 == 0;

            emit coLoi(
                "Bat tay BLE HCI lan " +
                QString::number(soLanThuKetNoi) +
                " chua thanh cong (BlueZ/Qt ma 1, "
                "khong phai ghep doi). Se tu thu lai."
            );
        }
        else
        {
            emit coLoi(
                "Loi ket noi BLE: " +
                controller->errorString() +
                " (ma " +
                QString::number(
                    static_cast<int>(controller->error())
                ) +
                ")"
            );
        }
    }

    datSanSang(false);

    QLowEnergyController *controllerBiLoi = controller;

    if (
        controllerBiLoi != nullptr &&
        controllerBiLoi->state() !=
            QLowEnergyController::UnconnectedState
    )
    {
        controllerBiLoi->disconnectFromDevice();
    }

    // BlueZ co the bao le-connection-abort-by-local ma khong phat
    // disconnected(). Khong duoc cho vo han tin hieu do, vi controller
    // con ton tai se chan bo hen thu lai. Doi het callback hien tai roi
    // chu dong don controller va hen quet lai.
    QTimer::singleShot(
        1000,
        this,
        [this, controllerBiLoi, canKhoiPhucAdapter]()
        {
            if (controller != controllerBiLoi)
            {
                return;
            }

            xoaController();

            if (canKhoiPhucAdapter)
            {
                khoiPhucAdapterBluetooth();
            }
            else
            {
                henThuLai();
            }
        }
    );
}

void KetNoiBle::xuLyTimDichVuXong()
{
    if (controller == nullptr)
    {
        return;
    }

    dichVu = controller->createServiceObject(
        UUID_DICH_VU,
        this
    );

    if (dichVu == nullptr)
    {
        emit coLoi(
            "Thiet bi khong co BLE service cua he thong"
        );
        controller->disconnectFromDevice();
        return;
    }

    connect(
        dichVu,
        &QLowEnergyService::stateChanged,
        this,
        &KetNoiBle::xuLyTrangThaiDichVuThayDoi
    );

    connect(
        dichVu,
        &QLowEnergyService::characteristicChanged,
        this,
        &KetNoiBle::xuLyCharacteristicThayDoi
    );

    connect(
        dichVu,
        &QLowEnergyService::descriptorWritten,
        this,
        &KetNoiBle::xuLyDescriptorDaGhi
    );

    connect(
        dichVu,
        &QLowEnergyService::characteristicWritten,
        this,
        &KetNoiBle::xuLyCharacteristicDaGhi
    );

    connect(
        dichVu,
        &QLowEnergyService::errorOccurred,
        this,
        &KetNoiBle::xuLyLoiDichVu
    );

    dichVu->discoverDetails();
}

void KetNoiBle::xuLyTrangThaiDichVuThayDoi()
{
    if (
        dichVu == nullptr ||
        dichVu->state() !=
            QLowEnergyService::RemoteServiceDiscovered
    )
    {
        return;
    }

    characteristicNhan =
        dichVu->characteristic(UUID_ESP32_GUI_PI);

    characteristicGui =
        dichVu->characteristic(UUID_PI_GUI_ESP32);

    if (
        !characteristicNhan.isValid() ||
        !characteristicGui.isValid()
    )
    {
        emit coLoi(
            "Thiet bi thieu BLE characteristic"
        );
        controller->disconnectFromDevice();
        return;
    }

    const QLowEnergyDescriptor cccd =
        characteristicNhan.descriptor(
            QBluetoothUuid::DescriptorType::
                ClientCharacteristicConfiguration
        );

    if (!cccd.isValid())
    {
        emit coLoi(
            "BLE characteristic Notify khong co CCCD"
        );
        controller->disconnectFromDevice();
        return;
    }

    dichVu->writeDescriptor(
        cccd,
        QByteArray::fromHex("0100")
    );
}

void KetNoiBle::xuLyDescriptorDaGhi(
    const QLowEnergyDescriptor &descriptor,
    const QByteArray &giaTri
)
{
    if (
        descriptor.uuid() ==
            QBluetoothUuid(
                QBluetoothUuid::DescriptorType::
                    ClientCharacteristicConfiguration
            ) &&
        giaTri == QByteArray::fromHex("0100")
    )
    {
        datSanSang(true);
    }
}

void KetNoiBle::xuLyCharacteristicThayDoi(
    const QLowEnergyCharacteristic &characteristic,
    const QByteArray &giaTri
)
{
    if (
        sanSang &&
        characteristic.uuid() == UUID_ESP32_GUI_PI
    )
    {
        xuLyFrame(giaTri);
    }
}

void KetNoiBle::xuLyLoiDichVu()
{
    if (dichVu != nullptr)
    {
        emit coLoi(
            "Loi GATT BLE (ma " +
            QString::number(
                static_cast<int>(dichVu->error())
            ) +
            ")"
        );
    }

    datSanSang(false);

    if (controller != nullptr)
    {
        controller->disconnectFromDevice();
    }
}

void KetNoiBle::datSanSang(bool giaTri)
{
    if (sanSang == giaTri)
    {
        return;
    }

    sanSang = giaTri;

    if (sanSang)
    {
        daBaoLoiLanNay = false;
        timerReconnect->stop();
        emit daKetNoiBle();
    }
    else
    {
        hangDoiGhi.clear();
        dangGhi = false;
        emit daNgatKetNoiBle();
    }
}

void KetNoiBle::xoaController()
{
    hangDoiGhi.clear();
    dangGhi = false;
    characteristicNhan = QLowEnergyCharacteristic();
    characteristicGui = QLowEnergyCharacteristic();

    if (dichVu != nullptr)
    {
        dichVu->disconnect(this);
        dichVu->deleteLater();
        dichVu = nullptr;
    }

    if (controller != nullptr)
    {
        controller->disconnect(this);
        controller->deleteLater();
        controller = nullptr;
    }
}

void KetNoiBle::henThuLai()
{
    if (
        dangHoatDong &&
        !dangQuetLuaChon &&
        !diaChiDaChon.isEmpty() &&
        !timerReconnect->isActive()
    )
    {
        timerReconnect->start();
    }
}

void KetNoiBle::khoiPhucAdapterBluetooth()
{
    if (dangKhoiPhucAdapter)
    {
        return;
    }

    dangKhoiPhucAdapter = true;
    timerReconnect->stop();
    timerConnectAfterScan->stop();

    emit coLoi(
        "Da co 3 lan bat tay HCI chua thanh cong. "
        "Dang khoi phuc adapter Bluetooth Pi..."
    );

    QProcess *tatAdapter = new QProcess(this);

    connect(
        tatAdapter,
        qOverload<int, QProcess::ExitStatus>(&QProcess::finished),
        this,
        [this, tatAdapter](int, QProcess::ExitStatus)
        {
            tatAdapter->deleteLater();

            QTimer::singleShot(
                1500,
                this,
                [this]()
                {
                    QProcess *batAdapter = new QProcess(this);

                    connect(
                        batAdapter,
                        qOverload<int, QProcess::ExitStatus>(
                            &QProcess::finished
                        ),
                        this,
                        [this, batAdapter](int, QProcess::ExitStatus)
                        {
                            batAdapter->deleteLater();

                            // Cho BlueZ dang ky lai hci0 roi tao discovery
                            // agent moi. Agent cu duoc tao truoc power-cycle
                            // co the khong phat finished/error va lam ket noi
                            // lai bi ket vo han.
                            QTimer::singleShot(
                                2500,
                                this,
                                [this]()
                                {
                                    taoLaiBoQuet();

                                    // Power-cycle lam BlueZ xoa Device1.
                                    // Quet tableBleDevices tien trinh rieng de tao lai
                                    // Device1, sau do Qt se ket noi truc tiep
                                    // tableBleDevices thong tin da chon, khong scan lai.
                                    QProcess *quetBluez =
                                        new QProcess(this);

                                    connect(
                                        quetBluez,
                                        qOverload<
                                            int,
                                            QProcess::ExitStatus
                                        >(&QProcess::finished),
                                        this,
                                        [this, quetBluez](
                                            int,
                                            QProcess::ExitStatus
                                        )
                                        {
                                            quetBluez->deleteLater();
                                            dangKhoiPhucAdapter = false;
                                            soLanThuKetNoi = 0;

                                            emit coLoi(
                                                "Adapter Bluetooth Pi da khoi phuc. "
                                                "Da lam moi thiet bi, dang ket noi "
                                                "truc tiep BLE."
                                            );

                                            if (dangHoatDong)
                                            {
                                                timerReconnect->setInterval(
                                                    THOI_GIAN_THU_LAI_MS
                                                );
                                                henThuLai();
                                            }
                                        }
                                    );

                                    quetBluez->start(
                                        "bluetoothctl",
                                        {
                                            "--timeout",
                                            "6",
                                            "scan",
                                            "le"
                                        }
                                    );
                                }
                            );
                        }
                    );

                    batAdapter->start(
                        "bluetoothctl",
                        {"power", "on"}
                    );
                }
            );
        }
    );

    tatAdapter->start(
        "bluetoothctl",
        {"power", "off"}
    );
}

void KetNoiBle::batDauQuetKetNoiLai()
{
    if (
        !dangHoatDong ||
        sanSang ||
        controller != nullptr ||
        boQuet->isActive() ||
        diaChiDaChon.isEmpty() ||
        !thietBiDaChon.isValid()
    )
    {
        return;
    }

    daBaoLoiLanNay = false;
    timerReconnect->setInterval(THOI_GIAN_THU_LAI_MS);

    // Khong scan lai truoc moi lan connect. Cypress tren Pi de gap HCI
    // 0x3e khi chuyen lien tuc tu discovery sang LE connection. Thong tin
    // dia chi public cua ESP32 da duoc luu tu lan nguoi dung chon.
    thietBiChoKetNoi = thietBiDaChon;
    timerConnectAfterScan->start();
}

void KetNoiBle::xuLyFrame(const QByteArray &frame)
{
    const QList<QByteArray> cacDong =
        frame.split('\n');

    for (QByteArray dong : cacDong)
    {
        dong = dong.trimmed();

        const qsizetype viTri = dong.indexOf('|');

        if (
            viTri <= 0 ||
            viTri >= dong.size() - 1
        )
        {
            emit coLoi(
                "Frame BLE khong dung TOPIC|PAYLOAD"
            );
            continue;
        }

        emit nhanDuLieu(
            QString::fromUtf8(dong.left(viTri)).trimmed(),
            QString::fromUtf8(dong.mid(viTri + 1)).trimmed()
        );
    }
}
