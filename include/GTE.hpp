#pragma once
#include <cstdint>
#include "Logger.hpp"

struct GTEVector3_16_t {
    int16_t x;
    int16_t y;
    int16_t z;
};

struct GTEMatrix_16_t {
    GTEVector3_16_t v0;
    GTEVector3_16_t v1;
    GTEVector3_16_t v2;
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

struct GTEColor_32_t {
    int32_t r;
    int32_t g;
    int32_t b;
};

struct GTEVector3_32_t {
    int32_t x;
    int32_t y;
    int32_t z;
};

struct GTEVector2_32_t {
    int32_t x;
    int32_t y;
};

/*
cop2r63 (cnt31) - FLAG - Returns any calculation errors.
0-11 Not used (always zero) (Read only)
12   IR0 saturated to +0000h..+1000h
13   SY2 saturated to -0400h..+03FFh
14   SX2 saturated to -0400h..+03FFh
15   MAC0 Result larger than 31 bits and negative
16   MAC0 Result larger than 31 bits and positive
17   Divide overflow. RTPS/RTPT division result saturated to max=1FFFFh
18   SZ3 or OTZ saturated to +0000h..+FFFFh
19   Color-FIFO-B saturated to +00h..+FFh
20   Color-FIFO-G saturated to +00h..+FFh
21   Color-FIFO-R saturated to +00h..+FFh
22   IR3 saturated to +0000h..+7FFFh (lm=1) or to -8000h..+7FFFh (lm=0)
23   IR2 saturated to +0000h..+7FFFh (lm=1) or to -8000h..+7FFFh (lm=0)
24   IR1 saturated to +0000h..+7FFFh (lm=1) or to -8000h..+7FFFh (lm=0)
25   MAC3 Result larger than 43 bits and negative
26   MAC2 Result larger than 43 bits and negative
27   MAC1 Result larger than 43 bits and negative
28   MAC3 Result larger than 43 bits and positive
29   MAC2 Result larger than 43 bits and positive
30   MAC1 Result larger than 43 bits and positive
31   Error Flag (Bit30..23, and 18..13 ORed together) (Read only)
*/
union GTEFLAGRegister {
    struct {
        uint32_t unused : 12;
        uint32_t ir0 : 1;
        uint32_t sy2 : 1;
        uint32_t sx2 : 1;
        uint32_t mac0Negative : 1;
        uint32_t mac0Positive : 1;
        uint32_t divideOverflow : 1;
        uint32_t sz3OrOtz : 1;
        uint32_t colorFifoB : 1;
        uint32_t colorFifoG : 1;
        uint32_t colorFifoR : 1;
        uint32_t ir3 : 1;
        uint32_t ir2 : 1;
        uint32_t ir1 : 1;
        uint32_t mac3Negative : 1;
        uint32_t mac2Negative : 1;
        uint32_t mac1Negative : 1;
        uint32_t mac3Positive : 1;
        uint32_t mac2Positive : 1;
        uint32_t mac1Positive : 1;
        uint32_t error : 1;
    };

    uint32_t _value;

    GTEFLAGRegister() : _value(0) {}
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

GTE Control Register Summary (cop2r32-63)
cop2r32-36 9xS16 RT11RT12,..,RT33 Rotation matrix     (3x3)        ;cnt0-4
cop2r37-39 3x 32 TRX,TRY,TRZ      Translation vector  (X,Y,Z)      ;cnt5-7
cop2r40-44 9xS16 L11L12,..,L33    Light source matrix (3x3)        ;cnt8-12
cop2r45-47 3x 32 RBK,GBK,BBK      Background color    (R,G,B)      ;cnt13-15
cop2r48-52 9xS16 LR1LR2,..,LB3    Light color matrix source (3x3)  ;cnt16-20
cop2r53-55 3x 32 RFC,GFC,BFC      Far color           (R,G,B)      ;cnt21-23
cop2r56-57 2x 32 OFX,OFY          Screen offset       (X,Y)        ;cnt24-25
cop2r58 BuggyU16 H                Projection plane distance.       ;cnt26
cop2r59      S16 DQA              Depth queing parameter A (coeff) ;cnt27
cop2r60       32 DQB              Depth queing parameter B (offset);cnt28
cop2r61-62 2xS16 ZSF3,ZSF4        Average Z scale factors          ;cnt29-30
cop2r63      U20 FLAG             Returns any calculation errors   ;cnt31
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

    GTEMatrix_16_t rt; // cop2r32-36
    GTEVector3_32_t tr; // cop2r37-39
    GTEMatrix_16_t l; // cop2r40-44
    GTEColor_32_t bk; // cop2r45-47
    GTEMatrix_16_t lr; // cop2r48-52
    GTEColor_32_t fc; // cop2r53-55
    GTEVector2_32_t of; // cop2r56-57
    uint16_t hl; // cop2r58
    int16_t dqa; // cop2r59
    int32_t dqb; // cop2r60
    int16_t zsf3; // cop2r61
    int16_t zsf4; // cop2r62
    GTEFLAGRegister flag; // cop2r63
public:
    GTE(LogLevel logLevel);
    ~GTE();

    void setData(uint32_t index, uint32_t value);
    uint32_t getData(uint32_t index);
    void setControl(uint32_t index, uint32_t value);
    void execute(uint32_t value);
};
