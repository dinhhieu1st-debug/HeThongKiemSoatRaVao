#ifndef CAUHINHHETHONG_H
#define CAUHINHHETHONG_H

#include <QString>

struct CauHinhHeThong
{
    int thoiGianDongCuaGiay;
};

class QuanLyCauHinh
{
public:
    // Tao file cau hinh mac dinh neu chua ton tai
    static bool khoiTao();

    // Doc cau hinh tu file
    static CauHinhHeThong docCauHinh();

    // Luu cau hinh vao file
    static bool luuCauHinh(
        const CauHinhHeThong &cauHinh
    );

    // Lay duong dan file cau hinh
    static QString layDuongDanFile();
};

#endif
