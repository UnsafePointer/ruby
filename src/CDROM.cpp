#include "CDROM.hpp"
#include "Output.hpp"
#include "Helpers.hpp"

using namespace std;

const uint32_t SecondsPerMinute = 60;
const uint32_t SectorsPerSecond = 75;

CDROM::CDROM(unique_ptr<InterruptController> &interruptController, bool logActivity) : interruptController(interruptController), image(), logActivity(logActivity), status(), interrupt(), statusCode(), mode(), parameters(), response(), interruptQueue(), seekSector(), readSector() {

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
    logMessage(format("STATUS [W]: %#x", value));
    status.index = value & 0x3;
}

void CDROM::setInterruptRegister(uint8_t value) {
    logMessage(format("INTE [W]: %#x", value));
    interrupt.enable = value;
}

void CDROM::setInterruptFlagRegister(uint8_t value) {
    logMessage(format("INTF [W]: %#x", value));
    if (value & 0x40) {
        clearParameters();
        updateStatusRegister();
    }
    if (!interruptQueue.empty()) {
        interruptQueue.pop();
    }
}

void CDROM::execute(uint8_t value) {
    clearInterruptQueue();
    clearResponse();
    switch (value) {
        case 0x01: {
            operationGetstat();
            break;
        }
        case 0x02: {
            operationSetloc();
            break;
        }
        case 0x06: {
            operationReadN();
            break;
        }
        case 0x0E: {
            operationSetmode();
            break;
        }
        case 0x15: {
            operationSeekL();
            break;
        }
        case 0x19: {
            operationTest();
            break;
        }
        case 0x1A: {
            operationGetID();
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
    logMessage(format("STATUS [R]: %#x", status._value));
    return status._value;
}

uint8_t CDROM::getInterruptFlagRegister() const {
    uint8_t flags = 0b11100000;
    if (!interruptQueue.empty()) {
        flags |= interruptQueue.front() & 0x7;
    }
    logMessage(format("INTF [R]: %#x", flags));
    return flags;
}

uint8_t CDROM::getReponse() {
    uint8_t value = 0;
    if (!response.empty()) {
        value = response.front();
        response.pop();
        updateStatusRegister();
    }
    logMessage(format("RESPONSE [R]: %#x", value));
    return value;
}

void CDROM::clearParameters() {
    queue<uint8_t> empty;
    swap(parameters, empty);
}

void CDROM::clearInterruptQueue() {
    queue<CDROMInterruptNumber> empty;
    swap(interruptQueue, empty);
}

void CDROM::clearResponse() {
    queue<uint8_t> empty;
    swap(response, empty);
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

uint8_t CDROM::popParameter() {
    uint8_t value = 0;
    if (!parameters.empty()) {
        value = parameters.front();
        parameters.pop();
        updateStatusRegister();
    }
    return value;
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
            interruptQueue.push(INT3);
            break;
        }
        default: {
            printError("Unhandled CDROM operation test with subfunction: %#x", subfunction);
        }
    }
    logMessage(format("CMD Test [%#x]", subfunction));
}

/*
Getstat - Command 01h --> INT3(stat)
*/
void CDROM::operationGetstat() {
    logMessage("CMD Getstat");
    pushResponse(statusCode._value);
    interruptQueue.push(INT3);
}

/*
GetID - Command 1Ah --> INT3(stat) --> INT2/5 (stat,flags,type,atip,"SCEx")

1st byte: stat  (as usually, but with bit3 same as bit7 in 2nd byte)
2nd byte: flags (bit7=denied, bit4=audio... or reportedly import, uh?)
  bit7: Licensed (0=Licensed Data CD, 1=Denied Data CD or Audio CD)
  bit6: Missing  (0=Disk Present, 1=Disk Missing)
  bit4: Audio CD (0=Data CD, 1=Audio CD) (always 0 when Modchip installed)
3rd byte: Disk type (from TOC Point=A0h) (eg. 00h=Audio or Mode1, 20h=Mode2)
4th byte: Usually 00h (or 8bit ATIP from Point=C0h, if session info exists)
  that 8bit ATIP value is taken form the middle 8bit of the 24bit ATIP value
5th-8th byte: SCEx region (eg. ASCII "SCEE" = Europe) (0,0,0,0 = Unlicensed)
*/
void CDROM::operationGetID() {
    // TODO: timming issue?
    // pushResponse(statusCode._value);
    // interruptQueue.push(INT3);

    pushResponse(statusCode._value);
    pushResponse(0x00);
    pushResponse(0x20);
    pushResponse(0x00);
    pushResponse('S');
    pushResponse('C');
    pushResponse('E');
    pushResponse('A');
    interruptQueue.push(INT2);
}

/*
Setloc - Command 02h,amm,ass,asect --> INT3(stat)
*/
void CDROM::operationSetloc() {
    uint8_t minute = decimalFromBCDEncodedInt(popParameter());
    uint8_t second = decimalFromBCDEncodedInt(popParameter());
    uint8_t sector = decimalFromBCDEncodedInt(popParameter());

    seekSector = (minute * SecondsPerMinute * SectorsPerSecond) + (second * SectorsPerSecond) + sector;

    pushResponse(statusCode._value);
    interruptQueue.push(INT3);

    logMessage(format("CMD Setloc(%d, %d, %d)", minute, second, sector));
}

/*
SeekL - Command 15h --> INT3(stat) --> INT2(stat)
*/
void CDROM::operationSeekL() {
    readSector = seekSector;

    pushResponse(statusCode._value);
    interruptQueue.push(INT3);

    statusCode.setState(CDROMState::Seeking);

    pushResponse(statusCode._value);
    interruptQueue.push(INT2);

    logMessage("CMD SeekL");
}

/*
Setmode - Command 0Eh,mode --> INT3(stat)
*/
void CDROM::operationSetmode() {
    uint8_t value = popParameter();
    mode._value = value;

    pushResponse(statusCode._value);
    interruptQueue.push(INT3);

    logMessage("CMD Setmode");
}

/*
ReadN - Command 06h --> INT3(stat) --> INT1(stat) --> datablock
*/
void CDROM::operationReadN() {
    readSector = seekSector;

    statusCode.setState(CDROMState::Reading);

    pushResponse(statusCode._value);
    interruptQueue.push(INT3);

    logMessage("CMD ReadN");
}

void CDROM::logMessage(std::string message) const {
    if (!logActivity) {
        return;
    }
    printWarning("  CD-ROM: %s", message.c_str());
}

void CDROM::loadCDROMImageFile(string filePath) {
    statusCode.setShellOpen(false);
    if (filePath.empty()) {
        return;
    }
    image.open(filePath);
}
