#include "giaothucble.h"

bool GiaoThucBle::phanTichFrame(
    const QByteArray &frame,
    QList<QPair<QString, QString>> &danhSachGoiTin,
    QString &thongBaoLoi
)
{
    danhSachGoiTin.clear();
    thongBaoLoi.clear();

    const QList<QByteArray> cacDong = frame.split('\n');
    bool tatCaHopLe = true;

    for (QByteArray dong : cacDong)
    {
        dong = dong.trimmed();
        if (dong.isEmpty())
        {
            continue;
        }

        const qsizetype viTri = dong.indexOf('|');

        if (viTri <= 0 || viTri >= dong.size() - 1)
        {
            thongBaoLoi = "Frame BLE khong dung TOPIC|PAYLOAD";
            tatCaHopLe = false;
            continue;
        }

        const QString topic = QString::fromUtf8(dong.left(viTri)).trimmed();
        const QString payload = QString::fromUtf8(dong.mid(viTri + 1)).trimmed();

        if (topic.isEmpty())
        {
            thongBaoLoi = "Topic khong duoc de trong";
            tatCaHopLe = false;
            continue;
        }

        danhSachGoiTin.append(qMakePair(topic, payload));
    }

    return tatCaHopLe && !danhSachGoiTin.isEmpty();
}

bool GiaoThucBle::dongGoiFrame(
    const QString &topic,
    const QString &payload,
    QByteArray &frame,
    QString &thongBaoLoi
)
{
    frame.clear();
    thongBaoLoi.clear();

    const QString topicChuan = topic.trimmed();
    const QString payloadChuan = payload.trimmed();

    if (topicChuan.isEmpty())
    {
        thongBaoLoi = "Topic khong duoc de trong";
        return false;
    }

    if (payloadChuan.contains('|'))
    {
        thongBaoLoi = "Payload khong duoc chua ky tu phan cach |";
        return false;
    }

    if (payloadChuan.contains('\n') || payloadChuan.contains('\r'))
    {
        thongBaoLoi = "Payload khong duoc chua ky tu xuong dong";
        return false;
    }

    frame = topicChuan.toUtf8() + '|' + payloadChuan.toUtf8();
    return true;
}
