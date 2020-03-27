#pragma once
#include <cstdint>
#include <queue>
#include <memory>
#include <filesystem>
#include "InterruptController.hpp"
#include "CDImage.hpp"
#include "Logger.hpp"

/*
INT0   No response received (no interrupt request)
INT1   Received SECOND (or further) response to ReadS/ReadN (and Play+Report)
INT2   Received SECOND response (to various commands)
INT3   Received FIRST response (to any command)
INT4   DataEnd (when Play/Forward reaches end of disk) (maybe also for Read?)
INT5   Received error-code (in FIRST or SECOND response)
        INT5 also occurs on SECOND GetID response, on unlicensed disks
        INT5 also occurs when opening the drive door (even if no command
        was sent, ie. even if no read-command or other command is active)
INT6   N/A
INT7   N/A
*/
enum CDROMInterruptNumber : uint8_t {
    INT0 = 0,
    INT1 = 1,
    INT2 = 2,
    INT3 = 3,
    INT4 = 4,
    INT5 = 5,
    INT6 = 6,
    INT7 = 7,
};

enum XAADCPMFifoStatus : uint8_t {
    XAADCPMFifoEmpty = 0,
    XAADCPMFifoNotEmpty = 1,
};

enum ParameterFifoEmptyStatus : uint8_t {
    ParameterFifoNotEmpty = 0,
    ParameterFifoEmpty = 1,
};

enum ParameterFifoFullStatus : uint8_t {
    ParameterFifoFull = 0,
    ParameterFifoNotFull = 1,
};

enum ResponseFifoStatus : uint8_t {
    ResponseFifoEmpty = 0,
    ResponseFifoNotEmpty = 1,
};

enum DataFifoStatus : uint8_t {
    DataFifoEmpty = 0,
    DataFifoNotEmpty = 1,
};

enum TransmissionStatus : uint8_t {
    TransmissionNotBusy = 0,
    TransmissionBusy = 1,
};

/*
1F801800h - Index/Status Register (Bit0-1 R/W) (Bit2-7 Read Only)
0-1 Index   Port 1F801801h-1F801803h index (0..3 = Index0..Index3)   (R/W)
2   ADPBUSY XA-ADPCM fifo empty  (0=Empty) ;set when playing XA-ADPCM sound
3   PRMEMPT Parameter fifo empty (1=Empty) ;triggered before writing 1st byte
4   PRMWRDY Parameter fifo full  (0=Full)  ;triggered after writing 16 bytes
5   RSLRRDY Response fifo empty  (0=Empty) ;triggered after reading LAST byte
6   DRQSTS  Data fifo empty      (0=Empty) ;triggered after reading LAST byte
7   BUSYSTS Command/parameter transmission busy  (1=Busy)
*/
union CDROMStatus {
    struct {
        uint8_t index : 2;
        uint8_t _XAADCPMFifoEmpty : 1;
        uint8_t _parameterFifoEmpty : 1;
        uint8_t _parameterFifoFull : 1;
        uint8_t _responseFifoEmpty : 1;
        uint8_t _dataFifoEmpty : 1;
        uint8_t _transmissionBusy : 1;
    };

    uint8_t _value;

    CDROMStatus() : _value(0x18) {}

    XAADCPMFifoStatus XAADCPMFifoEmpty() { return XAADCPMFifoStatus(_XAADCPMFifoEmpty); }

    ParameterFifoEmptyStatus parameterFifoEmpty() { return ParameterFifoEmptyStatus(_parameterFifoEmpty); }
    void setParameterFifoEmpty(ParameterFifoEmptyStatus status) { _parameterFifoEmpty = status; }

    ParameterFifoFullStatus parameterFifoFull() { return ParameterFifoFullStatus(_parameterFifoFull); }
    void setParameterFifoFull(ParameterFifoFullStatus status) { _parameterFifoFull = status; }

    ResponseFifoStatus responseFifoEmpty() { return ResponseFifoStatus(_responseFifoEmpty); }
    void setResponseFifoEmpty(ResponseFifoStatus status) { _responseFifoEmpty = status; }

    DataFifoStatus dataFifoEmpty() { return DataFifoStatus(_dataFifoEmpty); }
    void setDataFifoEmpty(DataFifoStatus status) { _dataFifoEmpty = status; }

    TransmissionStatus transmissionBusy() { return TransmissionStatus(_transmissionBusy); }
};

/*
1F801803h.Index0 - Interrupt Enable Register (R)
1F801803h.Index2 - Interrupt Enable Register (R) (Mirror)
0-4  Interrupt Enable Bits (usually all set, ie. 1Fh=Enable All IRQs)
5-7  Unknown/unused (write: should be zero) (read: usually all bits set)
*/
union CDROMInterrupt {
    struct {
        uint8_t enable : 4;
        uint8_t unknown : 4;
    };

