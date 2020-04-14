#pragma once
#include <cstdint>

enum GTEInstructionMVMVATranslationVector {
    TR,
    BK,
    FC,
    TranslationVectorNone
};

enum GTEInstructionMVMVAMultiplyVector {
    V0,
    V1,
    V2,
    IR
};

enum GTEInstructionMVMVATranslationMatrix {
    Rotation,
    Light,
    Color,
    ReservedMatrix
};

/*
GTE Command Encoding (COP2 imm25 opcodes)
0-5    Real GTE Command Number (00h..3Fh) (used by hardware)
6-9    Always zero                        (ignored by hardware)
10     lm - Saturate IR1,IR2,IR3 result (0=To -8000h..+7FFFh, 1=To 0..+7FFFh)
11-12  Always zero                        (ignored by hardware)
13-14  MVMVA Translation Vector (0=TR, 1=BK, 2=FC/Bugged, 3=None)
15-16  MVMVA Multiply Vector    (0=V0, 1=V1, 2=V2, 3=IR/long)
17-18  MVMVA Multiply Matrix    (0=Rotation. 1=Light, 2=Color, 3=Reserved)
19     sf - Shift Fraction in IR registers (0=No fraction, 1=12bit fraction)
20-24  Fake GTE Command Number (00h..1Fh) (ignored by hardware)
25-31  Must be 0100101b for "COP2 imm25" instructions
*/
union GTEInstruction {
    struct {
        uint32_t command : 6;
        uint32_t unused : 4;
        uint32_t lm : 1;
        uint32_t unused2 : 2;
        uint32_t translationVector : 2;
        uint32_t multiplyVector : 2;
        uint32_t translationMatrix : 2;
        uint32_t shiftFraction : 1;
        uint32_t unsued3 : 5;
        uint32_t known: 7;
    };
    uint32_t value;

    GTEInstruction() : value(0) {}
    GTEInstruction(uint32_t value);

    GTEInstructionMVMVATranslationVector mvmvaTranslationVector();
    GTEInstructionMVMVAMultiplyVector mvmvaMultiplyVector();
    GTEInstructionMVMVATranslationMatrix mvmvaTranslationMatrix();
};
