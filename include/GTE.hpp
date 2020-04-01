#pragma once
#include <cstdint>
#include "Logger.hpp"

struct GTEVector3_16_t {
    int16_t x;
    int16_t y;
    int16_t z;
};

union GTEColor_u8_t {
    struct {
        uint8_t r : 8;
        uint8_t g : 8;
        uint8_t b : 8;
        uint8_t c : 8;
    };
    uint32_t _value;
    GTEColor_u8_t() : _value(0) {}
};

struct GTEVector2_16_t {
    int16_t x;
    int16_t y;
};

/*
GTE Data Register Summary (cop2r0-31)
cop2r0-1   3xS16 VXY0,VZ0              Vector 0 (X,Y,Z)
cop2r2-3   3xS16 VXY1,VZ1              Vector 1 (X,Y,Z)
cop2r4-5   3xS16 VXY2,VZ2              Vector 2 (X,Y,Z)
cop2r6     4xU8  RGBC                  Color/code value
cop2r7     1xU16 OTZ                   Average Z value (for Ordering Table)
cop2r8     1xS16 IR0                   16bit Accumulator (Interpolate)
cop2r9-11  3xS16 IR1,IR2,IR3           16bit Accumulator (Vector)
cop2r12-15 6xS16 SXY0,SXY1,SXY2,SXYP   Screen XY-coordinate FIFO  (3 stages)
cop2r16-19 4xU16 SZ0,SZ1,SZ2,SZ3       Screen Z-coordinate FIFO   (4 stages)
cop2r20-22 12xU8 RGB0,RGB1,RGB2        Color CRGB-code/color FIFO (3 stages)
cop2r23    4xU8  (RES1)                Prohibited
cop2r24    1xS32 MAC0                  32bit Maths Accumulators (Value)
cop2r25-27 3xS32 MAC1,MAC2,MAC3        32bit Maths Accumulators (Vector)
cop2r28-29 1xU15 IRGB,ORGB             Convert RGB Color (48bit vs 15bit)
cop2r30-31 2xS32 LZCS,LZCR             Count Leading-Zeroes/Ones (sign bits)
*/
class GTE {
    Logger logger;
    GTEVector3_16_t v0; // cop2r0-1
    GTEVector3_16_t v1; // cop2r2-3
    GTEVector3_16_t v2; // cop2r4-5
    GTEColor_u8_t rgbc; // cop2r6
    uint16_t otz; // cop2r7
    int16_t ir0; // cop2r8
    int16_t ir1; // cop2r9
    int16_t ir2; // cop2r10
    int16_t ir3; // cop2r11
    GTEVector2_16_t sxy0; // cop2r12
    GTEVector2_16_t sxy1; // cop2r13
    GTEVector2_16_t sxy2; // cop2r14
    GTEVector2_16_t sxyp; // cop2r15
    uint16_t sz0; // cop2r18
    uint16_t sz1; // cop2r17
    uint16_t sz2; // cop2r18
    uint16_t sz3; // cop2r19
    GTEColor_u8_t rgb0; // cop2r20
    GTEColor_u8_t rgb1; // cop2r21
    GTEColor_u8_t rgb2; // cop2r22
    uint32_t res1; // cop2r23
    int32_t mac0; // cop2r24
    int32_t mac1; // cop2r25
    int32_t mac2; // cop2r26
    int32_t mac3; // cop2r27
    uint16_t irgb; // cop2r28
    uint16_t orgb; // cop2r29
    int32_t lzcs; // cop2r30
    int32_t lzcr; // cop2r31
public:
    GTE(LogLevel logLevel);
    ~GTE();

    void execute(uint32_t value);
};
