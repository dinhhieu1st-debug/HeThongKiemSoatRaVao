#include "bandichtiengvietqt.h"

#include <QHash>
#include <QString>

QString BanDichTiengVietQt::translate(
    const char *,
    const char *noiDungNguon,
    const char *,
    int
) const
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
