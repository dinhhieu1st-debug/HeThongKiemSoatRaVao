#ifndef GIAOTHUCBLE_H
#define GIAOTHUCBLE_H

#include <QByteArray>
#include <QList>
#include <QPair>
#include <QString>

class GiaoThucBle
{
public:
    // Phan tich frame BLE dang TOPIC|PAYLOAD (co the gom nhieu dong phan cach bang \n)
    static bool phanTichFrame(
        const QByteArray &frame,
        QList<QPair<QString, QString>> &danhSachGoiTin,
        QString &thongBaoLoi
    );

    // Dong goi du lieu thanh frame TOPIC|PAYLOAD va kiem tra hop le
    static bool dongGoiFrame(
        const QString &topic,
        const QString &payload,
        QByteArray &frame,
        QString &thongBaoLoi
    );
};

#endif // GIAOTHUCBLE_H
