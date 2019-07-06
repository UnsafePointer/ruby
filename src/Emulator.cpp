#include "Emulator.hpp"
#include "TestRunner.hpp"

using namespace std;

uint32_t systemClocksPerSecond = 33868800;
uint32_t videoSystemClocksPerScanline = 3413;
uint32_t scanlinesPerFrame = 263;

Emulator::Emulator() {
    cop0 = make_unique<COP0>();
    bios = make_unique<BIOS>();
    ram = make_unique<RAM>();
    gpu = make_unique<GPU>();
    dma = make_unique<DMA>(ram, gpu);
    scratchpad = make_unique<Scratchpad>();
    cdrom = make_unique<CDROM>();
    interruptController = make_unique<InterruptController>(cop0);
    expansion1 = make_unique<Expansion1>();
    interconnect = make_unique<Interconnect>(bios, ram, gpu, dma, scratchpad, cdrom, interruptController, expansion1);
    cpu = make_unique<CPU>(interconnect, cop0);
}

Emulator::~Emulator() {}

CPU* Emulator::getCPU() {
    return cpu.get();
}

void Emulator::emulateFrame() {
    TestRunner *testRunner = TestRunner::getInstance();
    uint32_t systemClockStep = 21;
    uint32_t videoSystemClockStep = systemClockStep*11/7;
    uint32_t totalSystemClocksThisFrame = 0;
    uint32_t videoSystemClocksScanlineCounter = 0;
    uint32_t totalScanlines = 0;
    while (totalSystemClocksThisFrame < systemClocksPerSecond) {
        for (uint32_t i = 0; i < systemClockStep / 3; i++) {
            testRunner->checkTTY();
            if (!cpu->executeNextInstruction()) {
                testRunner->setup();
            }
            totalSystemClocksThisFrame++;
        }
        videoSystemClocksScanlineCounter += videoSystemClockStep;
        if (videoSystemClocksScanlineCounter >= videoSystemClocksPerScanline) {
            totalScanlines++;
            videoSystemClocksScanlineCounter = 0;
        }
        if (totalScanlines >= scanlinesPerFrame) {
            interruptController->trigger(VBLANK);
            totalScanlines = 0;
        }
    }
}
