#include "read_binary_file.c"

// Returns mask of length `bits` set to 1
uint64_t getMask(uint64_t bits) {
    if (bits >= 64) {
        return -1;
    } else {
        return (1LL << bits) - 1;
    }
}

// Returns mask set to 1 from bit `left` - 1 to bit `right` (inclusive)
uint64_t getMaskBetween(uint64_t left, uint64_t right) {
    return getMask(left) ^ getMask(right);
}

// Returns mask to be applied to the result of an operation
uint64_t getResultMask(bool sf) {
    return getMask(sf ? 64 : 32);
}

// Returns the given `bit` of the given `value`
bool getBit(uint64_t value, uint64_t bit) {
    return (value >> bit) & 1;
}

// Returns the most significant bit of the given value
bool getSignBit(uint64_t value, bool sf) {
    return getBit(value, sf ? 63 : 31);
}

// Returns whether the masked value is equal to the target value
bool isMaskEquals(uint64_t value, uint64_t mask, uint64_t equalTo) {
    return (value & mask) == equalTo;
}

// Returns the given portion of the given value
uint64_t getPart(uint64_t value, uint64_t littleEnd, uint64_t length) {
    return (value >> littleEnd) & getMask(length);
}

// Returns the given portion of the given `value` as a 64-bit signed value
uint64_t getSignedPart(uint64_t value, uint64_t littleEnd, uint64_t length) {
    uint64_t part = getPart(value, littleEnd, length);
    if (getBit(part, length - 1)) { // Signed
        return part | getMaskBetween(64, length);
    } else { // Not signed
        return part;
    }
}

// Returns the given portion of the current instruction
uint64_t getInstructionPart(uint64_t littleEnd, uint64_t length) {
    return getPart(machine.instruction, littleEnd, length);
}

// Returns the given signed portion of the current instruction
uint64_t getInstructionPartSigned(uint64_t littleEnd, uint64_t length) {
    return getSignedPart(machine.instruction, littleEnd, length);
}
