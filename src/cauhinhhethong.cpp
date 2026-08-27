#include "cauhinhhethong.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

namespace
{

CauHinhHeThong cauHinhMacDinh()
{
    CauHinhHeThong cauHinh;

    cauHinh.thoiGianDongCuaGiay =
        15;

    return cauHinh;
}

}

QString QuanLyCauHinh::layDuongDanFile()
{
    const QString thuMucChuongTrinh =
        QCoreApplication::applicationDirPath();

    QDir thuMuc(thuMucChuongTrinh);

    if (!thuMuc.exists("config"))
    {
        thuMuc.mkdir("config");
    }

    return thuMuc.filePath(
        "config/cauhinh.json"
    );
}

bool QuanLyCauHinh::khoiTao()
{
    const QString duongDanFile =
        layDuongDanFile();

    if (QFile::exists(duongDanFile))
    {
        return true;
    }

    return luuCauHinh(
        cauHinhMacDinh()
    );
}

CauHinhHeThong QuanLyCauHinh::docCauHinh()
{
    const CauHinhHeThong macDinh =
        cauHinhMacDinh();

    QFile fileCauHinh(
        layDuongDanFile()
    );

    if (
        !fileCauHinh.open(
            QIODevice::ReadOnly |
            QIODevice::Text
        )
    )
    {
        return macDinh;
    }

    const QByteArray duLieu =
        fileCauHinh.readAll();

    fileCauHinh.close();

    QJsonParseError loiPhanTich;

    const QJsonDocument taiLieu =
        QJsonDocument::fromJson(
            duLieu,
            &loiPhanTich
        );

    if (
        loiPhanTich.error !=
            QJsonParseError::NoError ||
        !taiLieu.isObject()
    )
    {
        return macDinh;
    }

    const QJsonObject doiTuong =
        taiLieu.object();

    CauHinhHeThong cauHinh;

    const int thoiGianDongCua =
        doiTuong
            .value("thoi_gian_dong_cua_giay")
            .toInt(
                macDinh.thoiGianDongCuaGiay
            );

    if (thoiGianDongCua < 1)
    {
        cauHinh.thoiGianDongCuaGiay =
            macDinh.thoiGianDongCuaGiay;
    }
    else
    {
        cauHinh.thoiGianDongCuaGiay =
            thoiGianDongCua;
    }

    return cauHinh;
}

bool QuanLyCauHinh::luuCauHinh(
    const CauHinhHeThong &cauHinh
)
{
    if (cauHinh.thoiGianDongCuaGiay < 1)
    {
        return false;
    }

    QJsonObject doiTuong;

    doiTuong.insert(
        "thoi_gian_dong_cua_giay",
        cauHinh.thoiGianDongCuaGiay
    );

    const QJsonDocument taiLieu(
        doiTuong
    );

    QFile fileCauHinh(
        layDuongDanFile()
    );

    if (
        !fileCauHinh.open(
            QIODevice::WriteOnly |
            QIODevice::Truncate |
            QIODevice::Text
        )
    )
    {
        return false;
    }

    const qint64 soByteDaGhi =
        fileCauHinh.write(
            taiLieu.toJson(
                QJsonDocument::Indented
            )
        );

    fileCauHinh.close();

    return soByteDaGhi > 0;
}
