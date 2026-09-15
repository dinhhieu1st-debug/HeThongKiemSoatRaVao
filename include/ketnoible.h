#ifndef KETNOIBLE_H
#define KETNOIBLE_H

#include <QByteArray>
#include <QBluetoothAddress>
#include <QBluetoothDeviceInfo>
#include <QLowEnergyCharacteristic>
#include <QLowEnergyDescriptor>
#include <QObject>
#include <QQueue>
#include <QString>

class QBluetoothDeviceDiscoveryAgent;
class QLowEnergyController;
class QLowEnergyService;
class QTimer;

class KetNoiBle : public QObject
{
    Q_OBJECT

public:
    explicit KetNoiBle(QObject *cha = nullptr);
    ~KetNoiBle() override;

    bool quetDeLuaChon();
    bool ketNoiTheoDiaChi(const QString &diaChi);
    void dung();

    bool daKetNoi() const;
    bool dangXuLy() const;

    bool guiDuLieu(
        const QString &topic,
        const QString &payload
    );

signals:
    void batDauQuet();
    void timThayThietBi(
        const QString &ten,
        const QString &diaChi,
        int rssi,
        bool dungHeThong
    );
    void ketThucQuet();

    void daKetNoiBle();
    void daNgatKetNoiBle();
    void coLoi(const QString &noiDung);

    void nhanDuLieu(
        const QString &topic,
        const QString &payload
    );

private slots:
    void xuLyTimThayThietBi(
        const QBluetoothDeviceInfo &thongTin
    );
    void xuLyQuetDaDung();
    void xuLyLoiQuet();
    void ketNoiSauKhiQuetDung();
    void batDauQuetKetNoiLai();

    void xuLyControllerDaKetNoi();
    void xuLyControllerDaNgat();
    void xuLyLoiController();
    void xuLyTimDichVuXong();

    void xuLyTrangThaiDichVuThayDoi();
    void xuLyCharacteristicThayDoi(
        const QLowEnergyCharacteristic &characteristic,
        const QByteArray &giaTri
    );
    void xuLyDescriptorDaGhi(
        const QLowEnergyDescriptor &descriptor,
        const QByteArray &giaTri
    );
    void xuLyCharacteristicDaGhi(
        const QLowEnergyCharacteristic &characteristic,
        const QByteArray &giaTri
    );
    void xuLyLoiDichVu();

private:
    void taoLaiBoQuet();
    void henKetNoiThietBi(
        const QBluetoothDeviceInfo &thongTin
    );
    void taoControllerVaKetNoi(
        const QBluetoothDeviceInfo &thongTin
    );
    void datSanSang(bool sanSang);
    void xoaController();
    void henThuLai();
    void khoiPhucAdapterBluetooth();
    void xuLyFrame(const QByteArray &frame);
    void guiGoiTiepTheo();

    QBluetoothAddress diaChiAdapterCucBo;
    QBluetoothDeviceDiscoveryAgent *boQuet;
    QLowEnergyController *controller;
    QLowEnergyService *dichVu;
    QTimer *timerConnectAfterScan;
    QTimer *timerReconnect;

    QList<QBluetoothDeviceInfo> danhSachThietBi;
    QBluetoothDeviceInfo thietBiDaChon;
    QBluetoothDeviceInfo thietBiChoKetNoi;
    QLowEnergyCharacteristic characteristicNhan;
    QLowEnergyCharacteristic characteristicGui;
    QString diaChiDaChon;

    QQueue<QByteArray> hangDoiGhi;
    bool dangGhi;
    bool dangHoatDong;
    bool dangQuetLuaChon;
    bool dangChoQuetDung;
    bool sanSang;
    bool daBaoLoiLanNay;
    bool dangKhoiPhucAdapter;
    int soLanThuKetNoi;
};

#endif