    uint8_t _value;

    CDROMInterrupt() : _value(0) {}
};

enum CDROMState : uint8_t {
    Unknown = 0,
    Reading = 5,
    Seeking = 6,
    Playing = 7,
};

/*
Status code (stat)
0  Error         Invalid Command/parameters (followed by Error Byte)
1  Spindle Motor (0=Motor off, or in spin-up phase, 1=Motor on)
2  SeekError     (0=Okay, 1=Seek error)     (followed by Error Byte)
3  IdError       (0=Okay, 1=GetID denied) (also set when Setmode.Bit4=1)
4  ShellOpen     Once shell open (0=Closed, 1=Is/was Open)
5  Read          Reading data sectors  ;/set until after Seek completion)
6  Seek          Seeking               ; at a time (ie. Read/Play won't get
7  Play          Playing CD-DA         ;\only ONE of these bits can be set
*/
union CDROMStatusCode {
    struct {
        uint8_t error : 1;
        uint8_t spindleMotor : 1;
        uint8_t seekError : 1;
        uint8_t getIdError : 1;
        uint8_t shellOpen : 1;
        uint8_t read : 1;
        uint8_t seek : 1;
        uint8_t play : 1;
    };

    uint8_t _value;

    CDROMStatusCode() : _value(0x10) {}

    void setState(CDROMState state) {
        error = false;
        spindleMotor = true;
        seekError = false;
        getIdError = false;
        _value &= ~(0xE0); // Always clear status: read, seek and play
        if (state == Unknown) {
            return;
        }
        _value |= (1 << state);
    }

    void setShellOpen(bool open) {
        shellOpen = open;
        if (!shellOpen) {
            setState(CDROMState::Unknown);
        }
    }
};

enum CDROMModeSectorSize : uint8_t {
    DataOnly800h = 0,
    WholeSector924h = 1,
};

enum CDROMModeSpeed : uint8_t {
    Normal = 0,
    Double = 1,
};

/*
Setmode
0   CDDA        (0=Off, 1=Allow to Read CD-DA Sectors; ignore missing EDC)
1   AutoPause   (0=Off, 1=Auto Pause upon End of Track) ;for Audio Play
2   Report      (0=Off, 1=Enable Report-Interrupts for Audio Play)
3   XA-Filter   (0=Off, 1=Process only XA-ADPCM sectors that match Setfilter)
4   Ignore Bit  (0=Normal, 1=Ignore Sector Size and Setloc position)
5   Sector Size (0=800h=DataOnly, 1=924h=WholeSectorExceptSyncBytes)
6   XA-ADPCM    (0=Off, 1=Send XA-ADPCM sectors to SPU Audio Input)
7   Speed       (0=Normal speed, 1=Double speed)
*/
union CDROMMode {
    struct {
        uint8_t CDDASectorsReadEnable : 1;
        uint8_t endOfTrackAutoPauseEnable : 1;
        uint8_t reportInterruptsForAudioPlayEnable : 1;
        uint8_t XAFilterEnable : 1;
        uint8_t unknown : 1;
        uint8_t _sectorSize : 1;
        uint8_t XAADPCMEnable : 1;
        uint8_t _speed : 1;
    };

    uint8_t _value;

    CDROMMode() : _value(0x0) {}

    CDROMModeSectorSize sectorSize() { return CDROMModeSectorSize(_sectorSize); }
    CDROMModeSpeed speed() { return CDROMModeSpeed(_speed); }
};

class CDROM {
    Logger logger;
    std::unique_ptr<InterruptController> &interruptController;
    CDImage image;

    CDROMStatus status;
    CDROMInterrupt interrupt;
    CDROMStatusCode statusCode;
    CDROMMode mode;

    std::queue<uint8_t> parameters;
    std::queue<uint8_t> response;
    std::queue<CDROMInterruptNumber> interruptQueue;
    uint32_t seekSector;
    uint32_t readSector;
    uint32_t counter;
    CDSector currentSector;
    std::vector<uint32_t> readBuffer;
    uint32_t readBufferIndex;

    uint8_t leftCDToLeftSPUVolume;
    uint8_t leftCDToRightSPUVolume;
    uint8_t rightCDToLeftSPUVolume;

    void setStatusRegister(uint8_t value);
    void setInterruptRegister(uint8_t value);
    void setInterruptFlagRegister(uint8_t value);
    void setRequestRegister(uint8_t value);
    void setAudioVolumeLeftCDToLeftSPURegister(uint8_t value);
    void setAudioVolumeLeftCDToRightSPURegister(uint8_t value);
    void setAudioVolumeRightCDToLeftSPURegister(uint8_t value);
    void execute(uint8_t value);

