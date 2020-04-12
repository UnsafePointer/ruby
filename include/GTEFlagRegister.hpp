#pragma once
#include <cstdint>

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
union GTEFlagRegister {
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

    GTEFlagRegister() : _value(0) {};

    void setMACNegative(unsigned int index);
    void setMACPositive(unsigned int index);
    void setIR(unsigned int index);
    int64_t calculateMAC(unsigned int index, int64_t value);
    int32_t calculateMAC0(int64_t value);
    int16_t calculateIR(unsigned int index, int64_t value, bool lm);
    uint16_t calculateSZ3(int64_t value);
};
