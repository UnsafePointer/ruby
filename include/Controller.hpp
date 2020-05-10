#pragma once
#include <cstdint>
#include <memory>
#include "Logger.hpp"
#include "DigitalController.hpp"
#include "InterruptController.hpp"

enum Device : uint8_t {
    NoDevice = 0x0,
    ControllerDevice = 0x01,
    MemoryCardDevice = 0x81
};

/*
1F80104Ah JOY_CTRL (R/W) (usually 1003h,3003h,0000h)
0     TX Enable (TXEN)  (0=Disable, 1=Enable)
1     /JOYn Output      (0=High, 1=Low/Select) (/JOYn as defined in Bit13)
2     RX Enable (RXEN)  (0=Normal, when /JOYn=Low, 1=Force Enable Once)
3     Unknown? (read/write-able) (for SIO, this would be TX Output Level)
4     Acknowledge       (0=No change, 1=Reset JOY_STAT.Bits 3,9)          (W)
5     Unknown? (read/write-able) (for SIO, this would be RTS Output Level)
6     Reset             (0=No change, 1=Reset most JOY_registers to zero) (W)
7     Not used             (always zero) (unlike SIO, no matter of FACTOR)
8-9   RX Interrupt Mode    (0..3 = IRQ when RX FIFO contains 1,2,4,8 bytes)
10    TX Interrupt Enable  (0=Disable, 1=Enable) ;when JOY_STAT.0-or-2 ;Ready
11    RX Interrupt Enable  (0=Disable, 1=Enable) ;when N bytes in RX FIFO
12    ACK Interrupt Enable (0=Disable, 1=Enable) ;when JOY_STAT.7  ;/ACK=LOW
13    Desired Slot Number  (0=/JOY1, 1=/JOY2) (set to LOW when Bit1=1)
14-15 Not used             (always zero)
*/
union JoypadControl {
    struct {
        uint16_t txEnable : 1;
        uint16_t joyOutput : 1;
        uint16_t rxEnable : 1;
        uint16_t unknown : 1;
        uint16_t acknowledge : 1;
        uint16_t unknown2 : 1;
        uint16_t reset : 1;
        uint16_t unused : 1;
        uint16_t rxInterruptMode : 2;
        uint16_t txInterruptEnable : 1;
        uint16_t rxInterruptEnable : 1;
        uint16_t ackInterruptEnable : 1;
        uint16_t desiredSlotNumber : 1;
        uint16_t unused2 : 2;
    };

    uint16_t _value;

    JoypadControl() : _value(0x0) {}
};

/*
1F801048h JOY_MODE (R/W) (usually 000Dh, ie. 8bit, no parity, MUL1)
0-1   Baudrate Reload Factor (1=MUL1, 2=MUL16, 3=MUL64) (or 0=MUL1, too)
2-3   Character Length       (0=5bits, 1=6bits, 2=7bits, 3=8bits)
4     Parity Enable          (0=No, 1=Enable)
5     Parity Type            (0=Even, 1=Odd) (seems to be vice-versa...?)
6-7   Unknown (always zero)
8     CLK Output Polarity    (0=Normal:High=Idle, 1=Inverse:Low=Idle)
9-15  Unknown (always zero)
*/
union JoypadMode {
    struct {
        uint16_t baudRateReloadFactor : 2;
        uint16_t characterLength : 2;
        uint16_t parityEnable : 1;
        uint16_t parityType : 1;
        uint16_t unknown : 2;
        uint16_t CLKOutputPolarity : 1;
        uint16_t unknown2 : 7;
    };

    uint16_t _value;

    JoypadMode() : _value(0x0) {}
};

/*
1F801040h JOY_RX_DATA (R)
0-7   Received Data      (1st RX FIFO entry) (oldest entry)
8-15  Preview            (2nd RX FIFO entry)
16-23 Preview            (3rd RX FIFO entry)
24-31 Preview            (4th RX FIFO entry) (5th..8th cannot be previewed)
*/
union JoypadRxData {
    struct {
        uint32_t receivedData : 8;
        uint32_t preview1 : 8;
        uint32_t preview2 : 8;
        uint32_t preview3 : 8;
    };

    uint32_t _value;

    JoypadRxData() : _value(0x0) {}
};

/*
1F801044h JOY_STAT (R)
0     TX Ready Flag 1   (1=Ready/Started)
1     RX FIFO Not Empty (0=Empty, 1=Not Empty)
2     TX Ready Flag 2   (1=Ready/Finished)
3     RX Parity Error   (0=No, 1=Error; Wrong Parity, when enabled)  (sticky)
4     Unknown (zero)    (unlike SIO, this isn't RX FIFO Overrun flag)
5     Unknown (zero)    (for SIO this would be RX Bad Stop Bit)
6     Unknown (zero)    (for SIO this would be RX Input Level AFTER Stop bit)
7     /ACK Input Level  (0=High, 1=Low)
8     Unknown (zero)    (for SIO this would be CTS Input Level)
9     Interrupt Request (0=None, 1=IRQ7) (See JOY_CTRL.Bit4,10-12)   (sticky)
10    Unknown (always zero)
11-31 Baudrate Timer    (21bit timer, decrementing at 33MHz)
*/
union JoypadStatus {
    struct {
        uint32_t txReadyFlag1 : 1;
        uint32_t rxFifoNotEmpty : 1;
        uint32_t txReadyFlag2 : 1;
        uint32_t rxParityError : 1;
        uint32_t unknown1 : 1;
        uint32_t unknown2 : 1;
        uint32_t unknown3 : 1;
        uint32_t ackInputLevel : 1;
        uint32_t unknown4 : 1;
        uint32_t interruptRequest : 1;
        uint32_t unknown5 : 1;
        uint32_t baudRateTimer : 21;
    };

    uint32_t _value;

    JoypadStatus() : _value(0x0) {}
};

/*
1F801040h JOY_TX_DATA (W)
0-7   Data to be sent
8-31  Not used
*/
union JoypadTxData {
    struct {
        uint32_t toBeSentData : 8;
        uint32_t unused : 24;
    };

    uint32_t _value;

    JoypadTxData() : _value(0x0) {}
};

class Controller {
    Logger logger;

    std::unique_ptr<InterruptController> &interruptController;
    bool shouldCount;
    uint32_t counter;

    std::unique_ptr<DigitalController> digitalController;
    Device currentDevice;

    JoypadControl control;
    uint16_t joypadBaud;
    JoypadMode mode;
    JoypadRxData rxData;
    JoypadStatus status;
    JoypadTxData txData;

    void setControlRegister(uint16_t value);
    void setJoypadBaudRegister(uint16_t value);
    void setModeRegister(uint16_t value);
    void setTxDataRegister(uint8_t value);

    uint8_t getRxDataRegister();
    uint32_t getStatusRegister();
    uint16_t getControlRegister();
public:
    Controller(LogLevel logLevel, std::unique_ptr<InterruptController> &interruptController);
    ~Controller();

    void step(uint32_t steps);
    void updateInput(SDL_Event event);

    template <typename T>
    inline T load(uint32_t offset);
    template <typename T>
    inline void store(uint32_t offset, T value);
};
