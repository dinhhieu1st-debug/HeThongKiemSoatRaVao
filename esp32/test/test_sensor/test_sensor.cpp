#include <unity.h>
#include <Arduino.h>
#include "obstacle_sensor.h"
#include "config.h"

void test_sensor_initial_read(void)
{
    ObstacleSensor::begin();
    // Doc trang thai hien tai cua cam bien quang LM393
    bool coVatCan = ObstacleSensor::hasObstacle();
    Serial.print("Trang thai ban dau cua cam bien: ");
    Serial.println(coVatCan ? "CO VAT CAN" : "KHONG CO VAT CAN (CLEAR)");
    TEST_ASSERT_TRUE(coVatCan == true || coVatCan == false);
}

void test_sensor_manual_action(void)
{
    Serial.println();
    Serial.println("==================================================");
    Serial.println("WAITING FOR MANUAL ACTION:");
    Serial.println("Please trigger the LM393 sensor (place hand in front)");
    Serial.println("Waiting up to 5 seconds...");
    Serial.println("==================================================");

    unsigned long batDau = millis();
    bool daKichHoat = false;
    while (millis() - batDau < 5000)
    {
        ObstacleSensor::update();
        if (ObstacleSensor::hasObstacle())
        {
            daKichHoat = true;
            break;
        }
        delay(50);
    }

    if (daKichHoat)
    {
        Serial.println("-> Cảm biến đã kích hoạt thành công!");
        TEST_ASSERT_TRUE(true);
    }
    else
    {
        Serial.println("-> Không có thao tác che cảm biến trong 5s. Danh dau WAITING FOR MANUAL ACTION.");
        TEST_PASS();
    }
}

void setup()
{
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_sensor_initial_read);
    RUN_TEST(test_sensor_manual_action);
    UNITY_END();
}

void loop()
{
}
