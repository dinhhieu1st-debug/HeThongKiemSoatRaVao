#include <Arduino.h>

#include "buzzer.h"
#include "config.h"
#include "communication_service.h"
#include "obstacle_sensor.h"
#include "oled_display.h"
#include "rfid_sensor.h"
#include "servo_cua.h"

enum class TrangThaiHeThong
{
    CHO,
    CHO_QUET_THE,
    DA_DOC_THE,
    CHO_NGUOI_ROI
};

namespace
{

TrangThaiHeThong trangThaiHienTai =
    TrangThaiHeThong::CHO;

bool oledSanSang = false;
bool rfidSanSang = false;

unsigned long thoiDiemBatDauTrangThai = 0;
unsigned long thoiDiemBatDauXacNhanRoi = 0;

// Thoi gian tu dong dong cua.
// Mac dinh lay tu config.h, sau do co the nhan lai tu Qt.
unsigned long thoiGianTuDongDongCuaMs =
    CARD_WAIT_TIMEOUT_MS;

// Trang thai mo cua thu cong tu Qt
bool dangMoCuaThuCong = false;
bool daPhatHienNguoiSauKhiMo = false;
unsigned long thoiDiemMoCuaThuCong = 0;
unsigned long thoiDiemBatDauKhongConNguoi = 0;

// Bien dieu khien chuoi tieng bip
int soLanBipConLai = 0;
bool dangChoBipTiepTheo = false;
unsigned long thoiDiemKetThucLanBip = 0;

const unsigned long THOI_GIAN_MOT_TIENG_BIP = 150;
const unsigned long KHOANG_CACH_GIUA_CAC_TIENG_BIP = 180;

void batDauChuoiBip(int soLan)
{
    if (soLan <= 0)
    {
        return;
    }

    soLanBipConLai = soLan - 1;
    dangChoBipTiepTheo = false;

    Buzzer::beep(
        THOI_GIAN_MOT_TIENG_BIP
    );
}

void xuLyChuoiBip()
{
    if (
        soLanBipConLai <= 0 &&
        !dangChoBipTiepTheo
    )
    {
        return;
    }

    if (
        !Buzzer::isActive() &&
        !dangChoBipTiepTheo &&
        soLanBipConLai > 0
    )
    {
        dangChoBipTiepTheo = true;
        thoiDiemKetThucLanBip = millis();
        return;
    }

    if (
        dangChoBipTiepTheo &&
        millis() - thoiDiemKetThucLanBip >=
            KHOANG_CACH_GIUA_CAC_TIENG_BIP
    )
    {
        Buzzer::beep(
            THOI_GIAN_MOT_TIENG_BIP
        );

        soLanBipConLai--;
        dangChoBipTiepTheo = false;
    }
}

// Khai bao truoc ham dua he thong ve trang thai cho
void vaoTrangThaiCho();

void huyCheDoMoCuaThuCong()
{
    dangMoCuaThuCong = false;
    daPhatHienNguoiSauKhiMo = false;
    thoiDiemMoCuaThuCong = 0;
    thoiDiemBatDauKhongConNguoi = 0;
}

void dongCuaVaGuiTrangThai()
{
    ServoCua::dongCua();

    CommunicationService::publishDoorStatus(
        "CLOSED"
    );

    huyCheDoMoCuaThuCong();

    // Dat lai may trang thai de co the quet the tiep
    vaoTrangThaiCho();
}

// Nhan so giay tu Qt va doi sang mili giay
void xuLyThoiGianDongCua(
    unsigned long soGiay
)
{
    if (
        soGiay < 1 ||
        soGiay > 300
    )
    {
        Serial.println(
            "Thoi gian dong cua khong hop le."
        );

        return;
    }

    thoiGianTuDongDongCuaMs =
        soGiay * 1000UL;

    Serial.println();
    Serial.println("==============================");
    Serial.println("DA CAP NHAT THOI GIAN DONG CUA");
    Serial.print("SO GIAY: ");
    Serial.println(soGiay);
    Serial.println("==============================");
}

void xuLyDongCuaThuCongTuDong()
{
    if (!dangMoCuaThuCong)
    {
        return;
    }

    if (!ServoCua::dangMo())
    {
        huyCheDoMoCuaThuCong();
        return;
    }

    const bool coNguoi =
        ObstacleSensor::hasObstacle();

    if (coNguoi)
    {
        daPhatHienNguoiSauKhiMo = true;
        thoiDiemBatDauKhongConNguoi = 0;
        return;
    }

    // Da co nguoi di qua, dong cua sau khi nguoi roi 3 giay
    if (daPhatHienNguoiSauKhiMo)
    {
        if (thoiDiemBatDauKhongConNguoi == 0)
        {
            thoiDiemBatDauKhongConNguoi =
                millis();

            Serial.println(
                "MO THU CONG: DANG XAC NHAN NGUOI DA ROI..."
            );
        }

        if (
            millis() -
                thoiDiemBatDauKhongConNguoi >=
            PERSON_CLEAR_CONFIRM_MS
        )
        {
            Serial.println();
            Serial.println("==============================");
            Serial.println("MO THU CONG");
            Serial.println("NGUOI DA ROI");
            Serial.println("TU DONG DONG CUA");
            Serial.println("==============================");

            dongCuaVaGuiTrangThai();
        }

        return;
    }

    // Mo cua nhung khong co ai di qua
    if (
        millis() - thoiDiemMoCuaThuCong >=
        thoiGianTuDongDongCuaMs
    )
    {
        Serial.println();
        Serial.println("==============================");
        Serial.println("HET THOI GIAN MO CUA");
        Serial.println("KHONG CO NGUOI DI QUA");
        Serial.println("TU DONG DONG CUA");
        Serial.println("==============================");

        dongCuaVaGuiTrangThai();
    }
}

void xuLyLenhCua(const String &lenh)
{
    String lenhChuan = lenh;

    lenhChuan.trim();
    lenhChuan.toUpperCase();

    if (lenhChuan == "OPEN")
    {
        Serial.println();
        Serial.println("==============================");
        Serial.println("NHAN LENH MO CUA TU QT");
        Serial.println("MO CUA KHONG PHU THUOC CAM BIEN");
        Serial.println("==============================");

        ServoCua::moCua();

        CommunicationService::publishDoorStatus(
            "OPENED"
        );

        dangMoCuaThuCong = true;

        daPhatHienNguoiSauKhiMo =
            ObstacleSensor::hasObstacle();

        thoiDiemMoCuaThuCong =
            millis();

        thoiDiemBatDauKhongConNguoi = 0;

        return;
    }

    if (lenhChuan == "CLOSE")
    {
        Serial.println();
        Serial.println("==============================");
        Serial.println("NHAN LENH DONG CUA TU QT");
        Serial.println("==============================");

        dongCuaVaGuiTrangThai();
        return;
    }

    Serial.print("Lenh cua khong hop le: ");
    Serial.println(lenhChuan);
}

void chuanBiQuetLaiThe()
{
    trangThaiHienTai =
        TrangThaiHeThong::CHO_QUET_THE;

    thoiDiemBatDauTrangThai =
        millis();

    thoiDiemBatDauXacNhanRoi = 0;

    RFIDSensor::wake();

    if (oledSanSang)
    {
        OLEDDisplay::wake();
        OLEDDisplay::showWaiting(true);
    }

    Serial.println();
    Serial.println("==============================");
    Serial.println("THE SAI");
    Serial.println("CHO PHEP QUET LAI NGAY");
    Serial.println("==============================");
}

void xuLyKetQuaThe(const String &ketQua)
{
    String ketQuaChuan = ketQua;

    ketQuaChuan.trim();
    ketQuaChuan.toUpperCase();

    if (ketQuaChuan == "GRANTED")
    {
        Serial.println();
        Serial.println("==============================");
        Serial.println("THE HOP LE");
        Serial.println("MO CUA VA KEU 1 TIENG BIP");
        Serial.println("==============================");

        batDauChuoiBip(1);
        xuLyLenhCua("OPEN");
        return;
    }

    if (ketQuaChuan == "DENIED")
    {
        Serial.println();
        Serial.println("==============================");
        Serial.println("THE KHONG HOP LE");
        Serial.println("KEU 3 TIENG BIP");
        Serial.println("==============================");

        if (
            ServoCua::dangMo() &&
            !dangMoCuaThuCong
        )
        {
            dongCuaVaGuiTrangThai();
        }

        batDauChuoiBip(3);

        // Cho phep quet the khac ngay
        chuanBiQuetLaiThe();
        return;
    }

    Serial.print(
        "Ket qua the khong hop le: "
    );

    Serial.println(ketQuaChuan);
}

void vaoTrangThaiCho()
{
    trangThaiHienTai =
        TrangThaiHeThong::CHO;

    thoiDiemBatDauTrangThai =
        millis();

    thoiDiemBatDauXacNhanRoi = 0;

    RFIDSensor::sleep();

    if (oledSanSang)
    {
        OLEDDisplay::sleep();
    }

    if (
        soLanBipConLai <= 0 &&
        !dangChoBipTiepTheo
    )
    {
        Buzzer::off();
    }

    if (
        ServoCua::dangMo() &&
        !dangMoCuaThuCong
    )
    {
        Serial.println();
        Serial.println("==============================");
        Serial.println("KHONG CON NGUOI");
        Serial.println("TU DONG DONG CUA");
        Serial.println("==============================");

        dongCuaVaGuiTrangThai();
    }

    Serial.println();
    Serial.println("==============================");
    Serial.println("TRANG THAI: CHO");
    Serial.println("OLED DA TAT");
    Serial.println("RC522 DA TAT ANTEN");
    Serial.println("CHO LM393 PHAT HIEN NGUOI");
    Serial.println("==============================");
}

void vaoTrangThaiChoQuetThe()
{
    trangThaiHienTai =
        TrangThaiHeThong::CHO_QUET_THE;

    thoiDiemBatDauTrangThai =
        millis();

    thoiDiemBatDauXacNhanRoi = 0;

    RFIDSensor::wake();

    if (oledSanSang)
    {
        OLEDDisplay::wake();
        OLEDDisplay::showWaiting(true);
    }

    Serial.println();
    Serial.println("==============================");
    Serial.println("TRANG THAI: CO NGUOI");
    Serial.println("DA BAT OLED");
    Serial.println("DA BAT RC522");
    Serial.println("DANG CHO QUET THE");
    Serial.println("==============================");

    CommunicationService::publishSensor(true);
}

void vaoTrangThaiDaDocThe(
    const String &uid
)
{
    trangThaiHienTai =
        TrangThaiHeThong::DA_DOC_THE;

    thoiDiemBatDauTrangThai =
        millis();

    thoiDiemBatDauXacNhanRoi = 0;

    Serial.println();
    Serial.println("==============================");
    Serial.println("TRANG THAI: DA QUET THE");
    Serial.print("UID: ");
    Serial.println(uid);
    Serial.println("DANG CHO QT KIEM TRA THE");
    Serial.println("==============================");

    if (oledSanSang)
    {
        OLEDDisplay::showCard(
            uid,
            ObstacleSensor::hasObstacle()
        );
    }

    CommunicationService::publishRFID(uid);
}

void vaoTrangThaiChoNguoiRoi()
{
    trangThaiHienTai =
        TrangThaiHeThong::CHO_NGUOI_ROI;

    thoiDiemBatDauTrangThai =
        millis();

    thoiDiemBatDauXacNhanRoi = 0;

    Serial.println();
    Serial.println(
        "TRANG THAI: CHO NGUOI ROI"
    );
}

void xuLyTrangThaiCho()
{
    if (ObstacleSensor::hasObstacle())
    {
        vaoTrangThaiChoQuetThe();
    }
}

void xuLyTrangThaiChoQuetThe()
{
    String uid;

    if (RFIDSensor::readCard(uid))
    {
        vaoTrangThaiDaDocThe(uid);
        return;
    }

    if (!ObstacleSensor::hasObstacle())
    {
        if (thoiDiemBatDauXacNhanRoi == 0)
        {
            thoiDiemBatDauXacNhanRoi =
                millis();

            Serial.println(
                "KHONG CON NGUOI, DANG XAC NHAN..."
            );
        }

        if (
            millis() -
                thoiDiemBatDauXacNhanRoi >=
            PERSON_CLEAR_CONFIRM_MS
        )
        {
            CommunicationService::publishSensor(false);
            vaoTrangThaiCho();
            return;
        }
    }
    else
    {
        thoiDiemBatDauXacNhanRoi = 0;
    }

    if (
        millis() - thoiDiemBatDauTrangThai >=
        CARD_WAIT_TIMEOUT_MS
    )
    {
        Serial.println(
            "HET THOI GIAN CHO QUET THE"
        );

        if (ObstacleSensor::hasObstacle())
        {
            thoiDiemBatDauTrangThai =
                millis();

            if (oledSanSang)
            {
                OLEDDisplay::showWaiting(true);
            }
        }
        else
        {
            CommunicationService::publishSensor(false);
            vaoTrangThaiCho();
        }
    }
}

void xuLyTrangThaiDaDocThe()
{
    if (
        millis() - thoiDiemBatDauTrangThai >=
        CARD_DISPLAY_TIME_MS
    )
    {
        vaoTrangThaiChoNguoiRoi();
    }
}

void xuLyTrangThaiChoNguoiRoi()
{
    const bool coVatCan =
        ObstacleSensor::hasObstacle();

    if (!coVatCan)
    {
        if (thoiDiemBatDauXacNhanRoi == 0)
        {
            thoiDiemBatDauXacNhanRoi =
                millis();

            Serial.println(
                "DANG XAC NHAN NGUOI DA ROI..."
            );
        }

        if (
            millis() -
                thoiDiemBatDauXacNhanRoi >=
            PERSON_CLEAR_CONFIRM_MS
        )
        {
            CommunicationService::publishSensor(false);
            vaoTrangThaiCho();
        }
    }
    else
    {
        thoiDiemBatDauXacNhanRoi = 0;
    }
}

}

