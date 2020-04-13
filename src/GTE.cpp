#include "GTE.hpp"
#include "Helpers.hpp"

GTE::GTE(LogLevel logLevel) : logger(logLevel, "  GTE: "),
    v0({0, 0, 0}),
    v1({0, 0, 0}),
    v2({0, 0, 0}),
    rgbc(GTEColor_u8_t()),
    otz(0),
    ir0(0),
    ir1(0),
    ir2(0),
    ir3(0),
    sxy0({0, 0}),
    sxy1({0, 0}),
    sxy2({0, 0}),
    sxyp({0, 0}),
    sz0(0),
    sz1(0),
    sz2(0),
    sz3(0),
    rgb0(GTEColor_u8_t()),
    rgb1(GTEColor_u8_t()),
    rgb2(GTEColor_u8_t()),
    res1(0),
    mac0(0),
    mac1(0),
    mac2(0),
    mac3(0),
    irgb(0),
    orgb(0),
    lzcs(0),
    lzcr(0),
    rt({{0, 0, 0}, {0, 0, 0}, {0, 0, 0}}),
    tr({0, 0, 0}),
    l({{0, 0, 0}, {0, 0, 0}, {0, 0, 0}}),
    bk({0, 0, 0}),
    lr({{0, 0, 0}, {0, 0, 0}, {0, 0, 0}}),
    fc({0, 0, 0}),
    of({0, 0}),
    hl(0),
    dqa(0),
    dqb(0),
    zsf3(0),
    zsf4(0),
    flag(GTEFlagRegister())
{
}

GTE::~GTE() {}

void GTE::setData(uint32_t index, uint32_t value) {
    logger.logMessage("DATA [W] (IDX: %d): %#x", index, value);
    switch (index) {
        case 0: {
            v0.x = value & 0xFFFF;
            v0.y = (value >> 16) & 0xFFFF;
            break;
        }
        case 1: {
            v0.z = value & 0xFFFF;
            break;
        }
        case 2: {
            v1.x = value & 0xFFFF;
            v1.y = (value >> 16) & 0xFFFF;
            break;
        }
        case 3: {
            v1.z = value & 0xFFFF;
            break;
        }
        case 4: {
            v2.x = value & 0xFFFF;
            v2.y = (value >> 16) & 0xFFFF;
            break;
        }
        case 5: {
            v2.z = value & 0xFFFF;
            break;
        }
        case 6: {
            rgbc._value = value;
            break;
        }
        case 7: {
            otz = value & 0xFFFF;
            break;
        }
        case 8: {
            ir0 = (value & 0xFFFF);
            break;
        }
        case 9: {
            ir1 = (value & 0xFFFF);
            break;
        }
        case 10: {
            ir2 = (value & 0xFFFF);
            break;
        }
        case 11: {
            ir3 = (value & 0xFFFF);
            break;
        }
        case 12: {
            sxy0.x = (value & 0xFFFF);
            sxy0.y = (value >> 16) & 0xFFFF;
            break;
        }
        case 13: {
            sxy1.x = (value & 0xFFFF);
            sxy1.y = (value >> 16) & 0xFFFF;
            break;
        }
        case 14: {
            sxy2.x = (value & 0xFFFF);
            sxy2.y = (value >> 16) & 0xFFFF;
            break;
        }
        case 15: {
            sxy0 = sxy1;
            sxy1 = sxy2;
            sxy2.x = (value & 0xFFFF);
            sxy2.y = (value >> 16) & 0xFFFF;
            break;
        }
        case 16: {
            sz0 = (value & 0xFFFF);
            break;
        }
        case 17: {
            sz1 = (value & 0xFFFF);
            break;
        }
        case 18: {
            sz2 = (value & 0xFFFF);
            break;
        }
        case 19: {
            sz3 = (value & 0xFFFF);
            break;
        }
        case 20: {
            rgb0._value = value;
            break;
        }
        case 21: {
            rgb1._value = value;
            break;
        }
        case 22: {
            rgb2._value = value;
            break;
        }
        case 23: {
            res1 = value;
            break;
        }
        case 24: {
            mac0 = value;
            break;
        }
        case 25: {
            mac1 = value;
            break;
        }
        case 26: {
            mac2 = value;
            break;
        }
        case 27: {
            mac3 = value;
            break;
        }
        case 28: {
            irgb = (value & 0x7FFF);
            ir1 = (value & 0x1F) * 0x80;
            ir2 = ((value >> 5) & 0x1F) * 0x80;
            ir3 = ((value >> 10) & 0x1F) * 0x80;
            break;
        }
        case 29: {
            break;
        }
        case 30: {
            lzcs = value;
            lzcr = countLeadingZeroes(value);
            break;
        }
        case 31: {
            break;
        }
        default: {
            logger.logError("Unhandled DATA write at index: %d with value: %#x", index, value);
            break;
        }
    }
}

