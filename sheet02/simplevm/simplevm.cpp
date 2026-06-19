#include <algorithm>
#include <cstdint>
#include <iostream>

extern void division_by_zero();
int32_t next();

namespace simplevm {
int32_t execute() {
    int32_t regs[4]{};
    while (true) {
        int32_t opcode = next();
        if (opcode == 0) {
            return regs[0];
        } else if (opcode == 10) {
            int32_t ireg = next();
            int32_t iimm = next();
            regs[ireg] = iimm;
        } else if (opcode == 20) {
            int32_t ireg = next();
            regs[0] = regs[ireg];
        } else if (opcode == 21) {
            int32_t ireg = next();
            regs[ireg] = regs[0];
        } else if (opcode == 22) {
            std::swap(regs[0], regs[1]);
        } else if (opcode == 50) {
            regs[0] = static_cast<uint32_t>(regs[0]) + regs[1];
        } else if (opcode == 51) {
            regs[0] = static_cast<uint32_t>(regs[0]) - regs[1];
        } else if (opcode == 53) {
            regs[0] = static_cast<uint32_t>(regs[0]) * regs[1];
        } else if (opcode == 54) {
            if (regs[1] == 0) {
                division_by_zero();
                return 0;
            }
            int32_t quotient = regs[0] / regs[1];
            int32_t remainder = regs[0] % regs[1];
            regs[0] = quotient;
            regs[1] = remainder;
        }
    }
    return regs[0];
}
} // namespace simplevm
