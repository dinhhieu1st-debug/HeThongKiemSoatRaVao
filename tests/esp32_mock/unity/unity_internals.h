/* ==========================================
    Unity Project - A Test Framework for C
    Copyright (c) 2007-21 Mike Karlesky, Mark VanderVoord, Greg Williams
    [Released under MIT License. Please refer to license.txt for details]
========================================== */

#ifndef UNITY_INTERNALS_H
#define UNITY_INTERNALS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <setjmp.h>
#include <stdint.h>
#include <stddef.h>

#ifndef UNITY_INT_WIDTH
#define UNITY_INT_WIDTH 32
#endif

typedef void (*UnityTestFunction)(void);

typedef enum
{
    UNITY_DISPLAY_STYLE_INT = 0,
    UNITY_DISPLAY_STYLE_INT8,
    UNITY_DISPLAY_STYLE_INT16,
    UNITY_DISPLAY_STYLE_INT32,
    UNITY_DISPLAY_STYLE_UINT,
    UNITY_DISPLAY_STYLE_UINT8,
    UNITY_DISPLAY_STYLE_UINT16,
    UNITY_DISPLAY_STYLE_UINT32,
    UNITY_DISPLAY_STYLE_HEX8,
    UNITY_DISPLAY_STYLE_HEX16,
    UNITY_DISPLAY_STYLE_HEX32,
    UNITY_DISPLAY_STYLE_FLOAT,
    UNITY_DISPLAY_STYLE_UNKNOWN
} UNITY_DISPLAY_STYLE_T;

typedef enum
{
    UNITY_FLOAT_OR_DOUBLE_IS_EQUAL,
    UNITY_FLOAT_OR_DOUBLE_IS_NOT_EQUAL
} UNITY_FLOAT_TRAIT_T;

struct UNITY_STORAGE_T
{
    const char* TestFile;
    const char* CurrentTestName;
    uint32_t CurrentTestLineNumber;
    uint32_t NumberOfTests;
    uint32_t TestFailures;
    uint32_t TestIgnores;
    uint32_t CurrentTestFailed;
    uint32_t CurrentTestIgnored;
    jmp_buf AbortFrame;
};

extern struct UNITY_STORAGE_T Unity;

void UnityBegin(const char* filename);
int  UnityEnd(void);
void UnityConcludeTest(void);
void UnityDefaultTestRun(UnityTestFunction Func, const char* FuncName, const int FuncLineNum);

void UnityPrint(const char* string);
void UnityPrintNumberByStyle(const int32_t number, const UNITY_DISPLAY_STYLE_T style);
void UnityPrintNumberUnsigned(const uint32_t number);
void UnityPrintMask(const uint32_t mask, const uint32_t number);
void UnityPrintFloat(const double number);

void UnityAssertEqualNumber(const int32_t expected,
                            const int32_t actual,
                            const char* msg,
                            const uint32_t lineNumber,
                            const UNITY_DISPLAY_STYLE_T style);

void UnityAssertEqualIntArray(const int32_t* expected,
                              const int32_t* actual,
                              const uint32_t num_elements,
                              const char* msg,
                              const uint32_t lineNumber,
                              const UNITY_DISPLAY_STYLE_T style);

void UnityAssertEqualString(const char* expected,
                            const char* actual,
                            const char* msg,
                            const uint32_t lineNumber);

void UnityAssertEqualStringLen(const char* expected,
                               const char* actual,
                               const uint32_t length,
                               const char* msg,
                               const uint32_t lineNumber);

void UnityAssertEqualMemory(const void* expected,
                            const void* actual,
                            const uint32_t length,
                            const uint32_t num_elements,
                            const char* msg,
                            const uint32_t lineNumber);

void UnityAssertFloatsWithin(const float delta,
                             const float expected,
                             const float actual,
                             const char* msg,
                             const uint32_t lineNumber);

void UnityAssertDoublesWithin(const double delta,
                              const double expected,
                              const double actual,
                              const char* msg,
                              const uint32_t lineNumber);

void UnityFail(const char* msg, const uint32_t line);
void UnityIgnore(const char* msg, const uint32_t line);

#ifdef __cplusplus
}
#endif

#endif /* UNITY_INTERNALS_H */