void setup()
{
    Serial.begin(115200);
    delay(1000);

    Serial.println();
    Serial.println("==============================");
    Serial.println("HE THONG KIEM SOAT RA VAO");
    Serial.println("CHE DO TIET KIEM NANG LUONG");
    Serial.println("==============================");

    ObstacleSensor::begin();
    Buzzer::begin();
    ServoCua::begin();

    oledSanSang =
        OLEDDisplay::begin();

    if (oledSanSang)
    {
        OLEDDisplay::showStartup();
    }
    else
    {
        Serial.println(
            "OLED KHOI DONG THAT BAI"
        );
    }

    rfidSanSang =
        RFIDSensor::begin();

    if (!rfidSanSang)
    {
        Serial.println(
            "RC522 KHOI DONG THAT BAI"
        );

        if (oledSanSang)
        {
            OLEDDisplay::showError(
                "KHONG CO RC522"
            );
        }
    }

    CommunicationService::setDoorCommandCallback(
        xuLyLenhCua
    );

    CommunicationService::setAccessResultCallback(
        xuLyKetQuaThe
    );

    // Gan ham nhan thoi gian tu dong dong cua
    CommunicationService::setDoorTimeoutCallback(
        xuLyThoiGianDongCua
    );

    CommunicationService::begin();

    delay(2000);

    vaoTrangThaiCho();
}

void loop()
{
    CommunicationService::update();
    ObstacleSensor::update();
    Buzzer::update();

    xuLyChuoiBip();

    // Xu ly cua mo thu cong doc lap voi cam bien quet the
    xuLyDongCuaThuCongTuDong();

    if (!rfidSanSang)
    {
        delay(10);
        return;
    }

    switch (trangThaiHienTai)
    {
    case TrangThaiHeThong::CHO:
        xuLyTrangThaiCho();
        break;

    case TrangThaiHeThong::CHO_QUET_THE:
        xuLyTrangThaiChoQuetThe();
        break;

    case TrangThaiHeThong::DA_DOC_THE:
        xuLyTrangThaiDaDocThe();
        break;

    case TrangThaiHeThong::CHO_NGUOI_ROI:
        xuLyTrangThaiChoNguoiRoi();
        break;
    }

    delay(10);
}
