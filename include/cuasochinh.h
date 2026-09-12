#ifndef CUASOCHINH_H
#define CUASOCHINH_H

#include "cosodulieu.h"

#include <QList>
#include <QMainWindow>
#include <QString>

#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui
{
class CuaSoChinh;
}
QT_END_NAMESPACE

class KetNoiBle;

class CuaSoChinh : public QMainWindow
{
    Q_OBJECT

public:
    explicit CuaSoChinh(
        const QString &tenDangNhap,
        const QString &hoTenNguoiDung,
        const QString &quyenNguoiDung,
        QWidget *cha = nullptr
    );

    ~CuaSoChinh();

signals:
    // Bao cho main.cpp biet nguoi dung muon dang xuat
    void yeuCauDangXuat();

private slots:
    // Dang xuat khoi tai khoan hien tai
    void xuLyDangXuat();

    // Cac chuc nang quan ly the RFID
    void xuLyThemThe();
    void xuLyCapNhatThe();
    void xuLyXoaThe();
    void xuLyTimKiemThe();
    void xuLyTaiLaiDanhSach();
    void xuLyLayUidVuaQuet();
    void xuLyChonDongTrongBang(int dong, int cot);

    // Cac chuc nang lich su ra vao
    void xuLyTimKiemLichSu();
    void xuLyTaiLaiLichSu();
    void xuLyXuatCsvLichSu();

    // Mo cua so quan ly tai khoan
    void xuLyMoQuanLyTaiKhoan();

    void xuLyYeuCauKetNoiBle();

    void xuLyBleDaKetNoi();
    void xuLyBleDaNgatKetNoi();
    void xuLyLoiBle(const QString &noiDungLoi);

    // Mot diem vao chung cho du lieu tu ESP32
    void xuLyDuLieuTuThietBi(
        const QString &topic,
        const QString &payload
    );

    // Gui lenh dieu khien cua qua kenh dang hoat dong
    void xuLyMoCua();
    void xuLyDongCua();

private:
    void themTheTuGiaoDien();
    void capNhatTheTuGiaoDien();
    void xoaTheTuGiaoDien();
    void xuatCsvLichSu();

    // Xu ly nghiep vu sau khi slot BLE da phan luong frame.
    void xuLyUidRfid(const QString &uid);
    void xuLyTrangThaiCamBien(const QString &trangThai);
    void xuLyTrangThaiHeThong(const QString &trangThai);
    void xuLyTrangThaiCua(const QString &trangThai);

    // Ap dung quyen cua tai khoan dang nhap
    void apDungPhanQuyen();

    // Tai danh sach the tu SQLite
    void taiDanhSachThe();

    // Hien thi danh sach the len bang
    void hienThiDanhSachThe(
        const QList<ThongTinThe> &danhSach
    );

    // Tai lich su ra vao tu SQLite
    void taiLichSuRaVao();

    // Hien thi lich su ra vao len bang
    void hienThiLichSuRaVao(
        const QList<ThongTinLichSu> &danhSach
    );

    // Luu mot lan quet the vao lich su
    int luuLichSuRaVao(
        const QString &uid,
        const QString &hoTen,
        const QString &maSo,
        const QString &ketQua,
        const QString &trangThaiCua
    );

    // Kiem tra cac o nhap thong tin the
    bool kiemTraThongTinNhap();

    // Xoa noi dung cac o nhap
    void xoaNoiDungNhap();

    // Ghi thong bao vao nhat ky
    void ghiNhatKy(const QString &noiDung);

    // Gui du lieu qua BLE
    bool guiDuLieu(
        const QString &topic,
        const QString &payload
    );
    bool guiLenhCua(const QString &lenh);
    bool guiKetQuaThe(const QString &ketQua);
    bool guiThoiGianDongCua(int soGiay);

    void capNhatTrangThaiKetNoi();
    void moHopThoaiChonBle();

    std::unique_ptr<Ui::CuaSoChinh> ui;

    // Doi tuong BLE Central/GATT Client
    KetNoiBle *ketNoiBle;

    // Luu UID gan nhat nhan duoc tu ESP32
    QString uidVuaQuet;

    // Ban ghi dang cho ESP32 xac nhan cua da mo
    int idLichSuDangChoMoCua;

    // Thong tin tai khoan dang su dung
    QString tenDangNhapHienTai;
    QString hoTenNguoiDungHienTai;
    QString quyenNguoiDungHienTai;
};

#endif