uint32_t GTE::getData(uint32_t index) {
    uint32_t value = 0;
    switch (index) {
        case 0: {
            value = ((uint16_t)v0.y << 16) | (uint16_t)v0.x;
            break;
        }
        case 1: {
            value = v0.z;
            break;
        }
        case 2: {
            value = ((uint16_t)v1.y << 16) | (uint16_t)v1.x;
            break;
        }
        case 3: {
            value = v1.z;
            break;
        }
        case 4: {
            value = ((uint16_t)v2.y << 16) | (uint16_t)v2.x;
            break;
        }
        case 5: {
            value = v2.z;
            break;
        }
        case 6: {
            value = rgbc._value;
            break;
        }
        case 7: {
            value = otz;
            break;
        }
        case 8: {
            value = ir0;
            break;
        }
        case 9: {
            value = ir1;
            break;
        }
        case 10: {
            value = ir2;
            break;
        }
        case 11: {
            value = ir3;
            break;
        }
        case 12: {
            value = ((uint16_t)sxy0.y << 16) | (uint16_t)sxy0.x;
            break;
        }
        case 13: {
            value = ((uint16_t)sxy1.y << 16) | (uint16_t)sxy1.x;
            break;
        }
        case 14:
        case 15: {
            value = ((uint16_t)sxy2.y << 16) | (uint16_t)sxy2.x;
            break;
        }
        case 16: {
            value = sz0;
            break;
        }
        case 17: {
            value = sz1;
            break;
        }
        case 18: {
            value = sz2;
            break;
        }
        case 19: {
            value = sz3;
            break;
        }
        case 20: {
            value = rgb0._value;
            break;
        }
        case 21: {
            value = rgb1._value;
            break;
        }
        case 22: {
            value = rgb2._value;
            break;
        }
        case 23: {
            value = res1;
            break;
        }
        case 24: {
            value = mac0;
            break;
        }
        case 25: {
            value = mac1;
            break;
        }
        case 26: {
            value = mac2;
            break;
        }
        case 27: {
            value = mac3;
            break;
        }
        case 28:
        case 29: {
            uint16_t ir1Saturated = saturate(ir1 / 0x80);
            uint16_t ir2Saturated = saturate(ir2 / 0x80);
            uint16_t ir3Saturated = saturate(ir3 / 0x80);
            irgb = ir3Saturated << 10 | ir2Saturated << 5 | ir1Saturated;
            value = irgb;
            break;
        }
        case 30: {
            value = lzcs;
            break;
        }
        case 31: {
            value = lzcr;
            break;
        }
        default: {
            logger.logError("Unhandled DATA read at index: %d", index);
            break;
        }
    }
    logger.logMessage("DATA [R] (IDX: %d): %#x", index, value);
    return value;
}

void GTE::setControl(uint32_t index, uint32_t value) {
    logger.logMessage("CONTROL [W] (IDX: %d): %#x", index, value);
    switch (index) {
        case 0: {
            rt.v0.x = value & 0xFFFF;
            rt.v0.y = (value >> 16) & 0xFFFF;
            break;
        }
        case 1: {
            rt.v0.z = value & 0xFFFF;
            rt.v1.x = (value >> 16) & 0xFFFF;
            break;
        }
        case 2: {
            rt.v1.y = value & 0xFFFF;
            rt.v1.z = (value >> 16) & 0xFFFF;
            break;
        }
        case 3: {
            rt.v2.x = value & 0xFFFF;
            rt.v2.y = (value >> 16) & 0xFFFF;
            break;
        }
        case 4: {
            rt.v2.z = value & 0xFFFF;
            break;
        }
        case 5: {
            tr.x = value;
            break;
        }
        case 6: {
            tr.y = value;
            break;
        }
        case 7: {
            tr.z = value;
            break;
        }
        case 8: {
            l.v0.x = value & 0xFFFF;
            l.v0.y = (value >> 16) & 0xFFFF;
            break;
        }
        case 9: {
            l.v0.z = value & 0xFFFF;
            l.v1.x = (value >> 16) & 0xFFFF;
            break;
        }
        case 10: {
            l.v1.y = value & 0xFFFF;
            l.v1.z = (value >> 16) & 0xFFFF;
            break;
        }
        case 11: {
            l.v2.x = value & 0xFFFF;
            l.v2.y = (value >> 16) & 0xFFFF;
            break;
        }
        case 12: {
            l.v2.z = value & 0xFFFF;
            break;
        }
        case 13: {
            bk.r = value;
            break;
        }
        case 14: {
            bk.g = value;
            break;
        }
        case 15: {
            bk.b = value;
            break;
        }
        case 16: {
            lr.v0.x = value & 0xFFFF;
            lr.v0.y = (value >> 16) & 0xFFFF;
            break;
        }
        case 17: {
            lr.v0.z = value & 0xFFFF;
            lr.v1.x = (value >> 16) & 0xFFFF;
            break;
        }
        case 18: {
            lr.v1.y = value & 0xFFFF;
            lr.v1.z = (value >> 16) & 0xFFFF;
            break;
        }
        case 19: {
            lr.v2.x = value & 0xFFFF;
            lr.v2.y = (value >> 16) & 0xFFFF;
            break;
        }
        case 20: {
            lr.v2.z = value & 0xFFFF;
            break;
        }
        case 21: {
            fc.r = value;
            break;
        }
        case 22: {
            fc.g = value;
            break;
        }
        case 23: {
            fc.b = value;
            break;
        }
        case 24: {
            of.x = value;
            break;
        }
        case 25: {
            of.y = value;
            break;
        }
        case 26: {
            hl = value;
            break;
        }
        case 27: {
            dqa = value;
            break;
        }
        case 28: {
            dqb = value;
            break;
        }
        case 29: {
            zsf3 = value;
            break;
        }
        case 30: {
            zsf4 = value;
            break;
        }
        case 31: {
            flag._value = value & 0x7FFFF000;
            if ((flag._value & 0x7F87E000) != 0) {
                flag._value |= 0x80000000;
            }
            break;
        }
        default: {
            logger.logError("Unhandled CONTROL write at index: %d with value: %#x", index, value);
            break;
        }
    }
}

