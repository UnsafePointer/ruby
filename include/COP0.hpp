#pragma once
#include <cstdint>
#include <memory>

enum ExceptionType : uint32_t {
    Interrupt = 0x0,
    LoadAddress = 0x4,
    StoreAddress = 0x5,
    BusErrorOnInstruction = 0x6,
    BusErrorOnData = 0x7,
    SysCall = 0x8,
    Breakpoint = 0x9,
    Illegal = 0xA,
    Coprocessor = 0xB,
    Overflow = 0xC,
};

/*
cop0r13 - CAUSE - (Read-only, except, Bit8-9 are R/W)
Describes the most recently recognised exception
0-1   -      Not used (zero)
2-6   Excode Describes what kind of exception occured:
                00h INT     Interrupt
                01h MOD     Tlb modification (none such in PSX)
                02h TLBL    Tlb load         (none such in PSX)
                03h TLBS    Tlb store        (none such in PSX)
                04h AdEL    Address error, Data load or Instruction fetch
                05h AdES    Address error, Data store
                            The address errors occur when attempting to read
                            outside of KUseg in user mode and when the address
                            is misaligned. (See also: BadVaddr register)
                06h IBE     Bus error on Instruction fetch
                07h DBE     Bus error on Data load/store
                08h Syscall Generated unconditionally by syscall instruction
                09h BP      Breakpoint - break instruction
                0Ah RI      Reserved instruction
                0Bh CpU     Coprocessor unusable
                0Ch Ov      Arithmetic overflow
                0Dh-1Fh     Not used
7     -      Not used (zero)
8-15  Ip     Interrupt pending field. Bit 8 and 9 are R/W, and
            contain the last value written to them. As long
            as any of the bits are set they will cause an
            interrupt if the corresponding bit is set in IM.
16-27 -      Not used (zero)
28-29 CE     Contains the coprocessor number if the exception
            occurred because of a coprocessor instuction for
            a coprocessor which wasn't enabled in SR.
30    -      Not used (zero)
31    BD     Is set when last exception points to the
            branch instuction instead of the instruction
            in the branch delay slot, where the exception
            occurred.
*/
union CauseRegister {
    struct {
        uint32_t : 2;
        uint32_t _exception : 5;
        uint32_t : 1;
        uint32_t interruptPending : 8;
        uint32_t : 12;
        uint32_t coprocessorNumber : 2;
        uint32_t : 1;
        uint32_t branchDelay : 1;
    };
    uint32_t value;

    CauseRegister() : value(0) {}

    ExceptionType exception() { return ExceptionType(_exception); }
};

enum OperationMode : uint32_t {
    Kernel = 0,
    User = 1,
};

enum BootExceptionVectors : uint32_t {
    RAMV = 0,
    ROMV = 1,
};

/*
cop0r12 - SR - System status register (R/W)
0     IEc Current Interrupt Enable  (0=Disable, 1=Enable) ;rfe pops IUp here
1     KUc Current Kernal/User Mode  (0=Kernel, 1=User)    ;rfe pops KUp here
2     IEp Previous Interrupt Disable                      ;rfe pops IUo here
3     KUp Previous Kernal/User Mode                       ;rfe pops KUo here
4     IEo Old Interrupt Disable                       ;left unchanged by rfe
5     KUo Old Kernal/User Mode                        ;left unchanged by rfe
6-7   -   Not used (zero)
8-15  Im  8 bit interrupt mask fields. When set the corresponding
        interrupts are allowed to cause an exception.
16    Isc Isolate Cache (0=No, 1=Isolate)
            When isolated, all load and store operations are targetted
            to the Data cache, and never the main memory.
            (Used by PSX Kernel, in combination with Port FFFE0130h)
17    Swc Swapped cache mode (0=Normal, 1=Swapped)
            Instruction cache will act as Data cache and vice versa.
            Use only with Isc to access & invalidate Instr. cache entries.
            (Not used by PSX Kernel)
18    PZ  When set cache parity bits are written as 0.
19    CM  Shows the result of the last load operation with the D-cache
        isolated. It gets set if the cache really contained data
        for the addressed memory location.
20    PE  Cache parity error (Does not cause exception)
21    TS  TLB shutdown. Gets set if a programm address simultaneously
        matches 2 TLB entries.
        (initial value on reset allows to detect extended CPU version?)
22    BEV Boot exception vectors in RAM/ROM (0=RAM/KSEG0, 1=ROM/KSEG1)
23-24 -   Not used (zero)
25    RE  Reverse endianness   (0=Normal endianness, 1=Reverse endianness)
            Reverses the byte order in which data is stored in
            memory. (lo-hi -> hi-lo)
            (Has affect only to User mode, not to Kernel mode) (?)
            (The bit doesn't exist in PSX ?)
26-27 -   Not used (zero)
28    CU0 COP0 Enable (0=Enable only in Kernal Mode, 1=Kernal and User Mode)
29    CU1 COP1 Enable (0=Disable, 1=Enable) (none such in PSX)
30    CU2 COP2 Enable (0=Disable, 1=Enable) (GTE in PSX)
31    CU3 COP3 Enable (0=Disable, 1=Enable) (none such in PSX)
*/
union StatusRegister {
    struct {
        uint32_t currentInterruptEnable : 1;
        uint32_t _currentOperationMode  : 1;
        uint32_t previousInterruptEnable : 1;
        uint32_t _previousOperationMode : 1;
        uint32_t oldInterruptEnable : 1;
        uint32_t _oldOperationMode : 1;
        uint32_t : 2;
        uint32_t interrurptMask : 8;
        uint32_t isolateCache : 1;
        uint32_t : 5;
        uint32_t _bootExceptionVectors : 1;
        uint32_t : 9;
    };
    uint32_t value;

    StatusRegister() : value(0) {}

    OperationMode currentOperationMode() { return OperationMode(_currentOperationMode); }
    OperationMode previousOperationMode() { return OperationMode(_previousOperationMode); }
    OperationMode oldOperationMode() { return OperationMode(_oldOperationMode); }
    BootExceptionVectors bootExceptionVectors() { return BootExceptionVectors(_bootExceptionVectors); }
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
public:
    uint32_t breakPointOnExecute; // cop0r3
    uint32_t breakPointOnDataAccess; // cop0r5
    uint32_t jumpDestination; // cop0r6
    uint32_t breakPointControl; // cop0r7
    uint32_t badVirtualAddress; // cop0r8
    uint32_t dataAccessBreakpointMask; // cop0r9
    uint32_t executeBreakpointMask; // cop0r11
    StatusRegister status; // cop0r12
    CauseRegister cause; // cop0r13
    uint32_t returnAddressFromTrap; // cop0r14
    uint32_t processorID; // cop0r15

    COP0();
    ~COP0();

    bool isCacheIsolated();
    bool areInterruptsPending();
};
