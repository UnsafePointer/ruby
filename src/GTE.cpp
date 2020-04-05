#include "GTE.hpp"
#include "GTEInstruction.hpp"
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
    zsf4(0)
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
    logger.logError("Unhandled Geometry Transformation Engine command: %#x", instruction.command);
}
