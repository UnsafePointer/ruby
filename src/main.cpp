#include "CPU.hpp"

int main() {
    CPU cpu = CPU();
    while (true) {
        cpu.executeNextInstruction();
    }
}
