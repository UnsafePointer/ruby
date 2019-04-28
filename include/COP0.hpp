#pragma once
#include <cstdint>
#include <memory>
#include "InterruptController.hpp"

enum ExceptionType : uint32_t {
    SysCall = 0x8,
    Overflow = 0xc,
    LoadAddress = 0x4,
    StoreAddress = 0x5,
    Break = 0x9,
    Coprocessor = 0xb,
    Illegal = 0xa,
    Interrupt = 0x0
};

/*
COP0 Register Summary
cop0r0-r2   - N/A
cop0r3      - BPC - Breakpoint on execute (R/W)
cop0r4      - N/A
cop0r5      - BDA - Breakpoint on data access (R/W)
cop0r6      - JUMPDEST - Randomly memorized jump address (R)
cop0r7      - DCIC - Breakpoint control (R/W)
cop0r8      - BadVaddr - Bad Virtual Address (R)
cop0r9      - BDAM - Data Access breakpoint mask (R/W)
cop0r10     - N/A
cop0r11     - BPCM - Execute breakpoint mask (R/W)
cop0r12     - SR - System status register (R/W)
cop0r13     - CAUSE - (R)  Describes the most recently recognised exception
cop0r14     - EPC - Return Address from Trap (R)
cop0r15     - PRID - Processor ID (R)
cop0r16-r31 - Garbage
cop0r32-r63 - N/A - None such (Control regs)
*/
class COP0 {
    uint32_t statusRegister;
    uint32_t causeRegister;
    uint32_t returnAddressFromTrap;

    bool areInterruptsEnabled();
public:
    COP0();
    ~COP0();

    bool isCacheIsolated();
    bool areInterruptsPending(std::unique_ptr<InterruptController> &interruptController);

    // GDB register naming and order used here:
    // status - 32
    uint32_t getStatusRegister();
    void setStatusRegister(uint32_t value);
    // badvaddr - 35
    uint32_t getReturnAddressFromTrap();
    void setReturnAddressFromTrap(uint32_t value);
    // cause - 36
    uint32_t getCauseRegister(std::unique_ptr<InterruptController> &interruptController);
    void setCauseRegister(uint32_t value);

    uint32_t updateRegistersWithException(ExceptionType exceptionType, uint32_t programCounter, bool isDelaySlot);
    void updateRegistersWithReturnFromException();
};