uint32_t GTE::getControl(uint32_t index) {
    uint32_t value = 0;
    switch (index) {
        case 0: {
            value = ((uint16_t)rt.v0.y << 16) | (uint16_t)rt.v0.x;
            break;
        }
        case 1: {
            value = ((uint16_t)rt.v1.x << 16) | (uint16_t)rt.v0.z;
            break;
        }
        case 2: {
            value = ((uint16_t)rt.v1.z << 16) |(uint16_t) rt.v1.y;
            break;
        }
        case 3: {
            value = ((uint16_t)rt.v2.y << 16) | (uint16_t)rt.v2.x;
            break;
        }
        case 4: {
            value = rt.v2.z;
            break;
        }
        case 5: {
            value = tr.x;
            break;
        }
        case 6: {
            value = tr.y;
            break;
        }
        case 7: {
            value = tr.z;
            break;
        }
        case 8: {
            value = ((uint16_t)l.v0.y << 16) | (uint16_t)l.v0.x;
            break;
        }
        case 9: {
            value = ((uint16_t)l.v1.x << 16) | (uint16_t)l.v0.z;
            break;
        }
        case 10: {
            value = ((uint16_t)l.v1.z << 16) | (uint16_t)l.v1.y;
            break;
        }
        case 11: {
            value = ((uint16_t)l.v2.y << 16) | (uint16_t)l.v2.x;
            break;
        }
        case 12: {
            value = l.v2.z;
            break;
        }
        case 13: {
            value = bk.r;
            break;
        }
        case 14: {
            value = bk.g;
            break;
        }
        case 15: {
            value = bk.b;
            break;
        }
        case 16: {
            value = ((uint16_t)lr.v0.y << 16) | (uint16_t)lr.v0.x;
            break;
        }
        case 17: {
            value = ((uint16_t)lr.v1.x << 16) | (uint16_t)lr.v0.z;
            break;
        }
        case 18: {
            value = ((uint16_t)lr.v1.z << 16) | (uint16_t)lr.v1.y;
            break;
        }
        case 19: {
            value = ((uint16_t)lr.v2.y << 16) | (uint16_t)lr.v2.x;
            break;
        }
        case 20: {
            value = lr.v2.z;
            break;
        }
        case 21: {
            value = fc.r;
            break;
        }
        case 22: {
            value = fc.g;
            break;
        }
        case 23: {
            value = fc.b;
            break;
        }
        case 24: {
            value = of.x;
            break;
        }
        case 25: {
            value = of.y;
            break;
        }
        case 26: {
            value = (int16_t)hl;
            break;
        }
        case 27: {
            value = dqa;
            break;
        }
        case 28: {
            value = dqb;
            break;
        }
        case 29: {
            value = zsf3;
            break;
        }
        case 30: {
            value = zsf4;
            break;
        }
        case 31: {
            value = flag._value;
            break;
        }
        default: {
            logger.logError("Unhandled CONTROL read at index: %d", index);
            break;
        }
    }
    logger.logMessage("CONTROL [R] (IDX: %d): %#x", index, value);
    return value;
}

void GTE::execute(uint32_t value) {
    GTEInstruction instruction = GTEInstruction(value);
    flag._value = 0;
    switch (instruction.command) {
        case 0x1: {
            perspectiveTransformation(instruction, 0);
            break;
        }
        case 0x6: {
            normalClipping(instruction);
            break;
        }
        case 0xc: {
            outerProductOfTwoVectors(instruction);
            break;
        }
        case 0x1b: {
            normalColorColorSingleVector(instruction);
            break;
        }
        case 0x1e: {
            normalColorNCS(instruction, 0);
            break;
        }
        case 0x20: {
            normalColorNCT(instruction);
            break;
        }
        case 0x28: {
            squareVector(instruction);
            break;
        }
        case 0x2d: {
            averageOfThreeZValues(instruction);
            break;
        }
        case 0x2e: {
            averageOfFourZValues(instruction);
            break;
        }
        case 0x30: {
            perspectiveTransformationOnThreePoints(instruction);
            break;
        }
        case 0x3d: {
            generalPurposeInterpolationGPF(instruction);
            break;
        }
        case 0x3e: {
            generalPurposeInterpolationGPL(instruction);
            break;
        }
        default: {
            logger.logError("Unhandled Geometry Transformation Engine command: %#x", instruction.command);
            break;
        }
    }
    if ((flag._value & 0x7F87E000) != 0) {
        flag._value |= 0x80000000;
    }
}

/*
SQR      5        Square vector.
Fields:  sf
Opcode:  cop2 $0a00428
                                                       sf=0    sf=1
in:      [IR1,IR2,IR3]     vector                      [1,15,0][1,3,12]
out:     [IR1,IR2,IR3]     vector^2                    [1,15,0][1,3,12]
         [MAC1,MAC2,MAC3]  vector^2                    [1,31,0][1,19,12]

Calculation: (left format sf=0, right format sf=1)

[1,31,0][1,19,12] MAC1=A1[IR1*IR1]                     [1,43,0][1,31,12]
[1,31,0][1,19,12] MAC2=A2[IR2*IR2]                     [1,43,0][1,31,12]
[1,31,0][1,19,12] MAC3=A3[IR3*IR3]                     [1,43,0][1,31,12]
[1,15,0][1,3,12]  IR1=Lm_B1[MAC1]                      [1,31,0][1,19,12][lm=1]
[1,15,0][1,3,12]  IR2=Lm_B2[MAC2]                      [1,31,0][1,19,12][lm=1]
[1,15,0][1,3,12]  IR3=Lm_B3[MAC3]                      [1,31,0][1,19,12][lm=1]
*/
void GTE::squareVector(GTEInstruction instruction) {
    mac1 = flag.calculateMAC(1, (ir1 * ir1) >> (instruction.shiftFraction * 12));
    mac2 = flag.calculateMAC(2, (ir2 * ir2) >> (instruction.shiftFraction * 12));
    mac3 = flag.calculateMAC(3, (ir3 * ir3) >> (instruction.shiftFraction * 12));

    ir1 = flag.calculateIR(1, mac1, instruction.lm);
    ir2 = flag.calculateIR(2, mac2, instruction.lm);
    ir3 = flag.calculateIR(3, mac3, instruction.lm);
}

