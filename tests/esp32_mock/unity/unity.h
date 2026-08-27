/* ==========================================
    Unity Project - A Test Framework for C
    Copyright (c) 2007-21 Mike Karlesky, Mark VanderVoord, Greg Williams
    [Released under MIT License. Please refer to license.txt for details]
========================================== */

#ifndef UNITY_FRAMEWORK_H
#define UNITY_FRAMEWORK_H

#include "unity_internals.h"

#ifdef __cplusplus
extern "C"
{
#endif

void setUp(void);
void tearDown(void);

#define UNITY_BEGIN() UnityBegin(__FILE__)
#define UNITY_END()   UnityEnd()

#define RUN_TEST(TestFunc) \
    UnityDefaultTestRun(TestFunc, #TestFunc, __LINE__)

/* Basic assertions */
#define TEST_ASSERT(condition) \
    do { \
        if (!(condition)) { \
            UnityAssertEqualNumber(1, 0, "Expression evaluated to false", __LINE__, UNITY_DISPLAY_STYLE_INT); \
        } \
    } while (0)

#define TEST_ASSERT_TRUE(condition)   TEST_ASSERT(condition)
#define TEST_ASSERT_FALSE(condition)  TEST_ASSERT(!(condition))
#define TEST_ASSERT_NULL(pointer)     TEST_ASSERT((pointer) == NULL)
#define TEST_ASSERT_NOT_NULL(pointer) TEST_ASSERT((pointer) != NULL)

#define TEST_ASSERT_EQUAL_INT(expected, actual) \
    UnityAssertEqualNumber((int32_t)(expected), (int32_t)(actual), NULL, __LINE__, UNITY_DISPLAY_STYLE_INT)

#define TEST_ASSERT_EQUAL_UINT(expected, actual) \
    UnityAssertEqualNumber((int32_t)(expected), (int32_t)(actual), NULL, __LINE__, UNITY_DISPLAY_STYLE_UINT)

#define TEST_ASSERT_EQUAL_HEX(expected, actual) \
    UnityAssertEqualNumber((int32_t)(expected), (int32_t)(actual), NULL, __LINE__, UNITY_DISPLAY_STYLE_HEX32)

#define TEST_ASSERT_EQUAL_FLOAT(expected, actual) \
    UnityAssertFloatsWithin(0.0001f, (float)(expected), (float)(actual), NULL, __LINE__)

#define TEST_ASSERT_FLOAT_WITHIN(delta, expected, actual) \
    UnityAssertFloatsWithin((float)(delta), (float)(expected), (float)(actual), NULL, __LINE__)

#define TEST_ASSERT_EQUAL_STRING(expected, actual) \
    UnityAssertEqualString((expected), (actual), NULL, __LINE__)

#define TEST_ASSERT_EQUAL_MEMORY(expected, actual, len) \
    UnityAssertEqualMemory((expected), (actual), (len), 1, NULL, __LINE__)

#define TEST_FAIL() \
    UnityFail("Test Failed", __LINE__)

#define TEST_FAIL_MESSAGE(message) \
    UnityFail((message), __LINE__)

#define TEST_IGNORE() \
    UnityIgnore("Test Ignored", __LINE__)

#define TEST_IGNORE_MESSAGE(message) \
    UnityIgnore((message), __LINE__)

#ifdef __cplusplus
}
#endif

#endif /* UNITY_FRAMEWORK_H */
