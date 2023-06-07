#include "emulate.c"

void testHelperFunctions() {
    printf("getMask\n");
    printf("%llx\n", getMask(3)); // 7
    printf("%llx\n", getMask(8)); // ff
    printf("%llx\n", getMask(32)); // 32/4 = 8fs
    printf("%llx\n", getMask(64)); // 16fs

    printf("negate\n");
    printf("%llx\n", negate(1)); // 16fs
    printf("%llx\n", negate(-1)); // 1
    printf("%llx\n", negate(16)); // 15 fs, 0
    printf("%llx\n", negate(-16)); // 10

    printf("getMaskBetween\n");
    printf("%llx\n", getMaskBetween(64, 0)); // 16fs
    printf("%llx\n", getMaskBetween(5, 1)); // 2+4+8+16 = 16+14 = 1e
    printf("%llx\n", getMaskBetween(16, 8)); // ff00
    printf("%llx\n", getMaskBetween(64, 32)); // 8 fs, 8 0s
    printf("%llx\n", getMaskBetween(0, 0)); // 0

    printf("getResultMask\n");
    printf("%llx\n", getResultMask(0)); // 8 fs
    printf("%llx\n", getResultMask(1)); // 16 fs

    printf("getBit\n");
    printf("%i\n", getBit(0b0010, 0)); // 0
    printf("%i\n", getBit(0b0010, 1)); // 1
    printf("%i\n", getBit(0b0010, 2)); // 0
    printf("%i\n", getBit(0b0010, 3)); // 0
    printf("%i\n", getBit(0b0010, 63)); // 0
    printf("%i\n", getBit(-1, 63)); // 1

    printf("getSignBit\n");
    printf("%i\n", getSignBit(1, 0)); // 0
    printf("%i\n", getSignBit(1, 1)); // 0
    printf("%i\n", getSignBit(-1, 0)); // 1
    printf("%i\n", getSignBit(-1, 1)); // 1

    printf("isMaskEquals\n");
    printf("%i\n", isMaskEquals(0b0011, 0b0001, 0b0001)); // 1
    printf("%i\n", isMaskEquals(0b0011, 0b1111, 0b0011)); // 1
    printf("%i\n", isMaskEquals(0b1001, 0b0101, 0b0101)); // 0
    printf("%i\n", isMaskEquals(0b0110, 0b0101, 0b0100)); // 1
    printf("%i\n", isMaskEquals(0b0110, 0b0101, 0b0101)); // 0
    printf("%i\n", isMaskEquals(0b0110, 0b0101, 0b0101)); // 0
    printf("%i\n", isMaskEquals(0b0110, 0b0001, 0b0101)); // 0

    printf("getSignedPart\n");
    printf("%llx\n", getSignedPart(0b0101, 0, 0)); // 0
    printf("%llx\n", getSignedPart(0b0100, 0, 1)); // 0
    printf("%llx\n", getSignedPart(0b0100, 0, 2)); // 0
    printf("%llx\n", getSignedPart(0b0100, 0, 3)); // -4
    printf("%llx\n", getSignedPart(0b0100, 0, 4)); // 4
    printf("%llx\n", getSignedPart(0xffff0000, 0, 16)); // 0
    printf("%llx\n", getSignedPart(0xffff0000, 0, 17)); // 12 fs, 0000

    printf("arithmeticShiftRight\n");
    printf("%llx\n", arithmeticShiftRight(0xf0000000, 4, 0)); // ff000000
    printf("%llx\n", arithmeticShiftRight(0xf0000000, 8, 0)); // fff00000
    printf("%llx\n", arithmeticShiftRight(0xf000000000000000, 4, 1)); // ff, 14 0s
    printf("%llx\n", arithmeticShiftRight(0xf000000000000000, 4, 0)); // 0
    printf("%llx\n", arithmeticShiftRight(0xf000000000000000, 8, 1)); // fff, 13 0s
    printf("%llx\n", arithmeticShiftRight(0xf000000000000000, 8, 0)); // 0
    printf("%llx\n", arithmeticShiftRight(0x0f000000, 4, 0)); // f00000
    printf("%llx\n", arithmeticShiftRight(0x0f000000, 8, 0)); // f0000
    printf("%llx\n", arithmeticShiftRight(0x0f00000000000000, 4, 1)); // f, 13 0s
    printf("%llx\n", arithmeticShiftRight(0x0f00000000000000, 4, 0)); // 0
    printf("%llx\n", arithmeticShiftRight(0x0f00000000000000, 8, 1)); // f, 12 0s
    printf("%llx\n", arithmeticShiftRight(0x0f00000000000000, 8, 0)); // 0

    printf("rotateRight\n");
    printf("%llx\n", rotateRight(0xf0000000, 4, 0)); // f000000
    printf("%llx\n", rotateRight(0xf0000000, 8, 0)); // f00000
    printf("%llx\n", rotateRight(0xf000000000000000, 4, 1)); // f, 14 0s
    printf("%llx\n", rotateRight(0xf000000000000000, 4, 0)); // 0
    printf("%llx\n", rotateRight(0xf000000000000000, 8, 1)); // f, 13 0s
    printf("%llx\n", rotateRight(0xf000000000000000, 8, 0)); // 0
    printf("%llx\n", rotateRight(0x0f00000f, 4, 0)); // f0f00000
    printf("%llx\n", rotateRight(0x0f00000f, 8, 0)); // f0f0000
    printf("%llx\n", rotateRight(0x0f0000000000000f, 4, 1)); // f0f, 13 0s
    printf("%llx\n", rotateRight(0x0f0000000000000f, 4, 0)); // f0000000
    printf("%llx\n", rotateRight(0x0f0000000000000f, 8, 1)); // f0f, 12 0s
    printf("%llx\n", rotateRight(0x0f0000000000000f, 8, 0)); // f000000

    machine.instruction = 0xffff00f0;

    printf("getInstructionPart\n");
    printf("%llx\n", getInstructionPart(0, 8)); // f0
    printf("%llx\n", getInstructionPart(0, 16)); // f0
    printf("%llx\n", getInstructionPart(8, 8)); // 0

    printf("getInstructionPartSigned\n");
    printf("%llx\n", getInstructionPartSigned(0, 8)); // 15fs 0
    printf("%llx\n", getInstructionPartSigned(0, 16)); // f0
    printf("%llx\n", getInstructionPartSigned(8, 8)); // 0

    machine.memory[0] = 0xff;
    machine.memory[1] = 0x00;
    machine.memory[2] = 0xff;
    machine.memory[3] = 0x00;
    machine.memory[4] = 0xff;
    machine.memory[5] = 0x00;
    machine.memory[6] = 0xff;
    machine.memory[7] = 0x00;

    printf("loadFromMemory\n");
    printf("%llx\n", loadFromMemory(0, 0)); // ff00ff
    printf("%llx\n", loadFromMemory(1, 0)); // ff00ff00
    printf("%llx\n", loadFromMemory(0, 1)); // ff00ff00ff00ff

    printf("storeInMemory\n");
    storeInMemory(0x00ff00ff, 0, 0);
    printf("%02x%02x%02x%02x\n", machine.memory[3], machine.memory[2], machine.memory[1], machine.memory[0]);
    storeInMemory(0x00ff00ffffff0000, 0, 0);
    printf("%02x%02x%02x%02x", machine.memory[7], machine.memory[6], machine.memory[5], machine.memory[4]);
    printf("%02x%02x%02x%02x\n", machine.memory[3], machine.memory[2], machine.memory[1], machine.memory[0]);
}

