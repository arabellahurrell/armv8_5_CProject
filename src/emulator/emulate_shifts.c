/*
 * Helper functions to execute shifts
 */

// Performs arithmetic shift right by `shiftAmount` on the `value` using the bit-width `sf`
uint64_t arithmeticShiftRight(uint64_t value, uint64_t shiftAmount, uint64_t sf) {
    uint64_t result, mask;
    if (isSigned(value, sf)) { // Signed
        mask = getMaskBetween(64, getLength(sf) - shiftAmount);
        result = (value >> shiftAmount) | mask;
    } else { // Unsigned
        result = value >> shiftAmount;
    }
    return result & getResultMask(sf);
}

// Performs rotate right by `shiftAmount` on the `value` using the bit-width `sf`
uint64_t rotateRight(uint64_t value, uint64_t shiftAmount, uint64_t sf) {
    uint64_t shifted, rotated;
    shifted = value >> shiftAmount;
    rotated = value << (getLength(sf) - shiftAmount);
    return (shifted | rotated) & getResultMask(sf);
}

// Applies the shift given by `shiftType` to `value` by `shiftAmount` using the bit-width `sf`
uint64_t applyShift(uint64_t value, uint64_t shiftType, uint64_t shiftAmount, uint64_t sf) {
    switch (shiftType) {
        case 0b00: // lsl
            return (value << shiftAmount) & getResultMask(sf);
        case 0b01: // lsr
            return (value >> shiftAmount) & getResultMask(sf);
        case 0b10: // asr
            return arithmeticShiftRight(value, shiftAmount, sf);
        case 0b11: // ror
            return rotateRight(value, shiftAmount, sf);
        default: return 0;
    }
}