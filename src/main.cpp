#include "cauhinhhethong.h"
#include "cosodulieu.h"
#include "cuasochinh.h"
#include "dangnhap.h"

#include <QApplication>
#include <QDialog>
#include <QEventLoop>
#include <QHash>
#include <QLocale>
#include <QLockFile>
#include <QMessageBox>
#include <QStandardPaths>
#include <QTranslator>

namespace
{
class BanDichTiengVietQt : public QTranslator
{
public:
    QString translate(
        const char *,
        const char *noiDungNguon,
        const char * = nullptr,
        int = -1
    ) const override
    {
        const QString noiDung =
            QString::fromUtf8(noiDungNguon);

        static const QHash<QString, QString> banDich =
        {
            {"OK", QString::fromUtf8("Đồng ý")},
            {"&OK", QString::fromUtf8("&Đồng ý")},
            {"Yes", QString::fromUtf8("Có")},
            {"&Yes", QString::fromUtf8("&Có")},
            {"No", QString::fromUtf8("Không")},
            {"&No", QString::fromUtf8("&Không")},
            {"Cancel", QString::fromUtf8("Hủy")},
            {"&Cancel", QString::fromUtf8("&Hủy")},
            {"Close", QString::fromUtf8("Đóng")},
            {"&Close", QString::fromUtf8("&Đóng")},
            {"Open", QString::fromUtf8("Mở")},
            {"&Open", QString::fromUtf8("&Mở")},
            {"Save", QString::fromUtf8("Lưu")},
            {"&Save", QString::fromUtf8("&Lưu")},
            {"Apply", QString::fromUtf8("Áp dụng")},
            {"Reset", QString::fromUtf8("Đặt lại")},
            {"Help", QString::fromUtf8("Trợ giúp")},
            {"Look in:", QString::fromUtf8("Xem trong:")},
            {"File name:", QString::fromUtf8("Tên tệp:")},
            {"Files of type:", QString::fromUtf8("Loại tệp:")},
            {"New Folder", QString::fromUtf8("Thư mục mới")},
            {"Back", QString::fromUtf8("Quay lại")},
            {"Forward", QString::fromUtf8("Tiếp theo")},
            {"Parent Directory", QString::fromUtf8("Thư mục cha")},
            {"List View", QString::fromUtf8("Dạng danh sách")},
            {"Detail View", QString::fromUtf8("Dạng chi tiết")}
        };

        return banDich.value(noiDung);
    }
};
}

int main(int argc, char *argv[])
{
    QLocale::setDefault(
        QLocale(
            QLocale::Vietnamese,
            QLocale::Vietnam
        )
    );

    QApplication ungDung(argc, argv);

    BanDichTiengVietQt banDichTiengVietQt;
    ungDung.installTranslator(
        &banDichTiengVietQt
    );

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
