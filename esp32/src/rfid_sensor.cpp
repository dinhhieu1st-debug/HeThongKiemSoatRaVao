#include "rfid_sensor.h"
#include "config.h"

#include <SPI.h>
#include <MFRC522.h>

namespace {

MFRC522 rfid(RFID_SS_PIN, RFID_RST_PIN);

bool enabled = false;

String uidToString(const MFRC522::Uid &uid) {
    String result = "";

    for (byte i = 0; i < uid.size; i++) {
        if (uid.uidByte[i] < 0x10) {
            result += "0";
        }

        result += String(uid.uidByte[i], HEX);
    }

    result.toUpperCase();
    return result;
}

}

namespace RFIDSensor {

bool begin() {
    SPI.begin(
        RFID_SCK_PIN,
        RFID_MISO_PIN,
        RFID_MOSI_PIN,
        RFID_SS_PIN
    );

    rfid.PCD_Init();
    delay(100);

    byte version = rfid.PCD_ReadRegister(
        MFRC522::VersionReg
    );

    Serial.print("RC522 version: 0x");
    Serial.println(version, HEX);

    if (version == 0x00 || version == 0xFF) {
        Serial.println("Khong tim thay RC522!");
        return false;
    }

    // Táº¯t anten RFID ngay sau khi khá»Ÿi Ä‘á»™ng
    rfid.PCD_AntennaOff();
    enabled = false;

    Serial.println("RC522 san sang.");
    Serial.println("RC522 dang o che do nghi.");

    return true;
}

void wake() {
    if (enabled) {
        return;
    }

    rfid.PCD_AntennaOn();
    delay(20);

    enabled = true;

    Serial.println("Da kich hoat RC522.");
}

void sleep() {
    if (!enabled) {
        return;
    }

    rfid.PCD_AntennaOff();
    enabled = false;

    Serial.println("Da tat RC522.");
}

bool isEnabled() {
    return enabled;
}

bool readCard(String &uid) {
    // KhÃ´ng Ä‘á»c tháº» khi RFID Ä‘ang táº¯t
    if (!enabled) {
        return false;
    }

    if (!rfid.PICC_IsNewCardPresent()) {
        return false;
    }

    if (!rfid.PICC_ReadCardSerial()) {
        return false;
    }

    uid = uidToString(rfid.uid);

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();

    return true;
}

}
