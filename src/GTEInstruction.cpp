#include "GTEInstruction.hpp"

GTEInstruction::GTEInstruction(uint32_t value) : value(value) {}

GTEInstructionMVMVATranslationVector GTEInstruction::mvmvaTranslationVector() {
    return GTEInstructionMVMVATranslationVector(translationVector);
}

GTEInstructionMVMVAMultiplyVector GTEInstruction::mvmvaMultiplyVector() {
    return GTEInstructionMVMVAMultiplyVector(multiplyVector);
}

GTEInstructionMVMVATranslationMatrix GTEInstruction::mvmvaTranslationMatrix() {
    return GTEInstructionMVMVATranslationMatrix(translationMatrix);
}
