#include "CDROM.hpp"
#include "Output.hpp"

using namespace std;

CDROM::CDROM(unique_ptr<InterruptController> &interruptController) : status(), parameters(), response(), interruptQueue(), interruptController(interruptController) {

}

CDROM::~CDROM() {

}

void CDROM::step() {
    if (!interruptQueue.empty()) {
        if ((interrupt.enable & 0x7) & (interruptQueue.front() & 0x7)) {
            interruptController->trigger(InterruptRequestNumber::CDROMIRQ);
        }
    }
}

void CDROM::setStatusRegister(uint8_t value) {
    status.index = value & 0x3;
}

void CDROM::setInterruptRegister(uint8_t value) {
    interrupt.enable = value;
}

void CDROM::setInterruptFlagRegister(uint8_t value) {
    if (value & 0x40) {
        status.parameterFifoEmpty = 1;
        status.parameterFifoFull = 1;
        clearParameters();
    }
}

void CDROM::execute(uint8_t value) {
    switch (value) {
        case 0x19: {
            operationTest();
            break;
        }
        default: {
            printError("Unhandled CDROM operation value: %#x", value);
        }
    }
    clearParameters();
    updateStatusRegister();
}

uint8_t CDROM::getStatusRegister() const {
    return status._value;
}

uint8_t CDROM::getInterruptFlagRegister() const {
    uint8_t flags = 0b11100000;
    if (!interruptQueue.empty()) {
        flags |= interruptQueue.front() & 0x7;
    }
    return flags;
}

uint8_t CDROM::getReponse() {
    uint8_t value = 0;
    if (!response.empty()) {
        value = response.front();
        response.pop();
        updateStatusRegister();
    }
    return value;
}

void CDROM::clearParameters() {
    queue<uint8_t> empty;
    swap(parameters, empty);
}

void CDROM::pushParameter(uint8_t value) {
    if (parameters.size() >= 16) {
        printError("Parameter FIFO full");
    }
    parameters.push(value);
    updateStatusRegister();
}

void CDROM::pushResponse(uint8_t value) {
    if (response.size() >= 16) {
        printError("Response FIFO full");
    }
    response.push(value);
    updateStatusRegister();
}

void CDROM::updateStatusRegister() {
    status.parameterFifoEmpty = parameters.empty();
    status.parameterFifoFull = !(parameters.size() >= 16);
    status.responseFifoEmpty = !response.empty();
}

/*
sub  params  response           ;Effect
00h      -   INT3(stat)         ;Force motor on, clockwise, even if door open
01h      -   INT3(stat)         ;Force motor on, anti-clockwise, super-fast
02h      -   INT3(stat)         ;Force motor on, anti-clockwise, super-fast
03h      -   INT3(stat)         ;Force motor off (ignored during spin-up)
04h      -   INT3(stat)         ;Start SCEx reading and reset counters
05h      -   INT3(total,success);Stop SCEx reading and get counters
06h *    n   INT3(old)  ;\early ;Adjust balance in RAM, send CX(30+n XOR 7)
07h *    n   INT3(old)  ; PSX   ;Adjust gain in RAM, send CX(38+n XOR 7)
08h *    n   INT3(old)  ;/only  ;Adjust balance in RAM only
06h..0Fh -   INT5(11h,10h)      ;N/A (11h,20h when NONZERO number of params)
10h      -   INT3(stat) ;CX(..) ;Force motor on, anti-clockwise, super-fast
11h      -   INT3(stat) ;CX(03) ;Move Lens Up (leave parking position)
12h      -   INT3(stat) ;CX(02) ;Move Lens Down (enter parking position)
13h      -   INT3(stat) ;CX(28) ;Move Lens Outwards
14h      -   INT3(stat) ;CX(2C) ;Move Lens Inwards
15h      -   INT3(stat) ;CX(22) ;If motor on: Move outwards,inwards,motor off
16h      -   INT3(stat) ;CX(23) ;No effect?
17h      -   INT3(stat) ;CX(E8) ;Force motor on, clockwise, super-fast
18h      -   INT3(stat) ;CX(EA) ;Force motor on, anti-clockwise, super-fast
19h      -   INT3(stat) ;CX(25) ;No effect?
1Ah      -   INT3(stat) ;CX(21) ;No effect?
1Bh..1Fh -   INT5(11h,10h)      ;N/A (11h,20h when NONZERO number of params)
20h      -   INT3(yy,mm,dd,ver) ;Get cdrom BIOS date/version (yy,mm,dd,ver)
21h      -   INT3(n)            ;Get Drive Switches (bit0=POS0, bit1=DOOR)
22h ***  -   INT3("for ...")    ;Get Region ID String
23h ***  -   INT3("CXD...")     ;Get Chip ID String for Servo Amplifier
24h ***  -   INT3("CXD...")     ;Get Chip ID String for Signal Processor
25h ***  -   INT3("CXD...")     ;Get Chip ID String for Decoder/FIFO
26h..2Fh -   INT5(11h,10h)      ;N/A (11h,20h when NONZERO number of params)
30h *    i,x,y     INT3(stat)       ;Prototype/Debug stuff   ;\supported on
31h *    x,y       INT3(stat)       ;Prototype/Debug stuff   ; early PSX only
4xh *    i         INT3(x,y)        ;Prototype/Debug stuff   ;/
30h..4Fh ..        INT5(11h,10h)    ;N/A always 11h,10h (no matter of params)
50h      a[,b[,c]] INT3(stat)       ;Servo/Signal send CX(a:b:c)
51h **   39h,xx    INT3(stat,hi,lo) ;Servo/Signal send CX(39xx) with response
51h..5Fh -         INT5(11h,10h)    ;N/A
60h      lo,hi     INT3(databyte)   ;HC05 SUB-CPU read RAM and I/O ports
61h..70h -         INT5(11h,10h)    ;N/A
71h ***  adr       INT3(databyte)   ;Decoder Read one register
72h ***  adr,dat   INT3(stat)       ;Decoder Write one register
73h ***  adr,len   INT3(databytes..);Decoder Read multiple registers, bugged
74h ***  adr,len,..INT3(stat)       ;Decoder Write multiple registers, bugged
75h ***  -         INT3(lo,hi,lo,hi);Decoder Get Host Xfer Info Remain/Addr
76h ***  a,b,c,d   INT3(stat)       ;Decoder Prepare Transfer to/from SRAM
77h..FFh -         INT5(11h,10h)    ;N/A
*/
void CDROM::operationTest() {
    uint8_t subfunction = parameters.front();
    switch (subfunction) {
        case 0x20: {
            pushResponse(0x94); // 148
            pushResponse(0x09); // 9
            pushResponse(0x19); // 25
            pushResponse(0xc0); // 192
            interruptQueue.push(0x3);
            break;
        }
        default: {
            printError("Unhandled CDROM operation test with subfunction: %#x", subfunction);
        }
    }
}
