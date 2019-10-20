#pragma once
#include <cstdint>
#include "Logger.hpp"

enum SPUControlRAMTransferMode {
    Stop = 0,
    ManualWrite = 1,
    DMAWrite = 2,
    DMARead = 3,
};

// 1F801DAAh - SPU Control Register (SPUCNT)
// 15    SPU Enable              (0=Off, 1=On)       (Don't care for CD Audio)
// 14    Mute SPU                (0=Mute, 1=Unmute)  (Don't care for CD Audio)
// 13-10 Noise Frequency Shift   (0..0Fh = Low .. High Frequency)
// 9-8   Noise Frequency Step    (0..03h = Step "4,5,6,7")
// 7     Reverb Master Enable    (0=Disabled, 1=Enabled)
// 6     IRQ9 Enable (0=Disabled/Acknowledge, 1=Enabled; only when Bit15=1)
// 5-4   Sound RAM Transfer Mode (0=Stop, 1=ManualWrite, 2=DMAwrite, 3=DMAread)
// 3     External Audio Reverb   (0=Off, 1=On)
// 2     CD Audio Reverb         (0=Off, 1=On) (for CD-DA and XA-ADPCM)
// 1     External Audio Enable   (0=Off, 1=On)
// 0     CD Audio Enable         (0=Off, 1=On) (for CD-DA and XA-ADPCM)
union SPUControl {
    struct {
       uint16_t CDAudioEnable : 1;
       uint16_t externalAudioEnable : 1;
       uint16_t CDAudioReverb : 1;
       uint16_t externalAudioReverb : 1;
       uint16_t _RAMTransferMode : 2;
       uint16_t IRQEnable : 1;
       uint16_t reverbMasterEnable : 1;
       uint16_t noiseFrequencyStep : 2;
       uint16_t noiseFrequencyShift : 4;
       uint16_t mute : 1;
       uint16_t enable : 1;
    };

    uint16_t value;

    SPUControl() : value() {}

    SPUControlRAMTransferMode RAMTransferMode() { return SPUControlRAMTransferMode(_RAMTransferMode); }
};

class SPU {
    Logger logger;

    SPUControl control;

    uint16_t controlRegister() const;
    void setControlRegister(uint16_t value);
public:
    SPU(LogLevel logLevel);
    ~SPU();

    template <typename T>
    inline T load(uint32_t offset) const;
    template <typename T>
    inline void store(uint32_t offset, T value);
};
