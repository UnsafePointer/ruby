#pragma once
#include <cstdint>
#include <iostream>
#include "Logger.hpp"
#include "Output.hpp"

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

enum SPUStatusCaptureBufferWrite {
    FirstHalf = 0,
    SecondHalf = 1,
};

// 1F801DAEh - SPU Status Register (SPUSTAT) (R)
// 15-12 Unknown/Unused (seems to be usually zero)
// 11    Writing to First/Second half of Capture Buffers (0=First, 1=Second)
// 10    Data Transfer Busy Flag          (0=Ready, 1=Busy)
// 9     Data Transfer DMA Read Request   (0=No, 1=Yes)
// 8     Data Transfer DMA Write Request  (0=No, 1=Yes)
// 7     Data Transfer DMA Read/Write Request ;seems to be same as SPUCNT.Bit5
// 6     IRQ9 Flag                        (0=No, 1=Interrupt Request)
// 5-0   Current SPU Mode   (same as SPUCNT.Bit5-0, but, applied a bit delayed)
union SPUStatus {
    struct {
        uint16_t currentMode : 5;
        uint16_t IRQFlag : 1;
        uint16_t dataTransferDMAReadWriteRequest : 1;
        uint16_t dataTransferDMAWWriteequest : 1;
        uint16_t dataTransferDMAWReadequest : 1;
        uint16_t dataTransferBusyFlag : 1;
        uint16_t _captureBufferWrite : 1;
        uint16_t unknown : 4;
    };

    uint16_t value;

    SPUStatus() : value() {}

    SPUStatusCaptureBufferWrite captureBufferWrite() { return SPUStatusCaptureBufferWrite(_captureBufferWrite); }
};

enum SPUVoiceKeyOffValue {
    NoChange = 0,
    StartRelease = 1,
};

// 1F801D8Ch - Voice 0..23 Key OFF (Start Release) (KOFF) (W)
// 0-23  Voice 0..23 Off (0=No change, 1=Start Release)
// 24-31 Not used
struct SPUVoiceKeyOff {
    uint32_t value;

    SPUVoiceKeyOff() : value() {}

    SPUVoiceKeyOffValue voiceKeyOffValueAtIndex(uint8_t voiceIndex) {
        if (voiceIndex >= 24) {
            std::cout << format("Unsupported index %d for KOFF SPU register", voiceIndex) << std::endl;
            exit(1);
        }
        return SPUVoiceKeyOffValue((value >> voiceIndex) & 0x1);
    }
};

class SPU {
    Logger logger;

    SPUControl control;
    SPUStatus status;
    SPUVoiceKeyOff voiceKeyOff;

    uint16_t controlRegister() const;
    void setControlRegister(uint16_t value);
    uint16_t statusRegister() const;
    void setVoiceKeyOffRegister(uint32_t value);
public:
    SPU(LogLevel logLevel);
    ~SPU();

    template <typename T>
    inline T load(uint32_t offset) const;
    template <typename T>
    inline void store(uint32_t offset, T value);
};
