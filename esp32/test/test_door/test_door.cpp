#include <unity.h>
#include <Arduino.h>
#include "servo_cua.h"
#include "config.h"

void test_door_initial_state(void)
{
    // Cua ban dau sau khi khoi tao phai o trang thai dong
    ServoCua::begin();
    TEST_ASSERT_FALSE(ServoCua::dangMo());
}

void test_door_open_command(void)
{
    ServoCua::moCua();
    TEST_ASSERT_TRUE(ServoCua::dangMo());
}

void test_door_close_command(void)
{
    ServoCua::dongCua();
    TEST_ASSERT_FALSE(ServoCua::dangMo());
}

void test_door_toggle_sequence(void)
{
    // Kiem tra chu trinh dong mo lien tuc
    ServoCua::moCua();
    TEST_ASSERT_TRUE(ServoCua::dangMo());
    delay(500);

    ServoCua::dongCua();
    TEST_ASSERT_FALSE(ServoCua::dangMo());
    delay(500);

    ServoCua::moCua();
    TEST_ASSERT_TRUE(ServoCua::dangMo());
    delay(500);

    ServoCua::dongCua();
    TEST_ASSERT_FALSE(ServoCua::dangMo());
}

void setup()
{
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_door_initial_state);
    RUN_TEST(test_door_open_command);
    RUN_TEST(test_door_close_command);
    RUN_TEST(test_door_toggle_sequence);
    UNITY_END();
}

void loop()
{
}