/*
NCLIP    8        Normal clipping
Fields:
Opcode:  cop2 $1400006

in:      SXY0,SXY1,SXY2    Screen coordinates                  [1,15,0]
out:     MAC0              Outerproduct of SXY1 and SXY2 with  [1,31,0]
                           SXY0 as origin.

Calculation:
[1,31,0] MAC0 = F[SX0*SY1+SX1*SY2+SX2*SY0-SX0*SY2-SX1*SY0-SX2*SY1] [1,43,0]
*/
void GTE::normalClipping(GTEInstruction instruction) {
    // TODO: unused
    (void)instruction;
    mac0 = flag.calculateMAC0((int64_t)sxy0.x * sxy1.y + sxy1.x * sxy2.y + sxy2.x * sxy0.y - sxy0.x * sxy2.y - sxy1.x * sxy0.y - sxy2.x * sxy1.y);
}

/*
AVSZ3    5        Average of three Z values
fields:
Opcode:  cop2 $158002D

in:      SZ1, SZ2, SZ3     Z-Values                            [0,16,0]
         ZSF3              Divider                             [1,3,12]
out:     OTZ               Average.                            [0,16,0]
         MAC0              Average.                            [1,31,0]

Calculation:
[1,31,0] MAC0=F[ZSF3*SZ1 + ZSF3*SZ2 + ZSF3*SZ3]                [1,31,12]
[0,16,0] OTZ=Lm_D[MAC0]                                        [1,31,0]
*/
void GTE::averageOfThreeZValues(GTEInstruction instruction) {
    // TODO: unused
    (void)instruction;
    int64_t average = (int64_t)zsf3 * sz1 + zsf3 * sz2 + zsf3 * sz3;
    mac0 = flag.calculateMAC0(average);
    otz = flag.calculateSZ3(average >> 12);
}

/*
AVSZ4    6        Average of four Z values
Fields:
Opcode:  cop2 $168002E

in:      SZ1,SZ2,SZ3,SZ4   Z-Values                            [0,16,0]
         ZSF4              Divider                             [1,3,12]
out:     OTZ               Average.                            [0,16,0]
         MAC0              Average.                            [1,31,0]

Calculation:
[1,31,0] MAC0=F[ZSF4*SZ0 + ZSF4*SZ1 + ZSF4*SZ2 + ZSF4*SZ3]     [1,31,12]
[0,16,0] OTZ=Lm_D[MAC0]                                        [1,31,0]
*/
void GTE::averageOfFourZValues(GTEInstruction instruction) {
    // TODO: unused
    (void)instruction;
    int64_t average = (int64_t)zsf4 * sz0 + zsf4 * sz1 + zsf4 * sz2 + zsf4 * sz3;
    mac0 = flag.calculateMAC0(average);
    otz = flag.calculateSZ3(average >> 12);
}

/*
OP       6        Outer product of 2 vectors
Fields:  sf
Opcode:  cop2 $170000C

in:      [R11R12,R22R23,R33] vector 1
         [IR1,IR2,IR3]      vector 2
out:     [IR1,IR2,IR3]      outer product
         [MAC1,MAC2,MAC3]   outer product

Calculation: (D1=R11R12,D2=R22R23,D3=R33)

         MAC1=A1[D2*IR3 - D3*IR2]
         MAC2=A2[D3*IR1 - D1*IR3]
         MAC3=A3[D1*IR2 - D2*IR1]
         IR1=Lm_B1[MAC0]
         IR2=Lm_B2[MAC1]
         IR3=Lm_B3[MAC2]
*/
void GTE::outerProductOfTwoVectors(GTEInstruction instruction) {
    int16_t d1 = rt.v0.x;
    int16_t d2 = rt.v1.y;
    int16_t d3 = rt.v2.z;

    mac1 = flag.calculateMAC(1, ((d2 * ir3) - (d3 * ir2)) >> (instruction.shiftFraction * 12));
    mac2 = flag.calculateMAC(2, ((d3 * ir1) - (d1 * ir3)) >> (instruction.shiftFraction * 12));
    mac3 = flag.calculateMAC(3, ((d1 * ir2) - (d2 * ir1)) >> (instruction.shiftFraction * 12));

    ir1 = flag.calculateIR(1, mac1, instruction.lm);
    ir2 = flag.calculateIR(2, mac2, instruction.lm);
    ir3 = flag.calculateIR(3, mac3, instruction.lm);
}

