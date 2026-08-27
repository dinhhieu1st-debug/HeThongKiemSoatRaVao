/* ==========================================
    Unity Project - A Test Framework for C
    Copyright (c) 2007-21 Mike Karlesky, Mark VanderVoord, Greg Williams
    [Released under MIT License. Please refer to license.txt for details]
========================================== */

#include "unity.h"
#include <string.h>
#include <math.h>

struct UNITY_STORAGE_T Unity;

static const char UnityStrOk[] = "PASS";
static const char UnityStrFail[] = "FAIL";
static const char UnityStrIgnore[] = "IGNORE";

void UnityPrint(const char* string)
{
    if (string)
    {
        printf("%s", string);
    }
}

void UnityPrintNumberByStyle(const int32_t number, const UNITY_DISPLAY_STYLE_T style)
{
    switch (style)
    {
        case UNITY_DISPLAY_STYLE_HEX8:
            printf("0x%02X", (uint8_t)number);
            break;
        case UNITY_DISPLAY_STYLE_HEX16:
            printf("0x%04X", (uint16_t)number);
            break;
        case UNITY_DISPLAY_STYLE_HEX32:
            printf("0x%08X", (uint32_t)number);
            break;
        case UNITY_DISPLAY_STYLE_UINT:
        case UNITY_DISPLAY_STYLE_UINT8:
        case UNITY_DISPLAY_STYLE_UINT16:
        case UNITY_DISPLAY_STYLE_UINT32:
            printf("%u", (uint32_t)number);
            break;
        default:
            printf("%d", (int32_t)number);
            break;
    }
}

void UnityPrintFloat(const double number)
{
    printf("%.4f", number);
}

void UnityBegin(const char* filename)
{
    Unity.TestFile = filename;
    Unity.CurrentTestName = NULL;
    Unity.CurrentTestLineNumber = 0;
    Unity.NumberOfTests = 0;
    Unity.TestFailures = 0;
    Unity.TestIgnores = 0;
    Unity.CurrentTestFailed = 0;
    Unity.CurrentTestIgnored = 0;

    UnityPrint("\n--------------------------------------------------\n");
    UnityPrint("UNITY TEST EXECUTION: ");
    UnityPrint(filename);
    UnityPrint("\n--------------------------------------------------\n");
}

int UnityEnd(void)
{
    UnityPrint("--------------------------------------------------\n");
    printf("%u Tests %u Failures %u Ignored\n",
           Unity.NumberOfTests, Unity.TestFailures, Unity.TestIgnores);
    if (Unity.TestFailures == 0U)
    {
        UnityPrint("OK - ALL TESTS PASSED!\n");
    }
    else
    {
        UnityPrint("FAILED!\n");
    }
    UnityPrint("--------------------------------------------------\n\n");
    return (int)(Unity.TestFailures);
}

void UnityConcludeTest(void)
{
    if (Unity.CurrentTestIgnored)
    {
        Unity.TestIgnores++;
    }
    else if (Unity.CurrentTestFailed)
    {
        Unity.TestFailures++;
    }
    else
    {
        printf("%s:%u:%s:%s\n", Unity.TestFile, Unity.CurrentTestLineNumber, Unity.CurrentTestName, UnityStrOk);
    }
}

void UnityDefaultTestRun(UnityTestFunction Func, const char* FuncName, const int FuncLineNum)
{
    Unity.CurrentTestName = FuncName;
    Unity.CurrentTestLineNumber = (uint32_t)FuncLineNum;
    Unity.NumberOfTests++;
    Unity.CurrentTestFailed = 0;
    Unity.CurrentTestIgnored = 0;

    if (setjmp(Unity.AbortFrame) == 0)
    {
        setUp();
        Func();
    }
    tearDown();
    UnityConcludeTest();
}

void UnityFail(const char* msg, const uint32_t line)
{
    Unity.CurrentTestFailed = 1;
    printf("%s:%u:%s:%s: %s\n", Unity.TestFile, line, Unity.CurrentTestName, UnityStrFail, msg ? msg : "");
    longjmp(Unity.AbortFrame, 1);
}

void UnityIgnore(const char* msg, const uint32_t line)
{
    Unity.CurrentTestIgnored = 1;
    printf("%s:%u:%s:%s: %s\n", Unity.TestFile, line, Unity.CurrentTestName, UnityStrIgnore, msg ? msg : "");
    longjmp(Unity.AbortFrame, 1);
}

void UnityAssertEqualNumber(const int32_t expected,
                            const int32_t actual,
                            const char* msg,
                            const uint32_t lineNumber,
                            const UNITY_DISPLAY_STYLE_T style)
{
    if (expected != actual)
    {
        Unity.CurrentTestFailed = 1;
        printf("%s:%u:%s:%s: Expected ", Unity.TestFile, lineNumber, Unity.CurrentTestName, UnityStrFail);
        UnityPrintNumberByStyle(expected, style);
        UnityPrint(" Was ");
        UnityPrintNumberByStyle(actual, style);
        if (msg)
        {
            printf(" (%s)", msg);
        }
        UnityPrint("\n");
        longjmp(Unity.AbortFrame, 1);
    }
}

void UnityAssertEqualString(const char* expected,
                            const char* actual,
                            const char* msg,
                            const uint32_t lineNumber)
{
    if (expected == actual) return;
    if (expected == NULL || actual == NULL || strcmp(expected, actual) != 0)
    {
        Unity.CurrentTestFailed = 1;
        printf("%s:%u:%s:%s: Expected '%s' Was '%s'",
               Unity.TestFile, lineNumber, Unity.CurrentTestName, UnityStrFail,
               expected ? expected : "NULL",
               actual ? actual : "NULL");
        if (msg)
        {
            printf(" (%s)", msg);
        }
        UnityPrint("\n");
        longjmp(Unity.AbortFrame, 1);
    }
}

void UnityAssertFloatsWithin(const float delta,
                             const float expected,
                             const float actual,
                             const char* msg,
                             const uint32_t lineNumber)
{
    float diff = actual - expected;
    if (diff < 0.0f) diff = -diff;
    if (diff > delta || isnan(actual) || isnan(expected))
    {
        Unity.CurrentTestFailed = 1;
        printf("%s:%u:%s:%s: Expected %.4f (+/- %.4f) Was %.4f",
               Unity.TestFile, lineNumber, Unity.CurrentTestName, UnityStrFail,
               expected, delta, actual);
        if (msg)
        {
            printf(" (%s)", msg);
        }
        UnityPrint("\n");
        longjmp(Unity.AbortFrame, 1);
    }
}

void UnityAssertEqualMemory(const void* expected,
                            const void* actual,
                            const uint32_t length,
                            const uint32_t num_elements,
                            const char* msg,
                            const uint32_t lineNumber)
{
    uint32_t total = length * num_elements;
    if (expected == actual) return;
    if (expected == NULL || actual == NULL || memcmp(expected, actual, total) != 0)
    {
        Unity.CurrentTestFailed = 1;
        printf("%s:%u:%s:%s: Memory mismatch (%u bytes)",
               Unity.TestFile, lineNumber, Unity.CurrentTestName, UnityStrFail, total);
        if (msg)
        {
            printf(" (%s)", msg);
        }
        UnityPrint("\n");
        longjmp(Unity.AbortFrame, 1);
    }
}
