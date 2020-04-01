#include "GTE.hpp"
#include "GTEInstruction.hpp"

GTE::GTE(LogLevel logLevel) : logger(logLevel, "  GTE: ") {}

GTE::~GTE() {}

void GTE::execute(uint32_t value) {
    GTEInstruction instruction = GTEInstruction(value);
    logger.logError("Unhandled Geometry Transformation Engine command: %#x", instruction.command);
}