    uint8_t getStatusRegister() const;
    uint8_t getInterruptFlagRegister() const;
    uint8_t getReponse();
    uint8_t getInterruptRegister() const;

    void clearParameters();
    void clearInterruptQueue();
    void clearResponse();
    void pushParameter(uint8_t value);
    void pushResponse(uint8_t value);
    uint8_t popParameter();
    bool isReadBufferEmpty();

    void updateStatusRegister();
    uint8_t loadByteFromReadBuffer();

/*
Command          Parameters      Response(s)
00h -            -               INT5(11h,40h)  ;reportedly "Sync" uh?
01h Getstat      -               INT3(stat)
02h Setloc     E amm,ass,asect   INT3(stat)
03h Play       E (track)         INT3(stat), optional INT1(report bytes)
04h Forward    E -               INT3(stat), optional INT1(report bytes)
05h Backward   E -               INT3(stat), optional INT1(report bytes)
06h ReadN      E -               INT3(stat), INT1(stat), datablock
07h MotorOn    E -               INT3(stat), INT2(stat)
08h Stop       E -               INT3(stat), INT2(stat)
09h Pause      E -               INT3(stat), INT2(stat)
0Ah Init         -               INT3(late-stat), INT2(stat)
0Bh Mute       E -               INT3(stat)
0Ch Demute     E -               INT3(stat)
0Dh Setfilter  E file,channel    INT3(stat)
0Eh Setmode      mode            INT3(stat)
0Fh Getparam     -               INT3(stat,mode,null,file,channel)
10h GetlocL    E -               INT3(amm,ass,asect,mode,file,channel,sm,ci)
11h GetlocP    E -               INT3(track,index,mm,ss,sect,amm,ass,asect)
12h SetSession E session         INT3(stat), INT2(stat)
13h GetTN      E -               INT3(stat,first,last)  ;BCD
14h GetTD      E track (BCD)     INT3(stat,mm,ss)       ;BCD
15h SeekL      E -               INT3(stat), INT2(stat)  ;\use prior Setloc
16h SeekP      E -               INT3(stat), INT2(stat)  ;/to set target
17h -            -               INT5(11h,40h)  ;reportedly "SetClock" uh?
18h -            -               INT5(11h,40h)  ;reportedly "GetClock" uh?
19h Test         sub_function    depends on sub_function (see below)
1Ah GetID      E -               INT3(stat), INT2/5(stat,flg,typ,atip,"SCEx")
1Bh ReadS      E?-               INT3(stat), INT1(stat), datablock
1Ch Reset        -               INT3(stat), Delay
1Dh GetQ       E adr,point       INT3(stat), INT2(10bytesSubQ,peak_lo) ;\not
1Eh ReadTOC      -               INT3(late-stat), INT2(stat)           ;/vC0
1Fh VideoCD      sub,a,b,c,d,e   INT3(stat,a,b,c,d,e)   ;<-- SCPH-5903 only
1Fh..4Fh -       -               INT5(11h,40h)  ;-Unused/invalid
50h Secret 1     -               INT5(11h,40h)  ;\
51h Secret 2     "Licensed by"   INT5(11h,40h)  ;
52h Secret 3     "Sony"          INT5(11h,40h)  ; Secret Unlock Commands
53h Secret 4     "Computer"      INT5(11h,40h)  ; (not in version vC0, and,
54h Secret 5     "Entertainment" INT5(11h,40h)  ; nonfunctional in japan)
55h Secret 6     "<region>"      INT5(11h,40h)  ;
56h Secret 7     -               INT5(11h,40h)  ;/
57h SecretLock   -               INT5(11h,40h)  ;-Secret Lock Command
58h..5Fh Crash   -               Crashes the HC05 (jumps into a data area)
6Fh..FFh -       -               INT5(11h,40h)  ;-Unused/invalid
*/
    void operationTest();

    void operationGetstat();
    void operationGetID();
    void operationSetloc();
    void operationSeekL();
    void operationSetmode();
    void operationReadN();
    void operationPause();
    void operationInit();
    void operationDemute();
    void operationGetTN();
    void operationReadS();

    void handleUnsupportedOperation(uint8_t operation);
public:
    CDROM(LogLevel logLevel, std::unique_ptr<InterruptController> &interruptController);
    ~CDROM();

    void step();

    template <typename T>
    inline T load(uint32_t offset);
    template <typename T>
    inline void store(uint32_t offset, T value);

    void loadCDROMImageFile(std::filesystem::path filePath);
    uint32_t loadWordFromReadBuffer();
};