/*
GPF      5        General purpose interpolation
Fields:  sf
Opcode:  cop2 $190003D

in:      IR0               scaling factor
         CODE              code field of RGB
         [IR1,IR2,IR3]     vector
out:     [IR1,IR2,IR3]     vector
         [MAC1,MAC2,MAC3]  vector
         RGB2              RGB fifo.

Calculation:

         MAC1=A1[IR0 * IR1]
         MAC2=A2[IR0 * IR2]
         MAC3=A3[IR0 * IR3]
         IR1=Lm_B1[MAC1]
         IR2=Lm_B2[MAC2]
         IR3=Lm_B3[MAC3]
[0,8,0]   Cd0<-Cd1<-Cd2<- CODE
[0,8,0]   R0<-R1<-R2<- Lm_C1[MAC1]
[0,8,0]   G0<-G1<-G2<- Lm_C2[MAC2]
[0,8,0]   B0<-B1<-B2<- Lm_C3[MAC3]
*/
void GTE::generalPurposeInterpolationGPF(GTEInstruction instruction) {
    mac1 = flag.calculateMAC(1, (ir0 * ir1)) >> (instruction.shiftFraction * 12);
    mac2 = flag.calculateMAC(2, (ir0 * ir2)) >> (instruction.shiftFraction * 12);
    mac3 = flag.calculateMAC(3, (ir0 * ir3)) >> (instruction.shiftFraction * 12);

    ir1 = flag.calculateIR(1, mac1, instruction.lm);
    ir2 = flag.calculateIR(2, mac2, instruction.lm);
    ir3 = flag.calculateIR(3, mac3, instruction.lm);

    rgb0._value = rgb1._value;
    rgb1._value = rgb2._value;

    rgb2.r = flag.calculateRGB(1, mac1 >> 4);
    rgb2.g = flag.calculateRGB(2, mac2 >> 4);
    rgb2.b = flag.calculateRGB(3, mac3 >> 4);
    rgb2.c = rgbc.c;
}

/*
GPL      5        General purpose interpolation
Fields:  sf
Opcode:  cop2 $1A0003E

in:      IR0               scaling factor
         CODE              code field of RGB
         [IR1,IR2,IR3]     vector
         [MAC1,MAC2,MAC3]  vector
out:     [IR1,IR2,IR3]     vector
         [MAC1,MAC2,MAC3]  vector
         RGB2              RGB fifo.

Calculation:

         MAC1=A1[MAC1 + IR0 * IR1]
         MAC2=A2[MAC2 + IR0 * IR2]
         MAC3=A3[MAC3 + IR0 * IR3]
         IR1=Lm_B1[MAC1]
         IR2=Lm_B2[MAC2]
         IR3=Lm_B3[MAC3]
[0,8,0]  Cd0<-Cd1<-Cd2<- CODE
[0,8,0]  R0<-R1<-R2<- Lm_C1[MAC1]
[0,8,0]  G0<-G1<-G2<- Lm_C2[MAC2]
[0,8,0]  B0<-B1<-B2<- Lm_C3[MAC3]
*/
void GTE::generalPurposeInterpolationGPL(GTEInstruction instruction) {
    int64_t m1 = (int64_t)mac1 << (instruction.shiftFraction * 12);
    int64_t m2 = (int64_t)mac2 << (instruction.shiftFraction * 12);
    int64_t m3 = (int64_t)mac3 << (instruction.shiftFraction * 12);

    mac1 = flag.calculateMAC(1, m1 + (ir0 * ir1)) >> (instruction.shiftFraction * 12);
    mac2 = flag.calculateMAC(2, m2 + (ir0 * ir2)) >> (instruction.shiftFraction * 12);
    mac3 = flag.calculateMAC(3, m3 + (ir0 * ir3)) >> (instruction.shiftFraction * 12);

    ir1 = flag.calculateIR(1, mac1, instruction.lm);
    ir2 = flag.calculateIR(2, mac2, instruction.lm);
    ir3 = flag.calculateIR(3, mac3, instruction.lm);

    rgb0._value = rgb1._value;
    rgb1._value = rgb2._value;

    rgb2.r = flag.calculateRGB(1, mac1 >> 4);
    rgb2.g = flag.calculateRGB(2, mac2 >> 4);
    rgb2.b = flag.calculateRGB(3, mac3 >> 4);
    rgb2.c = rgbc.c;
}

