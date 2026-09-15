#include <unity.h>
#include <Arduino.h>
#include "ble_protocol.h"

// Ham tro giup mo phong logic phan tich frame BLE tren ESP32 (tu ble_service.cpp)
bool parseBleFrame(const String &frame, String &topic, String &payload)
{
    const int viTri = frame.indexOf('|');
    if (viTri <= 0 || viTri >= static_cast<int>(frame.length()) - 1)
    {
        return false;
    }

    topic = frame.substring(0, viTri);
    payload = frame.substring(viTri + 1);
    topic.trim();
    payload.trim();
    payload.toUpperCase();
    return true;
}

void test_ble_protocol_valid_open_frame(void)
{
    String topic, payload;
    bool ok = parseBleFrame("access/door/command|OPEN", topic, payload);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING(TOPIC_DOOR_COMMAND, topic.c_str());
    TEST_ASSERT_EQUAL_STRING("OPEN", payload.c_str());
}

void test_ble_protocol_valid_close_frame(void)
{
    String topic, payload;
    bool ok = parseBleFrame("access/door/command|CLOSE", topic, payload);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING(TOPIC_DOOR_COMMAND, topic.c_str());
    TEST_ASSERT_EQUAL_STRING("CLOSE", payload.c_str());
}

void test_ble_protocol_valid_granted_frame(void)
{
    String topic, payload;
    bool ok = parseBleFrame("access/result|GRANTED", topic, payload);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING(TOPIC_ACCESS_RESULT, topic.c_str());
    TEST_ASSERT_EQUAL_STRING("GRANTED", payload.c_str());
}

void test_ble_protocol_valid_denied_frame(void)
{
    String topic, payload;
    bool ok = parseBleFrame("access/result|DENIED", topic, payload);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING(TOPIC_ACCESS_RESULT, topic.c_str());
    TEST_ASSERT_EQUAL_STRING("DENIED", payload.c_str());
}

void test_ble_protocol_missing_separator(void)
{
    String topic, payload;
    bool ok = parseBleFrame("access/door/commandOPEN", topic, payload);
    TEST_ASSERT_FALSE(ok);
}

void test_ble_protocol_empty_topic(void)
{
    String topic, payload;
    bool ok = parseBleFrame("|OPEN", topic, payload);
    TEST_ASSERT_FALSE(ok);
}

void test_ble_protocol_empty_payload(void)
{
    String topic, payload;
    bool ok = parseBleFrame("access/door/command|", topic, payload);
    TEST_ASSERT_FALSE(ok);
}

void test_ble_protocol_valid_timeout_config(void)
{
    String topic, payload;
    bool ok = parseBleFrame("access/config/door_timeout|15", topic, payload);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING(TOPIC_DOOR_TIMEOUT_CONFIG, topic.c_str());
    TEST_ASSERT_EQUAL_STRING("15", payload.c_str());
}

void setup()
{
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_ble_protocol_valid_open_frame);
    RUN_TEST(test_ble_protocol_valid_close_frame);
    RUN_TEST(test_ble_protocol_valid_granted_frame);
    RUN_TEST(test_ble_protocol_valid_denied_frame);
    RUN_TEST(test_ble_protocol_missing_separator);
    RUN_TEST(test_ble_protocol_empty_topic);
    RUN_TEST(test_ble_protocol_empty_payload);
    RUN_TEST(test_ble_protocol_valid_timeout_config);
    UNITY_END();
}

void loop()
{
}
