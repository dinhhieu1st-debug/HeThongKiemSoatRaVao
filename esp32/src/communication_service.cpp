#include "communication_service.h"

#include "ble_service.h"
#include "ble_protocol.h"

namespace CommunicationService
{

void begin()
{
    EspBleService::begin();
}

void update()
{
    EspBleService::update();
}

void setDoorCommandCallback(HamXuLyLenhCua hamXuLy)
{
    EspBleService::setDoorCommandCallback(hamXuLy);
}

void setAccessResultCallback(HamXuLyKetQuaThe hamXuLy)
{
    EspBleService::setAccessResultCallback(hamXuLy);
}

void setDoorTimeoutCallback(HamXuLyThoiGianDongCua hamXuLy)
{
    EspBleService::setDoorTimeoutCallback(hamXuLy);
}

bool isBleConnected()
{
    return EspBleService::isConnected();
}

bool publishRFID(const String &uid)
{
    return EspBleService::publish(TOPIC_RFID, uid);
}

bool publishSensor(bool coVatCan)
{
    const String noiDung = coVatCan ? "OBSTACLE" : "CLEAR";

    return EspBleService::publish(TOPIC_SENSOR, noiDung);
}

bool publishStatus(const String &trangThai)
{
    return EspBleService::publish(
        TOPIC_STATUS,
        trangThai
    );
}

bool publishDoorStatus(const String &trangThai)
{
    return EspBleService::publish(
        TOPIC_DOOR_STATUS,
        trangThai
    );
}

}
