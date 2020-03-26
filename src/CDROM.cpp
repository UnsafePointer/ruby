#include "CDROM.hpp"
#include "Helpers.hpp"
#include "ConfigurationManager.hpp"

using namespace std;

/*
INT1 Rate
Command                Average   Min       Max
Read (single speed)    006e1cdh  00686dah..0072732h
Read (double speed)    0036cd2h  00322dfh..003ab2bh
The INT1 rate needs to be precise for CD-DA and CD-XA Audio streaming, exact clock cycle values
should be: SystemClock*930h/4/44100Hz for Single Speed (and half as much for Double Speed)
(the "Average" values are AVERAGE values, not exact values).
*/
const uint32_t SystemClocksPerCDROMInt1SingleSpeed=2352;
const uint32_t SystemClocksPerCDROMInt1DoubleSpeed=2352/2;

CDROM::CDROM(LogLevel logLevel, unique_ptr<InterruptController> &interruptController) : logger(logLevel, "  CD-ROM: "), interruptController(interruptController), image(), status(), interrupt(), statusCode(), mode(), parameters(), response(), interruptQueue(), seekSector(), readSector(), counter(), currentSector(), readBuffer(), readBufferIndex(), leftCDToLeftSPUVolume(), leftCDToRightSPUVolume(), rightCDToLeftSPUVolume() {

}

CDROM::~CDROM() {

}

void CDROM::step() {
    if (!interruptQueue.empty()) {
        if ((interrupt.enable & 0x7) & (interruptQueue.front() & 0x7)) {
            interruptController->trigger(InterruptRequestNumber::CDROMIRQ);
        }
    }
    if ((statusCode.play || statusCode.read)) {
        counter++;
        if (counter >= SystemClocksPerCDROMInt1DoubleSpeed) {
            interruptQueue.push(INT1);
            pushResponse(statusCode._value);
            counter = 0;

            currentSector = image.readSector(readSector);
            readSector++;
        }
    }
}

void CDROM::setStatusRegister(uint8_t value) {
    logger.logMessage("STATUS [W]: %#x", value);
    status.index = value & 0x3;
}

void CDROM::setInterruptRegister(uint8_t value) {
    logger.logMessage("INTE [W]: %#x", value);
    interrupt.enable = value;
}

void CDROM::setInterruptFlagRegister(uint8_t value) {
    logger.logMessage("INTF [W]: %#x", value);
    if (value & 0x40) {
        clearParameters();
        updateStatusRegister();
    }
    if (!interruptQueue.empty()) {
        interruptQueue.pop();
    }
}

void CDROM::setRequestRegister(uint8_t value) {
    logger.logMessage("REQ [W]: %#x", value);
    if (value & 0x80) {
        readBuffer.clear();
        if (isReadBufferEmpty()) {
            CDROMModeSectorSize sectorSize = mode.sectorSize();
            if (sectorSize == DataOnly800h) {
                copy(&currentSector.data[0], &currentSector.data[0x800], back_inserter(readBuffer));
            } else { // WholeSector924h
                copy(&currentSector.header[0], &currentSector.ECC[276], back_inserter(readBuffer));
            }
            readBufferIndex = 0;
            status.setDataFifoEmpty(DataFifoNotEmpty);
        } else {
            readBufferIndex = 0;
            status.setDataFifoEmpty(DataFifoEmpty);
        }
    }
}

void CDROM::setAudioVolumeLeftCDToLeftSPURegister(uint8_t value) {
    leftCDToLeftSPUVolume = value;
}

void CDROM::setAudioVolumeLeftCDToRightSPURegister(uint8_t value) {
    leftCDToRightSPUVolume = value;
}

void CDROM::setAudioVolumeRightCDToLeftSPURegister(uint8_t value) {
    rightCDToLeftSPUVolume = value;
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
        case 0x09: {
            operationPause();
            break;
        }
        case 0x0A: {
            operationInit();
            break;
        }
        case 0x0C: {
            operationDemute();
            break;
        }
        case 0x0E: {
            operationSetmode();
            break;
        }
        case 0x13: {
            operationGetTN();
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
            handleUnsupportedOperation(value);
        }
    }
    clearParameters();
    updateStatusRegister();
}

uint8_t CDROM::getStatusRegister() const {
    logger.logMessage("STATUS [R]: %#x", status._value);
    return status._value;
}

uint8_t CDROM::getInterruptFlagRegister() const {
    uint8_t flags = 0b11100000;
    if (!interruptQueue.empty()) {
        flags |= interruptQueue.front() & 0x7;
    }
    logger.logMessage("INTF [R]: %#x", flags);
    return flags;
}

uint8_t CDROM::getReponse() {
    uint8_t value = 0;
    if (!response.empty()) {
        value = response.front();
        response.pop();
        updateStatusRegister();
    }
    logger.logMessage("RESPONSE [R]: %#x", value);
    return value;
}

uint8_t CDROM::getInterruptRegister() const {
    logger.logMessage("INTE [R]: %#x", interrupt.enable);
    return interrupt.enable;
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
        logger.logError("Parameter FIFO full");
    }
    parameters.push(value);
    updateStatusRegister();
}

