#pragma once
#include <cstdint>

enum Timer0SyncMode {
    PauseDuringHblank = 0,
    ResetToZeroAtHblank = 1,
    ResetToZeroAtHblankAndPauseOutsideHblank = 2,
    PauseUntilHblankThenSwitchToFreeRun = 3
};

enum Timer1SyncMode {
    PauseDuringVblank = 0,
    ResetToZeroAtVblank = 1,
    ResetToZeroAtHblankAndPauseOutsideVblank = 2,
    PauseUntilVblankThenSwitchToFreeRun = 3
};

enum Timer2SyncMode {
    StopCounterAtCurrentValue,
    FreeRun
};

enum Timer0ClockSource {
    T0SystemClock,
    DotClock
};

enum Timer1ClockSource {
    T1SystemClock,
    Hblank
};

enum Timer2ClockSource {
    T2SystemClock,
    SystemClockByEight
};

enum TimerResetCounter {
    AfterOverflow = 0,
    AfterTarget = 1
};

/*
1F801104h+N*10h - Timer 0..2 Counter Mode (R/W)
0     Synchronization Enable (0=Free Run, 1=Synchronize via Bit1-2)
1-2   Synchronization Mode   (0-3, see lists below)
        Synchronization Modes for Counter 0:
        0 = Pause counter during Hblank(s)
        1 = Reset counter to 0000h at Hblank(s)
        2 = Reset counter to 0000h at Hblank(s) and pause outside of Hblank
        3 = Pause until Hblank occurs once, then switch to Free Run
        Synchronization Modes for Counter 1:
        Same as above, but using Vblank instead of Hblank
        Synchronization Modes for Counter 2:
        0 or 3 = Stop counter at current value (forever, no h/v-blank start)
        1 or 2 = Free Run (same as when Synchronization Disabled)
3     Reset counter to 0000h  (0=After Counter=FFFFh, 1=After Counter=Target)
4     IRQ when Counter=Target (0=Disable, 1=Enable)
5     IRQ when Counter=FFFFh  (0=Disable, 1=Enable)
6     IRQ Once/Repeat Mode    (0=One-shot, 1=Repeatedly)
7     IRQ Pulse/Toggle Mode   (0=Short Bit10=0 Pulse, 1=Toggle Bit10 on/off)
8-9   Clock Source (0-3, see list below)
        Counter 0:  0 or 2 = System Clock,  1 or 3 = Dotclock
        Counter 1:  0 or 2 = System Clock,  1 or 3 = Hblank
        Counter 2:  0 or 1 = System Clock,  2 or 3 = System Clock/8
10    Interrupt Request       (0=Yes, 1=No) (Set after Writing)    (W=1) (R)
11    Reached Target Value    (0=No, 1=Yes) (Reset after Reading)        (R)
12    Reached FFFFh Value     (0=No, 1=Yes) (Reset after Reading)        (R)
13-15 Unknown (seems to be always zero)
16-31 Garbage (next opcode)
*/
union TimerCounterMode {
    struct {
        uint32_t syncEnable : 1;
        uint32_t _syncMode : 2;
        uint32_t _resetCounter : 1;
        uint32_t IRQWhenTarget : 1;
        uint32_t IRQWhenOverflow : 1;
        uint32_t IRQOnceOrRepeatMode : 1;
        uint32_t IRQPulseOrToggle : 1;
        uint32_t _clockSource : 2;
        uint32_t IRQ : 1;
        uint32_t rearchedTarget : 1;
        uint32_t rearchedOverflow : 1;
        uint32_t unknown : 3;
        uint32_t garbage : 16;
    };

    uint32_t _value;

    TimerCounterMode() : _value(0) {}

    TimerResetCounter timerResetCounter() { return TimerResetCounter(_resetCounter); }

    Timer0SyncMode timer0SyncMode() { return Timer0SyncMode(_syncMode); }
    Timer1SyncMode timer1SyncMode() { return Timer1SyncMode(_syncMode); }
    Timer2SyncMode timer2SyncMode() {
        if (_syncMode == 0 || _syncMode == 3) {
            return StopCounterAtCurrentValue;
        } else {
            return FreeRun;
        }
    }

    Timer0ClockSource timer0ClockSource() {
        if (_clockSource == 0 || _clockSource == 3) {
            return T0SystemClock;
        } else {
            return DotClock;
        }
    }
    Timer1ClockSource timer1ClockSource() {
        if (_clockSource == 0 || _clockSource == 3) {
            return T1SystemClock;
        } else {
            return Hblank;
        }
    }
    Timer2ClockSource timer2ClockSource() {
        if (_clockSource == 0 || _clockSource == 3) {
            return T2SystemClock;
        } else {
            return SystemClockByEight;
        }
    }
};

/*
1F801100h+N*10h - Timer 0..2 Current Counter Value (R/W)
0-15  Current Counter value (incrementing)
16-31 Garbage
*/
union TimerCounterValue {
    struct {
        uint32_t value : 16;
        uint32_t garbage : 16;
    };

    uint32_t _value;

    TimerCounterValue() : _value(0) {}
};

/*
1F801108h+N*10h - Timer 0..2 Counter Target Value (R/W)
0-15  Counter Target value
16-31 Garbage
*/
union TimerCounterTarget {
    struct {
        uint32_t target : 16;
        uint32_t garbage : 16;
    };

    uint32_t _value;

    TimerCounterTarget() : _value(0) {}
};

class Timer {
protected:
    TimerCounterValue counterValue;
    TimerCounterMode counterMode;
    TimerCounterTarget counterTarget;

    uint32_t counter;
public:
    Timer();
    ~Timer();

    virtual void step(uint32_t cycles) = 0;
    uint32_t counterValueRegister() const;
    uint32_t counterModeRegister() const;
    uint32_t counterTargetRegister() const;
    void setCounterValueRegister(uint32_t value);
    virtual void setCounterModeRegister(uint32_t value) = 0;
    void setCounterTargetRegister(uint32_t value);
    void checkInterruptRequest();

    template <typename T>
    inline T load(uint32_t offset);
    template <typename T>
    inline void store(uint32_t offset, T value);
};

class Timer0 : public Timer {
public:
    void step(uint32_t cycles) override;
    void setCounterModeRegister(uint32_t value) override;
};
class Timer1 : public Timer {
public:
    void step(uint32_t cycles) override;
    void setCounterModeRegister(uint32_t value) override;
};
class Timer2 : public Timer {
public:
    void step(uint32_t cycles) override;
    void setCounterModeRegister(uint32_t value) override;
};