void testExecuteArithmetic() {
    resetMachine();
    executeArithmeticInstruction(0, -2, -1, 1, 0);
    printf("%llx\n", machine.registers[0]); // 3
    printf("N %i\n", machine.state.N); // 3
    printf("Z %i\n", machine.state.Z); // 3
    printf("C %i\n", machine.state.C); // 3
    printf("V %i\n", machine.state.V); // 3
}

void testExecuteDPImmediateArithmetic() {
    resetMachine();
    uint64_t sf, opc, opi, rd, sh, imm12, rn, op0;
    sf = 0;
    opc = 0;
    op0 = 0b100;
    opi = 0b010;
    sh = 0;
    imm12 = 2;
    rd = 0;
    rn = 1;
    machine.registers[rn] = 1;
    machine.registers[rd] = 0;
    machine.instruction = (sf << 31) | (opc << 29) | (op0 << 26) | (opi << 23) | (sh << 22) | (imm12 << 10) | (rn << 5) | (rd);
    printf("instruction %lld\n", machine.instruction);
    executeDPImmediate();
    printf("rd %lld\n", machine.registers[rd]); // 3
}

void testExecuteDPImmediateWideMove() {
    resetMachine();
    uint64_t sf, opc, opi, rd, hw, imm16;
    machine.instruction = 0;
}

void testExecuteDPRegister() {

}

void testExecuteLoadOrStore() {
    resetMachine();
    uint64_t sf, U, L , xn, rt, xm, imm12, I, isRegOffset, isSDT, simm9, simm19;
    sf = 0;
    U = 0;
    L = 0;
    xn = 0;
    rt = 0;
    xm = 0;
    imm12 = 0;
    I = 0;
    isRegOffset = 0;
    isSDT = 0;
    simm9 = 0;
    simm19 = 0;
    machine.instruction = 0;
}

void testExecuteBranch() {

}