/*
RTPS     15       Perspective transformation
Fields:  none
Opcode:  cop2 $0180001

In:      V0       Vector to transform.                         [1,15,0]
         R        Rotation matrix                              [1,3,12]
         TR       Translation vector                           [1,31,0]
         H        View plane distance                          [0,16,0]
         DQA      Depth que interpolation values.              [1,7,8]
         DQB                                                   [1,7,8]
         OFX      Screen offset values.                        [1,15,16]
         OFY                                                   [1,15,16]
Out:     SXY fifo Screen XY coordinates.(short)                [1,15,0]
         SZ fifo  Screen Z coordinate.(short)                  [0,16,0]
         IR0      Interpolation value for depth queing.        [1,3,12]
         IR1      Screen X (short)                             [1,15,0]
         IR2      Screen Y (short)                             [1,15,0]
         IR3      Screen Z (short)                             [1,15,0]
         MAC1     Screen X (long)                              [1,31,0]
         MAC2     Screen Y (long)                              [1,31,0]
         MAC3     Screen Z (long)                              [1,31,0]

Calculation:
[1,31,0] MAC1=A1[TRX + R11*VX0 + R12*VY0 + R13*VZ0]            [1,31,12]
[1,31,0] MAC2=A2[TRY + R21*VX0 + R22*VY0 + R23*VZ0]            [1,31,12]
[1,31,0] MAC3=A3[TRZ + R31*VX0 + R32*VY0 + R33*VZ0]            [1,31,12]
[1,15,0] IR1= Lm_B1[MAC1]                                      [1,31,0]
[1,15,0] IR2= Lm_B2[MAC2]                                      [1,31,0]
[1,15,0] IR3= Lm_B3[MAC3]                                      [1,31,0]
         SZ0<-SZ1<-SZ2<-SZ3
[0,16,0] SZ3= Lm_D(MAC3)                                       [1,31,0]
         SX0<-SX1<-SX2, SY0<-SY1<-SY2
[1,15,0] SX2= Lm_G1[F[OFX + IR1*(H/SZ)]]                       [1,27,16]
[1,15,0] SY2= Lm_G2[F[OFY + IR2*(H/SZ)]]                       [1,27,16]
[1,31,0] MAC0= F[DQB + DQA * (H/SZ)]                           [1,19,24]
[1,15,0] IR0= Lm_H[MAC0]                                       [1,31,0]
*/
void GTE::perspectiveTransformation(GTEInstruction instruction, unsigned int index) {
    GTEVector3_16_t v = {};
    switch (index) {
        case 0: {
            v = v0;
            break;
        }
        case 1: {
            v = v1;
            break;
        }
        case 2: {
            v = v2;
            break;
        }
        default: {
            logger.logError("Unhandled index in RTPS operation with index: %d", index);
            break;
        }
    }
    mac1 = flag.calculateMAC(1, (tr.x * 0x1000 + rt.v0.x * v.x + rt.v0.y * v.y + rt.v0.z * v.z) >> (instruction.shiftFraction * 12));
    mac2 = flag.calculateMAC(2, (tr.y * 0x1000 + rt.v1.x * v.x + rt.v1.y * v.y + rt.v1.z * v.z) >> (instruction.shiftFraction * 12));
    mac3 = flag.calculateMAC(3, (tr.z * 0x1000 + rt.v2.x * v.x + rt.v2.y * v.y + rt.v2.z * v.z) >> (instruction.shiftFraction * 12));

    ir1 = flag.calculateIR(1, mac1, false);
    ir2 = flag.calculateIR(2, mac2, false);
    ir3 = flag.calculateIR(3, mac3, false);

    sz0 = sz1;
    sz1 = sz2;
    sz2 = sz3;
    sz3 = flag.calculateSZ3(mac3 >> ((1 - instruction.shiftFraction) * 12));

    sxy0 = sxy1;
    sxy1 = sxy2;

    int64_t result = 0;
    int64_t divide = 0;
    if (sz3 == 0) {
        result = 0x1FFFF;
    } else {
        divide = (((int64_t)hl * 0x20000 / sz3) + 1) / 2;
        if (divide > 0x1FFFF) {
            result = 0x1FFFF;
            flag.divideOverflow = 1;
        } else {
            result = divide;
        }
    }

    mac0 = result * ir1 + of.x;
    sxy2.x = flag.calculateSXY2(2, mac0 / 0x10000);
    mac0 = result * ir2 + of.y;
    sxy2.y = flag.calculateSXY2(2, mac0 / 0x10000);
    mac0 = result * dqa + dqb;
    ir0 = flag.calculateIR0(mac0 / 0x1000);
}

/*
RTPT     23       Perspective Transformation on 3 points.
Fields   none
opcode   cop2 $0280030

in       V0       Vector to transform.                         [1,15,0]
         V1                                                    [1,15,0]
         V2                                                    [1,15,0]
         R        Rotation matrix                              [1,3,12]
         TR       Translation vector                           [1,31,0]
         H        View plane distance                          [0,16,0]
         DQA      Depth que interpolation values.              [1,7,8]
         DQB                                                   [1,7,8]
         OFX      Screen offset values.                        [1,15,16]
         OFY                                                   [1,15,16]
out      SXY fifo Screen XY coordinates.(short)                [1,15,0]
         SZ fifo  Screen Z coordinate.(short)                  [0,16,0]
         IR0      Interpolation value for depth queing.        [1,3,12]
         IR1      Screen X (short)                             [1,15,0]
         IR2      Screen Y (short)                             [1,15,0]
         IR3      Screen Z (short)                             [1,15,0]
         MAC1     Screen X (long)                              [1,31,0]
         MAC2     Screen Y (long)                              [1,31,0]
         MAC3     Screen Z (long)                              [1,31,0]

Calculation: Same as RTPS, but repeats for V1 and V2.
*/
void GTE::perspectiveTransformationOnThreePoints(GTEInstruction instruction) {
    // TODO: unused
    (void)instruction;
    perspectiveTransformation(instruction, 0);
    perspectiveTransformation(instruction, 1);
    perspectiveTransformation(instruction, 2);
}

