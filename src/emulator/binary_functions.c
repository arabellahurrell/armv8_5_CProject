#include "structures.c"

// Returns a mask with trailing `length` bits set to 1
uint64_t getMask(uint64_t length) {
    if (length >= 64) {
        return -1; // Return all bits set
    } else {
        return (1LL << length) - 1;
    }
}

// Returns a mask with bits from `left` - 1 to `right` (inclusive) set to 1
uint64_t getMaskBetween(uint64_t left, uint64_t right) {
    return getMask(left) ^ getMask(right);
}

// Returns the number of bits of the bit-width `sf`
uint64_t getLength(bool sf) {
    return sf ? 64 : 32;
}

// Returns the number of bytes in a word with the bit-width `sf`
uint64_t getWordBytes(bool sf) {
    return sf ? 8 : 4;
}

// Returns a mask to be applied to the result of an operation using the given bit-width `sf`
uint64_t getResultMask(bool sf) {
    return getMask(getLength(sf));
}

// Returns true if the `bit` of `value` is set using little-endian interpretation
bool isSet(uint64_t value, uint64_t bit) {
    return (value >> bit) & 1;
}

// Returns true if the most significant bit of `value` is set using the bit-width `sf`
bool isSigned(uint64_t value, bool sf) {
    return isSet(value, getLength(sf) - 1);
}

// Returns true if `mask` applied to `value` equals `target` value
bool isMaskEquals(uint64_t value, uint64_t mask, uint64_t target) {
    return (value & mask) == target;
}

// Returns a portion of `value` starting at bit `littleEnd` of `length` bits
uint64_t getPart(uint64_t value, uint64_t littleEnd, uint64_t length) {
    return (value >> littleEnd) & getMask(length);
}

// Returns a portion of `value` starting at bit `littleEnd` of `length` bits with sign extension
uint64_t getPartSigned(uint64_t value, uint64_t littleEnd, uint64_t length) {
    uint64_t part = getPart(value, littleEnd, length);
    if (isSet(part, length - 1)) { // Signed
        return part | getMaskBetween(64, length); // Sign-extend part
    } else { // Not signed
        return part;
    }
}

// Returns a portion of the current instruction starting at bit `littleEnd` of `length` bits
uint64_t getInstructionPart(uint64_t littleEnd, uint64_t length) {
    return getPart(machine.instruction, littleEnd, length);
}

// Returns a portion of the current instruction starting at bit `littleEnd` of `length` bits with sign extension
uint64_t getInstructionPartSigned(uint64_t littleEnd, uint64_t length) {
    return getPartSigned(machine.instruction, littleEnd, length);
}