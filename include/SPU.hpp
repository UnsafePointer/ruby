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
    NoChangeKeyOff = 0,
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

enum SPUVoiceKeyOnValue {
    NoChangeKeyOn = 0,
    StartAttackDecaySustain = 1,
};

// 1F801D88h - Voice 0..23 Key ON (Start Attack/Decay/Sustain) (KON) (W)
// 0-23  Voice 0..23 On  (0=No change, 1=Start Attack/Decay/Sustain)
// 24-31 Not used
struct SPUVoiceKeyOn {
    uint32_t value;

    SPUVoiceKeyOn() : value() {}

    SPUVoiceKeyOnValue voiceKeyOffValueAtIndex(uint8_t voiceIndex) {
        if (voiceIndex >= 24) {
            std::cout << format("Unsupported index %d for KON SPU register", voiceIndex) << std::endl;
            exit(1);
        }
        return SPUVoiceKeyOnValue((value >> voiceIndex) & 0x1);
    }
};

enum SPUPitchModulationEnableFlagValue {
    NormalFrequency = 0,
    ModulateByPreviousVoice = 1,
};

// 1F801D90h - Voice 0..23 Pitch Modulation Enable Flags (PMON)
// Pitch modulation allows to generate "Frequency Sweep" effects by mis-using the amplitude from channel (x-1) as pitch factor for channel (x).
// 0     Unknown... Unused?
// 1-23  Flags for Voice 1..23 (0=Normal, 1=Modulate by Voice 0..22)
// 24-31 Not used
struct SPUPitchModulationEnableFlags {
    uint32_t value;

    SPUPitchModulationEnableFlags() : value() {}

    SPUPitchModulationEnableFlagValue pitchModulationEnableFlagValueAtIndex(uint8_t voiceIndex) {
        if (voiceIndex >= 24 || voiceIndex < 1) {
            std::cout << format("Unsupported index %d for KOFF SPU register", voiceIndex) << std::endl;
            exit(1);
        }
        return SPUPitchModulationEnableFlagValue((value >> voiceIndex) & 0x1);
    }
};

enum SPUNoiseModeEnableValue {
    ADPCM = 0,
    Noise = 1,
};

// 1F801D94h - Voice 0..23 Noise mode enable (NON)
// 0-23  Voice 0..23 Noise (0=ADPCM, 1=Noise)
// 24-31 Not used
struct SPUNoiseModeEnable {
    uint32_t value;

    SPUNoiseModeEnable() : value() {}

    SPUNoiseModeEnableValue noiseModeEnableValueAtIndex(uint8_t voiceIndex) {
        if (voiceIndex >= 24) {
            std::cout << format("Unsupported index %d for KOFF SPU register", voiceIndex) << std::endl;
            exit(1);
        }
        return SPUNoiseModeEnableValue((value >> voiceIndex) & 0x1);
    }
};

enum SPUReverbModeValue {
    ToMixer = 0,
    ToMixerAndToReverb = 1,
};

// 1F801D98h - Voice 0..23 Reverb mode aka Echo On (EON) (R/W)
// 0-23  Voice 0..23 Destination (0=To Mixer, 1=To Mixer and to Reverb)
// 24-31 Not used
struct SPUReverbMode {
    uint32_t value;

    SPUReverbMode() : value() {}

    SPUReverbModeValue reverbModeValueAtIndex(uint8_t voiceIndex) {
        if (voiceIndex >= 24) {
            std::cout << format("Unsupported index %d for KOFF SPU register", voiceIndex) << std::endl;
            exit(1);
        }
        return SPUReverbModeValue((value >> voiceIndex) & 0x1);
    }
};

// 1F801DB0h - CD Audio Input Volume (for normal CD-DA, and compressed XA-ADPCM)
// 0-15  Volume Left   (-8000h..+7FFFh)
// 16-31 Volume Right  (-8000h..+7FFFh)
union SPUCDAudioInputVolume {
    struct {
        uint32_t left : 16;
        uint32_t right : 16;
    };

    uint32_t value;

    SPUCDAudioInputVolume() : value() {}
};

// 1F801DB4h - External Audio Input Volume
// 0-15  Volume Left   (-8000h..+7FFFh)
// 16-31 Volume Right  (-8000h..+7FFFh)
union SPUExternalAudoInputVolume {
    struct {
        uint32_t left : 16;
        uint32_t right : 16;
    };

    uint32_t value;

