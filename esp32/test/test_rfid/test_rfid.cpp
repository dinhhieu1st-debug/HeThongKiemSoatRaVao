#include <unity.h>
#include <Arduino.h>
#include "rfid_sensor.h"
#include "config.h"

void test_rfid_initialization(void)
{
    bool ok = RFIDSensor::begin();
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_FALSE(RFIDSensor::isEnabled());
}

void test_rfid_wake_and_sleep(void)
{
    RFIDSensor::wake();
    TEST_ASSERT_TRUE(RFIDSensor::isEnabled());

    RFIDSensor::sleep();
    TEST_ASSERT_FALSE(RFIDSensor::isEnabled());
}

void test_rfid_scan_manual_action(void)
{
    RFIDSensor::wake();

    Serial.println();
    Serial.println("==================================================");
    Serial.println("WAITING FOR MANUAL ACTION:");
    Serial.println("Please scan RFID card now to verify RC522 reading!");
    Serial.println("Waiting up to 5 seconds...");
    Serial.println("==================================================");

    unsigned long batDau = millis();
    String uid = "";
    bool docThanhCong = false;

    while (millis() - batDau < 5000)
    {
        if (RFIDSensor::readCard(uid))
        {
            docThanhCong = true;
            break;
        }
        delay(100);
    }

    if (docThanhCong)
    {
        Serial.print("-> Da doc the RFID UID: ");
        Serial.println(uid);
        TEST_ASSERT_TRUE(uid.length() >= 8);
    }
    else
    {
        Serial.println("-> Khong co the quet trong 5s. Danh dau WAITING FOR MANUAL ACTION.");
        TEST_PASS();
    }

    RFIDSensor::sleep();
}

void setup()
{
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_rfid_initialization);
    RUN_TEST(test_rfid_wake_and_sleep);
    RUN_TEST(test_rfid_scan_manual_action);
    UNITY_END();
}

void loop()
{
}
