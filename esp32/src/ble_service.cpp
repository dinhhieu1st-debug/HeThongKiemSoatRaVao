#include "ble_service.h"
#include "ble_protocol.h"

#include <NimBLEDevice.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

namespace
{

const char *TEN_BLE = "HE THONG RA VAO HIEU";
const char *UUID_DICH_VU =
    "7d9a0001-6f71-4b88-9a5e-40cba20d1000";
const char *UUID_ESP32_GUI_PI =
    "7d9a0002-6f71-4b88-9a5e-40cba20d1000";
const char *UUID_PI_GUI_ESP32 =
    "7d9a0003-6f71-4b88-9a5e-40cba20d1000";

NimBLEServer *mayChu = nullptr;
NimBLECharacteristic *characteristicThongBao = nullptr;

EspBleService::HamXuLyLenhCua hamXuLyLenhCua = nullptr;
EspBleService::HamXuLyKetQuaThe hamXuLyKetQuaThe = nullptr;
EspBleService::HamXuLyThoiGianDongCua
    hamXuLyThoiGianDongCua = nullptr;

bool daKetNoi = false;
bool daBatNotify = false;
bool canBaoOnline = false;

struct FrameBle
{
    char duLieu[251];
};

QueueHandle_t hangDoiFrame = nullptr;

bool laChuoiSoHopLe(const String &noiDung)
{
    if (noiDung.length() == 0)
    {
        return false;
    }

    for (unsigned int i = 0; i < noiDung.length(); ++i)
    {
        if (!isDigit(noiDung[i]))
        {
            return false;
        }
    }

    return true;
}

void xuLyFrame(const String &frame)
{
    const int viTriPhanCach = frame.indexOf('|');

    if (
        viTriPhanCach <= 0 ||
        viTriPhanCach >= static_cast<int>(frame.length()) - 1
    )
    {
        Serial.println("Bo qua frame BLE khong hop le.");
        return;
    }

    String topic = frame.substring(0, viTriPhanCach);
    String noiDung = frame.substring(viTriPhanCach + 1);
    topic.trim();
    noiDung.trim();
    noiDung.toUpperCase();

    Serial.print("Nhan BLE [");
    Serial.print(topic);
    Serial.print("]: ");
    Serial.println(noiDung);

    if (topic == TOPIC_DOOR_COMMAND)
    {
        if (
            (noiDung == "OPEN" || noiDung == "CLOSE") &&
            hamXuLyLenhCua != nullptr
        )
        {
            hamXuLyLenhCua(noiDung);
        }
        return;
    }

    if (topic == TOPIC_ACCESS_RESULT)
    {
        if (
            (noiDung == "GRANTED" || noiDung == "DENIED") &&
            hamXuLyKetQuaThe != nullptr
        )
        {
            hamXuLyKetQuaThe(noiDung);
        }
        return;
    }

    if (topic == TOPIC_DOOR_TIMEOUT_CONFIG)
    {
        if (!laChuoiSoHopLe(noiDung))
        {
            return;
        }

        const unsigned long soGiay = noiDung.toInt();
        if (
            soGiay >= 1 &&
            soGiay <= 300 &&
            hamXuLyThoiGianDongCua != nullptr
        )
        {
            hamXuLyThoiGianDongCua(soGiay);
        }
    }
}

class CallbackMayChu : public NimBLEServerCallbacks
{
    void onConnect(
        NimBLEServer *,
        NimBLEConnInfo &
    ) override
    {
        daKetNoi = true;
        Serial.println("BLE da ket noi - BLE la kenh chinh.");
    }

    void onDisconnect(
        NimBLEServer *,
        NimBLEConnInfo &,
        int lyDo
    ) override
    {
        daKetNoi = false;
        daBatNotify = false;
        canBaoOnline = false;

        Serial.print("BLE da ngat, ly do: ");
        Serial.println(lyDo);
    }
};

class CallbackCharacteristic :
    public NimBLECharacteristicCallbacks
{
    void onWrite(
        NimBLECharacteristic *characteristic,
        NimBLEConnInfo &
    ) override
    {
        const std::string giaTri = characteristic->getValue();
        if (giaTri.empty() || giaTri.length() > 250)
        {
            return;
        }

        if (hangDoiFrame == nullptr)
        {
            return;
        }

        FrameBle frame{};
        memcpy(
            frame.duLieu,
            giaTri.data(),
            giaTri.length()
        );
        frame.duLieu[giaTri.length()] = '\0';
        xQueueSend(hangDoiFrame, &frame, 0);
    }

    void onSubscribe(
        NimBLECharacteristic *,
        NimBLEConnInfo &,
        uint16_t giaTriDangKy
    ) override
    {
        daBatNotify = (giaTriDangKy & 0x0001) != 0;
        if (daBatNotify)
        {
            canBaoOnline = true;
            Serial.println("Pi da subscribe BLE Notify.");
        }
    }
};

CallbackMayChu callbackMayChu;
CallbackCharacteristic callbackCharacteristic;

}