    SPUExternalAudoInputVolume() : value() {}
};

enum SPURAMDataTransferControlType {
    Fill,
    NormalTransfer,
    Rep2,
    Rep4,
    Rep8,
};

// 1F801DACh - Sound RAM Data Transfer Control (should be 0004h)
// 15-4   Unknown/no effect?                       (should be zero)
// 3-1    Sound RAM Data Transfer Type (see below) (should be 2)
// 0      Unknown/no effect?                       (should be zero)
// The Transfer Type selects how data is forwarded from Fifo to SPU RAM:
// __Transfer Type___Halfwords in Fifo________Halfwords written to SPU RAM__
// 0,1,6,7  Fill     A,B,C,D,E,F,G,H,...,X    X,X,X,X,X,X,X,X,...
// 2        Normal   A,B,C,D,E,F,G,H,...,X    A,B,C,D,E,F,G,H,...
// 3        Rep2     A,B,C,D,E,F,G,H,...,X    A,A,C,C,E,E,G,G,...
// 4        Rep4     A,B,C,D,E,F,G,H,...,X    A,A,A,A,E,E,E,E,...
// 5        Rep8     A,B,C,D,E,F,G,H,...,X    H,H,H,H,H,H,H,H,...
union SPURAMDataTransferControl {
    struct {
        uint16_t unknown1 : 1;
        uint16_t _type : 3;
        uint16_t unknown2 : 12;
    };
    uint16_t value;

    SPURAMDataTransferControl() : value() {}

    SPURAMDataTransferControlType type() {
        switch (_type) {
            case 0:
            case 1:
            case 6:
            case 7:
                return SPURAMDataTransferControlType::Fill;
            case 3:
                return SPURAMDataTransferControlType::Rep2;
            case 4:
                return SPURAMDataTransferControlType::Rep4;
            case 5:
                return SPURAMDataTransferControlType::Rep8;
            case 2:
                return SPURAMDataTransferControlType::NormalTransfer;
        }
    }
};


// 1F801DA6h - Sound RAM Data Transfer Address
// 15-0  Address in sound buffer divided by eight
union SPURAMDataTransferAddress {
    struct {
        uint16_t address : 16;
    };

    uint16_t value;

    SPURAMDataTransferAddress() : value() {}
};

class SPU {
    Logger logger;

    SPUControl control;
    SPUStatus status;
    SPUVoiceKeyOff voiceKeyOff;
    SPUPitchModulationEnableFlags pitchModulationEnableFlags;
    SPUNoiseModeEnable noiseModeEnable;
    SPUReverbMode reverbMode;
    SPUCDAudioInputVolume CDAudioInputVolume;
    SPUExternalAudoInputVolume externalAudioInputVolume;
    SPURAMDataTransferControl RAMDataTransferControl;
    SPURAMDataTransferAddress RAMDataTransferAddress;
    SPUVoiceKeyOn voiceKeyOn;

    uint16_t controlRegister() const;
    void setControlRegister(uint16_t value);
    uint16_t statusRegister() const;
    void setVoiceKeyOffRegister(uint32_t value);
    void setPitchModulationEnableFlagsRegister(uint32_t value);
    uint32_t pitchModulationEnableFlagsRegister() const;
    void setNoiseModeEnableRegister(uint32_t value);
    uint32_t noiseModeEnableRegister() const;
    void setReverbModeRegister(uint32_t value);
    uint32_t reverbModeRegister() const;
    void setCDAudioInputVolumeLeft(uint16_t value);
    uint16_t CDAudioInputVolumeLeft() const;
    void setCDAudioInputVolumeRight(uint16_t value);
    uint16_t CDAudioInputVolumeRight() const;
    void setExternalAudioInputVolumeLeft(uint16_t value);
    uint16_t externalAudioInputVolumeLeft() const;
    void setExternalAudioInputVolumeRight(uint16_t value);
    uint16_t externalAudioInputVolumeRight() const;
    void setRAMDataTransferControlRegister(uint16_t value);
    uint16_t RAMDataTransferControlRegister() const;
    void setRAMDataTransferAddressRegister(uint16_t value);
    uint32_t voiceKeyOnRegister() const;
    void setVoiceKeyOnRegister(uint32_t value);
public:
    SPU(LogLevel logLevel);
    ~SPU();

    template <typename T>
    inline T load(uint32_t offset) const;
    template <typename T>
    inline void store(uint32_t offset, T value);
};
