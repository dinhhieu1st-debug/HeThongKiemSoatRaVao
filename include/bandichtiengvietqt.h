#ifndef BANDICHTIENGVIETQT_H
#define BANDICHTIENGVIETQT_H

#include <QString>
#include <QTranslator>

class BanDichTiengVietQt : public QTranslator
{
public:
    QString translate(
        const char *nguCanh,
        const char *noiDungNguon,
        const char *ghiChu = nullptr,
        int soLuong = -1
    ) const override;
};

#endif