namespace EspBleService
{

void begin()
{
    hangDoiFrame = xQueueCreate(4, sizeof(FrameBle));

    if (hangDoiFrame == nullptr)
    {
        Serial.println("Khong tao duoc hang doi BLE.");
    }

    NimBLEDevice::init(TEN_BLE);
    NimBLEDevice::setMTU(185);

    // Giu cong suat mac dinh. Pi dang o gan nen P9 khong can thiet,
    // dong dinh cao con co the lam nguon cap chung voi servo kem on dinh.

    mayChu = NimBLEDevice::createServer();
    mayChu->setCallbacks(&callbackMayChu, false);
    mayChu->advertiseOnDisconnect(true);

    NimBLEService *dichVu =
        mayChu->createService(UUID_DICH_VU);

    characteristicThongBao = dichVu->createCharacteristic(
        UUID_ESP32_GUI_PI,
        NIMBLE_PROPERTY::NOTIFY
    );
    characteristicThongBao->setCallbacks(
        &callbackCharacteristic
    );

    NimBLECharacteristic *characteristicNhan =
        dichVu->createCharacteristic(
            UUID_PI_GUI_ESP32,
            NIMBLE_PROPERTY::WRITE |
                NIMBLE_PROPERTY::WRITE_NR
        );
    characteristicNhan->setCallbacks(
        &callbackCharacteristic
    );

    NimBLEAdvertising *quangBa =
        NimBLEDevice::getAdvertising();

    // Ten dai 21 ky tu duoc dat trong scan response.
    // Phai bat scan response truoc setName(), neu khong ten khong
    // con du cho trong goi advertising co UUID 128-bit.
    quangBa->enableScanResponse(true);
    quangBa->setMinInterval(160);
    quangBa->setMaxInterval(240);
    quangBa->addServiceUUID(UUID_DICH_VU);
    const bool daDatTen = quangBa->setName(TEN_BLE);
    const bool daBatQuangBa = quangBa->start();

    if (!daDatTen || !daBatQuangBa)
    {
        Serial.println("Khoi tao BLE advertising that bai.");
    }

    Serial.print("BLE advertising: ");
    Serial.println(TEN_BLE);
}

void update()
{
    FrameBle frame{};

    if (
        hangDoiFrame != nullptr &&
        xQueueReceive(hangDoiFrame, &frame, 0) == pdTRUE
    )
    {
        xuLyFrame(String(frame.duLieu));
    }

    if (canBaoOnline && daKetNoi && daBatNotify)
    {
        canBaoOnline = false;
        publish(TOPIC_STATUS, "ONLINE");
    }
}

void setDoorCommandCallback(HamXuLyLenhCua hamXuLy)
{
    hamXuLyLenhCua = hamXuLy;
}

void setAccessResultCallback(HamXuLyKetQuaThe hamXuLy)
{
    hamXuLyKetQuaThe = hamXuLy;
}

void setDoorTimeoutCallback(HamXuLyThoiGianDongCua hamXuLy)
{
    hamXuLyThoiGianDongCua = hamXuLy;
}

bool isConnected()
{
    return daKetNoi && daBatNotify;
}

bool publish(const String &topic, const String &payload)
{
    if (!isConnected() || characteristicThongBao == nullptr)
    {
        return false;
    }

    String frame = topic;
    frame += '|';
    frame += payload;

    if (frame.length() > 250)
    {
        return false;
    }

    characteristicThongBao->setValue(frame.c_str());
    const bool ketQua = characteristicThongBao->notify();

    Serial.print("Gui BLE [");
    Serial.print(topic);
    Serial.print("]: ");
    Serial.println(payload);

    return ketQua;
}

}
