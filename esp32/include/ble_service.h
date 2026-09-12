#ifndef BLE_SERVICE_H
#define BLE_SERVICE_H

#include <Arduino.h>

namespace EspBleService
{

using HamXuLyLenhCua =
    void (*)(const String &lenh);

using HamXuLyKetQuaThe =
    void (*)(const String &ketQua);

using HamXuLyThoiGianDongCua =
    void (*)(unsigned long soGiay);

void begin();
void update();

void setDoorCommandCallback(HamXuLyLenhCua hamXuLy);
void setAccessResultCallback(HamXuLyKetQuaThe hamXuLy);
void setDoorTimeoutCallback(HamXuLyThoiGianDongCua hamXuLy);

bool isConnected();
bool publish(const String &topic, const String &payload);

}

#endif