/*
NCS      14       Normal color
Fields:  none
Opcode:  cop2 $0C8041E

In:      V0                Normal vector                       [1,3,12]
         BK                Background color       RBK,GBK,BBK  [1,19,12]
         CODE              Code value from RGB.           CODE [0,8,0]
         LCM               Color matrix                        [1,3,12]
         LLM               Light matrix                        [1,3,12]
Out:     RGBn              RGB fifo.              Rn,Gn,Bn,CDn [0,8,0]
         [IR1,IR2,IR3]     Color vector                        [1,11,4]
         [MAC1,MAC2,MAC3]  Color vector                        [1,27,4]

[1,19,12] MAC1=A1[L11*VX0 + L12*VY0 + L13*VZ0]                 [1,19,24]
[1,19,12] MAC2=A2[L21*VX0 + L22*VY0 + L23*VZ0]                 [1,19,24]
[1,19,12] MAC3=A3[L31*VX0 + L32*VY0 + L33*VZ0]                 [1,19,24]
[1,3,12]  IR1= Lm_B1[MAC1]                                     [1,19,12][lm=1]
[1,3,12]  IR2= Lm_B2[MAC2]                                     [1,19,12][lm=1]
[1,3,12]  IR3= Lm_B3[MAC3]                                     [1,19,12][lm=1]
[1,19,12] MAC1=A1[RBK + LR1*IR1 + LR2*IR2 + LR3*IR3]           [1,19,24]
[1,19,12] MAC2=A2[GBK + LG1*IR1 + LG2*IR2 + LG3*IR3]           [1,19,24]
[1,19,12] MAC3=A3[BBK + LB1*IR1 + LB2*IR2 + LB3*IR3]           [1,19,24]
[1,3,12]  IR1= Lm_B1[MAC1]                                     [1,19,12][lm=1]
[1,3,12]  IR2= Lm_B2[MAC2]                                     [1,19,12][lm=1]
[1,3,12]  IR3= Lm_B3[MAC3]                                     [1,19,12][lm=1]
[0,8,0]   Cd0<-Cd1<-Cd2<- CODE
[0,8,0]   R0<-R1<-R2<- Lm_C1[MAC1]                             [1,27,4]
[0,8,0]   G0<-G1<-G2<- Lm_C2[MAC2]                             [1,27,4]
[0,8,0]   B0<-B1<-B2<- Lm_C3[MAC3]                             [1,27,4]
*/
void GTE::normalColorNCS(GTEInstruction instruction, unsigned int index) {
    GTEVector3_16_t v = {};
    switch (index) {
        case 0: {
            v = v0;
            break;
        }
        case 1: {
            v = v1;
            break;
        }
        case 2: {
            v = v2;
            break;
        }
        default: {
            logger.logError("Unhandled index in NCS operation with index: %d", index);
            break;
        }
    }
    mac1 = flag.calculateMAC(1, (int64_t)l.v0.x * v.x + l.v0.y * v.y + l.v0.z * v.z) >> (instruction.shiftFraction * 12);
    mac2 = flag.calculateMAC(2, (int64_t)l.v1.x * v.x + l.v1.y * v.y + l.v1.z * v.z) >> (instruction.shiftFraction * 12);
    mac3 = flag.calculateMAC(3, (int64_t)l.v2.x * v.x + l.v2.y * v.y + l.v2.z * v.z) >> (instruction.shiftFraction * 12);

    ir1 = flag.calculateIR(1, mac1, instruction.lm);
    ir2 = flag.calculateIR(2, mac2, instruction.lm);
    ir3 = flag.calculateIR(3, mac3, instruction.lm);

    int64_t temporalMAC = 0;

    temporalMAC = flag.calculateMAC(1, (int64_t)bk.r * 0x1000 + lr.v0.x * ir1);
    temporalMAC = flag.calculateMAC(1, temporalMAC + (int64_t)lr.v0.y * ir2);
    temporalMAC = flag.calculateMAC(1, temporalMAC + (int64_t)lr.v0.z * ir3);
    mac1 = temporalMAC >> (instruction.shiftFraction * 12);

    temporalMAC = flag.calculateMAC(2, (int64_t)bk.g * 0x1000 + lr.v1.x * ir1);
    temporalMAC = flag.calculateMAC(2, temporalMAC + (int64_t)lr.v1.y * ir2);
    temporalMAC = flag.calculateMAC(2, temporalMAC + (int64_t)lr.v1.z * ir3);
    mac2 = temporalMAC >> (instruction.shiftFraction * 12);

    temporalMAC = flag.calculateMAC(3, (int64_t)bk.b * 0x1000 + lr.v2.x * ir1);
    temporalMAC = flag.calculateMAC(3, temporalMAC + (int64_t)lr.v2.y * ir2);
    temporalMAC = flag.calculateMAC(3, temporalMAC + (int64_t)lr.v2.z * ir3);
    mac3 = temporalMAC >> (instruction.shiftFraction * 12);

    ir1 = flag.calculateIR(1, mac1, instruction.lm);
    ir2 = flag.calculateIR(2, mac2, instruction.lm);
    ir3 = flag.calculateIR(3, mac3, instruction.lm);

    rgb0._value = rgb1._value;
    rgb1._value = rgb2._value;

    rgb2.r = flag.calculateRGB(1, mac1 >> 4);
    rgb2.g = flag.calculateRGB(2, mac2 >> 4);
    rgb2.b = flag.calculateRGB(3, mac3 >> 4);
    rgb2.c = rgbc.c;
}

/*
NCT      30       Normal color
Fields:  none
Opcode:  cop2 $0D80420

In:      V0,V1,V2          Normal vector                       [1,3,12]
         BK                Background color       RBK,GBK,BBK  [1,19,12]
         CODE              Code value from RGB.           CODE [0,8,0]
         LCM               Color matrix                        [1,3,12]
         LLM               Light matrix                        [1,3,12]
Out:     RGBn              RGB fifo.              Rn,Gn,Bn,CDn [0,8,0]
         [IR1,IR2,IR3]     Color vector                        [1,11,4]
         [MAC1,MAC2,MAC3]  Color vector                        [1,27,4]

Calculation: Same as NCS, but repeated for V1 and V2.
*/
void GTE::normalColorNCT(GTEInstruction instruction) {
    normalColorNCS(instruction, 0);
    normalColorNCS(instruction, 1);
    normalColorNCS(instruction, 2);
}