void CDROM::pushResponse(uint8_t value) {
    if (response.size() >= 16) {
        logger.logError("Response FIFO full");
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

bool CDROM::isReadBufferEmpty() {
    if (readBuffer.empty()) {
        return true;
    }

    CDROMModeSectorSize sectorSize = mode.sectorSize();
    if (sectorSize == DataOnly800h) {
        return readBufferIndex >= 0x800;
    } else { // WholeSector924h
        return readBufferIndex >= 0x924;
    }
}

void CDROM::updateStatusRegister() {
    status.setParameterFifoEmpty(parameters.empty() ? ParameterFifoEmpty : ParameterFifoNotEmpty);
    status.setParameterFifoFull(parameters.size() >= 16 ? ParameterFifoFull : ParameterFifoNotFull);
    status.setResponseFifoEmpty(response.empty() ? ResponseFifoEmpty : ResponseFifoNotEmpty);
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
            logger.logError("Unhandled CDROM operation test with subfunction: %#x", subfunction);
        }
    }
    logger.logMessage("CMD Test [%#x]", subfunction);
}

/*
Getstat - Command 01h --> INT3(stat)
*/
void CDROM::operationGetstat() {
    logger.logMessage("CMD Getstat");
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

    logger.logMessage("CMD GetID");
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

    logger.logMessage("CMD Setloc(%d, %d, %d)", minute, second, sector);
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

    logger.logMessage("CMD SeekL");
}

/*
Setmode - Command 0Eh,mode --> INT3(stat)
*/
void CDROM::operationSetmode() {
    uint8_t value = popParameter();
    mode._value = value;

    pushResponse(statusCode._value);
    interruptQueue.push(INT3);

    logger.logMessage("CMD Setmode");
}

/*
ReadN - Command 06h --> INT3(stat) --> INT1(stat) --> datablock
*/
void CDROM::operationReadN() {
    readSector = seekSector;

    statusCode.setState(CDROMState::Reading);

    pushResponse(statusCode._value);
    interruptQueue.push(INT3);

    logger.logMessage("CMD ReadN");
}

/*
Pause - Command 09h --> INT3(stat) --> INT2(stat)
*/
void CDROM::operationPause() {
    pushResponse(statusCode._value);
    interruptQueue.push(INT3);

    statusCode.setState(CDROMState::Unknown);
    pushResponse(statusCode._value);
    interruptQueue.push(INT2);

    logger.logMessage("CMD Pause");
}

/*
Init - Command 0Ah --> INT3(stat) --> INT2(stat)
*/
void CDROM::operationInit() {
    pushResponse(statusCode._value);
    interruptQueue.push(INT3);

    statusCode.spindleMotor = 1;
    statusCode.setState(CDROMState::Unknown);

    mode._value = 0x0;

    pushResponse(statusCode._value);
    interruptQueue.push(INT2);

    logger.logMessage("CMD Init");
}

/*
Demute - Command 0Ch --> INT3(stat)
*/
void CDROM::operationDemute() {
    // TODO: track mute state

    pushResponse(statusCode._value);
    interruptQueue.push(INT3);

    logger.logMessage("CMD Demute");
}

/*
GetTN - Command 13h --> INT3(stat,first,last) ;BCD
*/
void CDROM::operationGetTN() {
    // TODO: CUE parser

    pushResponse(statusCode._value);
    pushResponse(0x1);
    pushResponse(0x1);
    interruptQueue.push(INT3);

    logger.logMessage("CMD GetTN");
}

void CDROM::handleUnsupportedOperation(uint8_t operation) {
    logger.logWarning("Unhandled CDROM operation value: %#x", operation);
    interruptQueue.push(INT5);
    pushResponse(0x11);
    pushResponse(0x10);
}

void CDROM::loadCDROMImageFile(std::filesystem::path filePath) {
    if (!std::filesystem::exists(filePath)) {
        return;
    }
    statusCode.setShellOpen(false);
    image.open(filePath);
}

uint8_t CDROM::loadByteFromReadBuffer() {
    if (readBuffer.empty()) {
        return 0;
    }

    /*
    The PSX hardware allows to read 800h-byte or 924h-byte sectors, indexed as [000h..7FFh] or
    [000h..923h], when trying to read further bytes, then the PSX will repeat the byte at
    index [800h-8] or [924h-4] as padding value.
    */
    CDROMModeSectorSize sectorSize = mode.sectorSize();
    if (sectorSize == DataOnly800h && readBufferIndex >= 0x800) {
        return readBuffer[0x800 - 0x8];
    } else if (sectorSize == WholeSector924h && readBufferIndex >= 0x924) {
        return readBuffer[0x924 - 0x4];
    }

    uint8_t value = readBuffer[readBufferIndex];
    readBufferIndex++;

    status.setDataFifoEmpty(isReadBufferEmpty() ? DataFifoEmpty : DataFifoNotEmpty);

    return value;
}

uint32_t CDROM::loadWordFromReadBuffer() {
    uint32_t value = 0;
    for (uint8_t i = 0; i < sizeof(uint32_t); i++) {
        uint8_t byte = loadByteFromReadBuffer();
        value |= (((uint32_t)byte) << (i * 8));
    }
    return value;
}
