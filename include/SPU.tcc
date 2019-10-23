#include "SPU.hpp"

template <typename T>
inline T SPU::load(uint32_t offset) const {
    static_assert(std::is_same<T, uint8_t>() || std::is_same<T, uint16_t>() || std::is_same<T, uint32_t>(), "Invalid type");
    if (offset >= 0 && offset <= 0x17f) {
        logger.logMessage("Unhandled Sound Processing Unit voice register read");
        return 0;
    }
    if (offset >= 0x1c0 && offset <= 0x1ff) {
        logger.logMessage("Unhandled Sound Processing Unit reverb configuration area read");
        return 0;
    }
    switch (offset) {
        case 0x1ae: {
            return statusRegister();
        }
        case 0x1aa: {
            return controlRegister();
        }
        case 0x188: {
            if (sizeof(T) != 2) {
                logger.logError("Unsupported KON read with size: %d", sizeof(T));
            }
            uint16_t lower = voiceKeyOn.value & 0xFFFF;
            return lower;
        }
        case 0x18a: {
            if (sizeof(T) != 2) {
                logger.logError("Unsupported KON read with size: %d", sizeof(T));
            }
            uint16_t upper = voiceKeyOn.value >> 16;
            return upper;
        }
        case 0x18c: {
            if (sizeof(T) != 2) {
                logger.logError("Unsupported KOFF read with size: %d", sizeof(T));
            }
            uint16_t lower = voiceKeyOff.value & 0xFFFF;
            return lower;
        }
        case 0x18e: {
            if (sizeof(T) != 2) {
                logger.logError("Unsupported KOFF read with size: %d", sizeof(T));
            }
            uint16_t upper = voiceKeyOff.value >> 16;
            return upper;
        }
        case 0x1ac: {
            if (sizeof(T) != 2) {
                logger.logError("Unsupported RAM Data Transfer Control read with size: %d", sizeof(T));
            }
            return RAMDataTransferControlRegister();
        }
        default: {
            logger.logError("Unhandled Sound Processing Unit read at offset: %#x, of size: %d", offset, sizeof(T));
            return 0;
        }
    }
}

