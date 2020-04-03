#include "GTE.hpp"
#include "GTEInstruction.hpp"

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
