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

void GTE::execute(uint32_t value) {
    GTEInstruction instruction = GTEInstruction(value);
    logger.logError("Unhandled Geometry Transformation Engine command: %#x", instruction.command);
}