/*
NCCS     17       Normal Color Color single vector
Fields:  none
Opcode:  cop2 $108041B

In:      V0                Normal vector                       [1,3,12]
         BK                Background color       RBK,GBK,BBK  [1,19,12]
         RGB               Primary color          R,G,B,CODE   [0,8,0]
         LLM               Light matrix                        [1,3,12]
         LCM               Color matrix                        [1,3,12]
Out:     RGBn              RGB fifo.              Rn,Gn,Bn,CDn [0,8,0]
         [IR1,IR2,IR3]     Color vector                        [1,11,4]
         [MAC1,MAC2,MAC3]  Color vector                        [1,27,4]

Calculation:

[1,19,12] MAC1=A1[L11*VX0 + L12*VY0 + L13*VZ0]                  [1,19,24]
[1,19,12] MAC2=A2[L21*VX0 + L22*VY0 + L23*VZ0]                  [1,19,24]
[1,19,12] MAC3=A3[L31*VX0 + L32*VY0 + L33*VZ0]                  [1,19,24]
[1,3,12]  IR1= Lm_B1[MAC1]                                      [1,19,12][lm=1]
[1,3,12]  IR2= Lm_B2[MAC2]                                      [1,19,12][lm=1]
[1,3,12]  IR3= Lm_B3[MAC3]                                      [1,19,12][lm=1]
[1,19,12] MAC1=A1[RBK + LR1*IR1 + LR2*IR2 + LR3*IR3]            [1,19,24]
[1,19,12] MAC2=A2[GBK + LG1*IR1 + LG2*IR2 + LG3*IR3]            [1,19,24]
[1,19,12] MAC3=A3[BBK + LB1*IR1 + LB2*IR2 + LB3*IR3]            [1,19,24]
[1,3,12]  IR1= Lm_B1[MAC1]                                      [1,19,12][lm=1]
[1,3,12]  IR2= Lm_B2[MAC2]                                      [1,19,12][lm=1]
[1,3,12]  IR3= Lm_B3[MAC3]                                      [1,19,12][lm=1]
[1,27,4]  MAC1=A1[R*IR1]                                        [1,27,16]
[1,27,4]  MAC2=A2[G*IR2]                                        [1,27,16]
[1,27,4]  MAC3=A3[B*IR3]                                        [1,27,16]
[1,3,12]  IR1= Lm_B1[MAC1]                                      [1,27,4][lm=1]
[1,3,12]  IR2= Lm_B2[MAC2]                                      [1,27,4][lm=1]
[1,3,12]  IR3= Lm_B3[MAC3]                                      [1,27,4][lm=1]
[0,8,0]   Cd0<-Cd1<-Cd2<- CODE
[0,8,0]   R0<-R1<-R2<- Lm_C1[MAC1]                              [1,27,4]
[0,8,0]   G0<-G1<-G2<- Lm_C2[MAC2]                              [1,27,4]
[0,8,0]   B0<-B1<-B2<- Lm_C3[MAC3]                              [1,27,4]
*/
void GTE::normalColorColorSingleVector(GTEInstruction instruction) {
    mac1 = flag.calculateMAC(1, (int64_t)l.v0.x * v0.x + l.v0.y * v0.y + l.v0.z * v0.z) >> (instruction.shiftFraction * 12);
    mac2 = flag.calculateMAC(2, (int64_t)l.v1.x * v0.x + l.v1.y * v0.y + l.v1.z * v0.z) >> (instruction.shiftFraction * 12);
    mac3 = flag.calculateMAC(3, (int64_t)l.v2.x * v0.x + l.v2.y * v0.y + l.v2.z * v0.z) >> (instruction.shiftFraction * 12);

    ir1 = flag.calculateIR(1, mac1, instruction.lm);
    ir2 = flag.calculateIR(2, mac2, instruction.lm);
    ir3 = flag.calculateIR(3, mac3, instruction.lm);

    int64_t temporalMAC = 0;

    temporalMAC = flag.calculateMAC(1, (int64_t)bk.r * 0x1000 + lr.v0.x * ir1);
    temporalMAC = flag.calculateMAC(1, temporalMAC + (int64_t)lr.v0.y * ir2);
    temporalMAC = flag.calculateMAC(1, temporalMAC + (int64_t)lr.v0.z * ir3);
    mac1 = temporalMAC >> (instruction.shiftFraction * 12);

    temporalMAC = flag.calculateMAC(2, (int64_t)bk.g * 0x1000 + lr.v1.x * ir1);
    temporalMAC = flag.calculateMAC(2, temporalMAC + (int64_t)lr.v1.y * ir2);
    temporalMAC = flag.calculateMAC(2, temporalMAC + (int64_t)lr.v1.z * ir3);
    mac2 = temporalMAC >> (instruction.shiftFraction * 12);

    temporalMAC = flag.calculateMAC(3, (int64_t)bk.b * 0x1000 + lr.v2.x * ir1);
    temporalMAC = flag.calculateMAC(3, temporalMAC + (int64_t)lr.v2.y * ir2);
    temporalMAC = flag.calculateMAC(3, temporalMAC + (int64_t)lr.v2.z * ir3);
    mac3 = temporalMAC >> (instruction.shiftFraction * 12);

    ir1 = flag.calculateIR(1, mac1, instruction.lm);
    ir2 = flag.calculateIR(2, mac2, instruction.lm);
    ir3 = flag.calculateIR(3, mac3, instruction.lm);

    mac1 = flag.calculateMAC(1, (rgbc.r * ir1) << 4);
    mac2 = flag.calculateMAC(2, (rgbc.g * ir2) << 4);
    mac3 = flag.calculateMAC(3, (rgbc.b * ir3) << 4);

    mac1 = flag.calculateMAC(1, mac1 >> (instruction.shiftFraction * 12));
    mac2 = flag.calculateMAC(2, mac2 >> (instruction.shiftFraction * 12));
    mac3 = flag.calculateMAC(3, mac3 >> (instruction.shiftFraction * 12));

    ir1 = flag.calculateIR(1, mac1, instruction.lm);
    ir2 = flag.calculateIR(2, mac2, instruction.lm);
    ir3 = flag.calculateIR(3, mac3, instruction.lm);

    rgb0._value = rgb1._value;
    rgb1._value = rgb2._value;

    rgb2.r = flag.calculateRGB(1, mac1 >> 4);
    rgb2.g = flag.calculateRGB(2, mac2 >> 4);
    rgb2.b = flag.calculateRGB(3, mac3 >> 4);
    rgb2.c = rgbc.c;
}
