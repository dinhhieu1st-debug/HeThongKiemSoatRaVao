#include "cauhinhhethong.h"
#include "cosodulieu.h"
#include "cuasochinh.h"
#include "dangnhap.h"

#include <QApplication>
#include <QDialog>
#include <QEventLoop>
#include <QLockFile>
#include <QMessageBox>
#include <QStandardPaths>

int main(int argc, char *argv[])
{
    QApplication ungDung(argc, argv);

    // Tao file khoa de ngan mo nhieu chuong trinh cung luc
    const QString thuMucTam =
        QStandardPaths::writableLocation(
            QStandardPaths::TempLocation
        );

    QLockFile khoaChuongTrinh(
        thuMucTam +
        "/hethongkiemsoatravao.lock"
    );

    khoaChuongTrinh.setStaleLockTime(0);

    if (!khoaChuongTrinh.tryLock(100))
    {
        QMessageBox::warning(
            nullptr,
            "Chuong trinh dang chay",
            "He thong kiem soat ra vao da duoc mo.\n"
            "Khong the mo them mot chuong trinh nua."
        );

        return 0;
    }

    // Khoi tao file cau hinh he thong
    if (!QuanLyCauHinh::khoiTao())
    {
        QMessageBox::critical(
            nullptr,
            "Loi cau hinh",
            "Khong the khoi tao file cau hinh he thong."
        );

        return 1;
    }

    // Khoi tao co so du lieu
    if (!CoSoDuLieu::khoiTao())
    {
        QMessageBox::critical(
            nullptr,
            "Loi co so du lieu",
            "Khong the khoi tao co so du lieu."
        );

        return 1;
    }

    // Khong tu dong thoat khi cua so chinh dong de con quay lai dang nhap
    ungDung.setQuitOnLastWindowClosed(false);

    bool tiepTucChay = true;

    while (tiepTucChay)
    {
        DangNhap cuaSoDangNhap;

        // Dong man hinh dang nhap thi ket thuc chuong trinh
        if (cuaSoDangNhap.exec() != QDialog::Accepted)
        {
            break;
        }

        const QString tenDangNhap =
            cuaSoDangNhap.layTenDangNhap();

        const QString hoTenNguoiDung =
            cuaSoDangNhap.layHoTenNguoiDung();

        const QString quyenNguoiDung =
            cuaSoDangNhap.layQuyenNguoiDung();

        Cuasochinh cuaSoChinh(
            tenDangNhap,
            hoTenNguoiDung,
            quyenNguoiDung
        );

        bool daDangXuat = false;

        QEventLoop vongLapCuaSoChinh;

        QObject::connect(
            &cuaSoChinh,
            &Cuasochinh::yeuCauDangXuat,
            &vongLapCuaSoChinh,
            [&daDangXuat, &vongLapCuaSoChinh]()
            {
                daDangXuat = true;
                vongLapCuaSoChinh.quit();
            }
        );

        QObject::connect(
            &cuaSoChinh,
            &QMainWindow::destroyed,
            &vongLapCuaSoChinh,
            &QEventLoop::quit
        );

        QObject::connect(
            &cuaSoChinh,
            &QMainWindow::destroyed,
            &ungDung,
            []()
            {
            }
        );

        cuaSoChinh.show();

        QObject::connect(
            &cuaSoChinh,
            &QMainWindow::destroyed,
            &vongLapCuaSoChinh,
            &QEventLoop::quit
        );

        QObject::connect(
            &cuaSoChinh,
            &QMainWindow::windowTitleChanged,
            &vongLapCuaSoChinh,
            []()
            {
            }
        );

        vongLapCuaSoChinh.exec();

        if (!daDangXuat)
        {
            tiepTucChay = false;
        }
    }

    return 0;
}