template <typename T>
inline void SPU::store(uint32_t offset, T value) {
    static_assert(std::is_same<T, uint8_t>() || std::is_same<T, uint16_t>() || std::is_same<T, uint32_t>(), "Invalid type");
    if (offset >= 0 && offset <= 0x17f) {
        logger.logMessage("Unhandled Sound Processing Unit voice register write");
        return;
    }
    if (offset >= 0x1c0 && offset <= 0x1ff) {
        logger.logMessage("Unhandled Sound Processing Unit reverb configuration area read");
        return;
    }
    switch (offset) {
        case 0x180: {
            logger.logMessage("Unhandled Sound Processing Unit main volume left write");
            break;
        }
        case 0x182: {
            logger.logMessage("Unhandled Sound Processing Unit main volume right write");
            break;
        }
        case 0x184: {
            logger.logMessage("Unhandled Sound Processing Unit reverb volume output left write");
            break;
        }
        case 0x186: {
            logger.logMessage("Unhandled Sound Processing Unit reverb volume output right write");
            break;
        }
        case 0x1aa: {
            if (sizeof(T) != 2) {
                logger.logError("Unsupported SPUCNT write with size: %d", sizeof(T));
            }
            setControlRegister(value);
            break;
        }
        case 0x18c: {
            if (sizeof(T) != 2) {
                logger.logError("Unsupported KOFF write with size: %d", sizeof(T));
            }
            uint32_t upper = (voiceKeyOff.value >> 16) << 16;
            uint32_t toWrite = upper | value;
            setVoiceKeyOffRegister(toWrite);
            break;
        }
        case 0x18e: {
            if (sizeof(T) != 2) {
                logger.logError("Unsupported KOFF write with size: %d", sizeof(T));
            }
            uint32_t lower = voiceKeyOff.value & 0xFFFF;
            uint32_t toWrite = (value << 16) | lower;
            setVoiceKeyOffRegister(toWrite);
            break;
        }
        case 0x190: {
            if (sizeof(T) != 2) {
                logger.logError("Unsupported PMON write with size: %d", sizeof(T));
            }
            uint32_t upper = (pitchModulationEnableFlags.value >> 16) << 16;
            uint32_t toWrite = upper | value;
            setPitchModulationEnableFlagsRegister(toWrite);
            break;
        }
        case 0x192: {
            if (sizeof(T) != 2) {
                logger.logError("Unsupported PMON write with size: %d", sizeof(T));
            }
            uint32_t lower = pitchModulationEnableFlags.value & 0xFFFF;
            uint32_t toWrite = (value << 16) | lower;
            setPitchModulationEnableFlagsRegister(toWrite);
            break;
        }
        case 0x194: {
            if (sizeof(T) != 2) {
                logger.logError("Unsupported NON write with size: %d", sizeof(T));
            }
            uint32_t upper = (noiseModeEnable.value >> 16) << 16;
            uint32_t toWrite = upper | value;
            setNoiseModeEnableRegister(toWrite);
            break;
        }
        case 0x196: {
            if (sizeof(T) != 2) {
                logger.logError("Unsupported NON write with size: %d", sizeof(T));
            }
            uint32_t lower = noiseModeEnable.value & 0xFFFF;
            uint32_t toWrite = (value << 16) | lower;
            setNoiseModeEnableRegister(toWrite);
            break;
        }
        case 0x198: {
            if (sizeof(T) != 2) {
                logger.logError("Unsupported EON write with size: %d", sizeof(T));
            }
            uint32_t upper = (reverbMode.value >> 16) << 16;
            uint32_t toWrite = upper | value;
            setReverbModeRegister(toWrite);
            break;
        }
        case 0x19a: {
            if (sizeof(T) != 2) {
                logger.logError("Unsupported EON write with size: %d", sizeof(T));
            }
            uint32_t lower = reverbMode.value & 0xFFFF;
            uint32_t toWrite = (value << 16) | lower;
            setReverbModeRegister(toWrite);
            break;
        }
        case 0x1b0: {
            if (sizeof(T) != 2) {
                logger.logError("Unsupported CD Audio Input Volume write with size: %d", sizeof(T));
            }
            setCDAudioInputVolumeLeft(value);
            break;
        }
        case 0x1b2: {
            if (sizeof(T) != 2) {
                logger.logError("Unsupported CD Audio Input Volume write with size: %d", sizeof(T));
            }
            setCDAudioInputVolumeRight(value);
            break;
        }
        case 0x1b4: {
            if (sizeof(T) != 2) {
                logger.logError("Unsupported External Audio Input Volume write with size: %d", sizeof(T));
            }
            setExternalAudioInputVolumeLeft(value);
            break;
        }
        case 0x1b6: {
            if (sizeof(T) != 2) {
                logger.logError("Unsupported External Audio Input Volume write with size: %d", sizeof(T));
            }
            setExternalAudioInputVolumeRight(value);
            break;
        }
        case 0x1ac: {
            if (sizeof(T) != 2) {
                logger.logError("Unsupported Sound RAM Data Transfer Control write with size: %d", sizeof(T));
            }
            setRAMDataTransferControlRegister(value);
            break;
        }
        case 0x1a6: {
            if (sizeof(T) != 2) {
                logger.logError("Unsupported Sound RAM Data Transfer Address write with size: %d", sizeof(T));
            }
            setRAMDataTransferAddressRegister(value);
            break;
        }
        case 0x1a8: {
            logger.logWarning("Unhandled Sound RAM Data Transfer FIFO write");
            break;
        }
        case 0x188: {
            if (sizeof(T) != 2) {
                logger.logError("Unsupported KON write with size: %d", sizeof(T));
            }
            uint32_t upper = (voiceKeyOn.value >> 16) << 16;
            uint32_t toWrite = upper | value;
            setVoiceKeyOnRegister(toWrite);
            break;
        }
        case 0x18A: {
            if (sizeof(T) != 2) {
                logger.logError("Unsupported KON write with size: %d", sizeof(T));
            }
            uint32_t lower = voiceKeyOn.value & 0xFFFF;
            uint32_t toWrite = (value << 16) | lower;
            setVoiceKeyOnRegister(toWrite);
            break;
        }
        case 0x1a2: {
            if (sizeof(T) != 2) {
                logger.logError("Unsupported Sound RAM Reverb Work Area Start Address write with size: %d", sizeof(T));
            }
            logger.logWarning("Unhandled Sound RAM Reverb Work Area Start Address");
            break;
        }
        default: {
            logger.logError("Unhandled Sound Processing Unit write at offset: %#x, of size: %d", offset, sizeof(T));
            return;
        }
    }
}
