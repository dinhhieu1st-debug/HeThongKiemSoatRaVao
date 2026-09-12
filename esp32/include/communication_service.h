#ifndef COMMUNICATION_SERVICE_H
#define COMMUNICATION_SERVICE_H

#include <Arduino.h>

namespace CommunicationService
{

using HamXuLyLenhCua = void (*)(const String &lenh);
using HamXuLyKetQuaThe = void (*)(const String &ketQua);
using HamXuLyThoiGianDongCua = void (*)(unsigned long soGiay);

void begin();
void update();

void setDoorCommandCallback(HamXuLyLenhCua hamXuLy);
void setAccessResultCallback(HamXuLyKetQuaThe hamXuLy);
void setDoorTimeoutCallback(HamXuLyThoiGianDongCua hamXuLy);

bool isBleConnected();

bool publishRFID(const String &uid);
bool publishSensor(bool coVatCan);
bool publishStatus(const String &trangThai);
bool publishDoorStatus(const String &trangThai);

}

#endif
