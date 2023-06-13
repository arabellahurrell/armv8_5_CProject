//
// Created by Arabella Hurrell on 30/05/2023.
//


char* shiftBits (char* shiftType) {
    static char shifts[2];
    if (strcmp(shiftType, "lsl") == 0) {
        shifts[0] = '0';
        shifts[1] = '0';
    } else if (strcmp(shiftType, "lsr") == 0) {
        shifts[0] = '0';
        shifts[1] = '1';
    } else if (strcmp(shiftType, "asr") == 0) {
        shifts[0] = '1';
        shifts[1] = '0';
    } else {
        shifts[0] = '1';
        shifts[1] = '1';
    }
    return shifts;
}


char* arithmetics(char* opcode, char* rd, char* rn, char* op2, char* shiftType, char* shiftAmount) {

    return strcat(strcat(sf(rn), strcat(opcode, "100010")), strcat(master(convert(op2,18), shiftType, shiftAmount) + 10, convert(rd, 5)));
}

char* registerArithmetic(char* opcode, char* rd, char* rn, char*rm, char* shiftType, char* value) {
    return strcat(strcat(strcat(sf(rd), opcode), strcat("01011", shiftBits(shiftType))), strcat(strcat("0", convert(rm, 5)), strcat(
            convert(value, 6), strcat(convert(rn, 5), convert(rd, 5)))));
}

char* moveWides(char* opcode, char* rd, char* imm, char* sh, char* shiftType, char* shiftAmount) {
    return strcat(strcat(sf(rd), strcat(opcode, strcat("101", convert(shiftAmount, 2)))), strcat(strcat(master(convert(imm, 16), shiftType, shiftAmount), sh), convert(rd, 16)));
}

char* arithmeticParser (char* opcode, char** splitted) {
    if ((splitted[2])[0] != 'x' && (splitted[2])[0] != 'w' ) {
        if (getStringArrayLength(splitted) == 3) {
            return arithmetics(opcode, splitted[0], splitted[1], splitted[2], "lsl", "0");
        } else {
            return arithmetics(opcode, splitted[0], splitted[1], splitted[2], splitted[3], splitted[4]);
        }
    } else {
        if (getStringArrayLength(splitted) == 3) {
            return registerArithmetic(opcode, splitted[0], splitted[1], splitted[2], "lsl", "0");
        } else {
            return registerArithmetic(opcode, splitted[0], splitted[1], splitted[2], splitted[3], splitted[4]);
        }
    }
}

char* moveWideParser (char* opcode, char** splitted) {
    if (getStringArrayLength(splitted) == 2) {
        if (binaryToDecimal(hexToBinary(splitted[1])) > ((1 << 12) -1)) {
            splitted[1] = convert(intToString(binaryToDecimal(hexToBinary(splitted[1])) << 12), 16);
            return moveWides(opcode, splitted[0], splitted[1], "1" , "lsl", "0");
        } else {
            return moveWides(opcode, splitted[0], splitted[1], "0" , "lsl", "0");
        }
    } else {
        if (binaryToDecimal(hexToBinary(splitted[1])) > (2^12 -1) ) {
            splitted[1] = convert(intToString(binaryToDecimal(hexToBinary(splitted[1])) << 12), 16);
            return moveWides(opcode, splitted[0], splitted[1], "1" , splitted[2], splitted[3]);
        } else {
            return moveWides(opcode, splitted[0], splitted[1], "0" , splitted[2], splitted[3]);
        }
    }
}

char* logicalBitwise (char* mnemonic, char* rd, char* rn, char* rm, char* shiftType, char* value) {
    char* N;
    char* opcode;
    if (strcmp(mnemonic, "and") ==0 || strcmp(mnemonic, "orr") ==0 || strcmp(mnemonic, "eon") ==0 || strcmp(mnemonic, "ands") ==0) {
        N = "0";
    } else {
        N = "1";
    }
    if (strcmp(mnemonic, "and") == 0 || strcmp(mnemonic, "bic") == 0) {
        opcode = "00";
    } else if (strcmp(mnemonic, "orr") == 0 || strcmp(mnemonic, "orn") == 0) {
        opcode = "01";
    } else if (strcmp(mnemonic, "eon") == 0 || strcmp(mnemonic, "eor") == 0) {
        opcode = "10";
    } else {
        opcode = "11";
    }

    char* shifts = strcat(shiftBits(shiftType), N);
    char* result = strcat(strcat(sf(rd), strcat(opcode, strcat(strcat("01010", shifts), convert(rm, 5)))), strcat(convert(value, 6), strcat(convert(rn, 5), convert(rd, 5))));
    return result;
}


char* multiply (char* negate, char* rd, char* rn, char* rm, char* ra) {
    char* result = strcat(strcat(sf(rd), strcat("0011011000", convert(rm, 5))), strcat(negate, strcat(convert(ra,5), strcat(convert(rn, 5), convert(rd, 5)))));
    return result;
}

char* add (char* arguments, char* address) {
    char** splitted = splitStringOnWhitespace(arguments);
    return arithmeticParser("00", splitted);
}

char* adds (char* arguments, char* address) {
    char** splitted = splitStringOnWhitespace(arguments);
    return arithmeticParser("01", splitted);
}

char* sub (char* arguments, char* address) {
    char** splitted = splitStringOnWhitespace(arguments);
    return arithmeticParser("10", splitted);
}

char* subs (char* arguments, char* address) {
    char** splitted = splitStringOnWhitespace(arguments);
    return arithmeticParser("11", splitted);
}

char* movk (char* arguments, char* address) {
    char** splitted = splitStringOnWhitespace(arguments);
    return moveWideParser("00", splitted);
}

char* movn (char* arguments, char* address) {
    char** splitted = splitStringOnWhitespace(arguments);
    return moveWideParser("10", splitted);
}

char* movz (char* arguments, char* address) {
    char** splitted = splitStringOnWhitespace(arguments);
    return moveWideParser("11", splitted);
}

char* madd (char* arguments, char* address) {
    char** splitted = splitStringOnWhitespace(arguments);
    return multiply("0", splitted[0], splitted[1], splitted[2], splitted[3]);
}

char* msub (char* arguments, char* address) {
    char** splitted = splitStringOnWhitespace(arguments);
    return multiply("1", splitted[0], splitted[1], splitted[2], splitted[3]);
}

char* and (char* arguments, char* address) {
    char** splitted = splitStringOnWhitespace(arguments);
    if (getStringArrayLength(splitted) == 3) {
        return "10001010000000000000000000000000";
    }
    return logicalBitwise("and" ,splitted[0], splitted[1], splitted[2], splitted[3], splitted[4]);
}

char* ands (char* arguments, char* address) {
    char** splitted = splitStringOnWhitespace(arguments);
    return logicalBitwise("ands",splitted[0], splitted[1], splitted[2], splitted[3], splitted[4]);
}

char* orr (char* arguments, char* address) {
    char** splitted = splitStringOnWhitespace(arguments);
    return logicalBitwise("orr",splitted[0], splitted[1], splitted[2], splitted[3], splitted[4]);
}

char* eor (char* arguments, char* address) {
    char** splitted = splitStringOnWhitespace(arguments);
    return logicalBitwise("eor",splitted[0], splitted[1], splitted[2], splitted[3], splitted[4]);
}

char* orn (char* arguments, char* address) {
    char** splitted = splitStringOnWhitespace(arguments);
    return logicalBitwise("orn",splitted[0], splitted[1], splitted[2], splitted[3], splitted[4]);
}

char* bic (char* arguments, char* address) {
    char** splitted = splitStringOnWhitespace(arguments);
    return logicalBitwise("bic",splitted[0], splitted[1], splitted[2], splitted[3], splitted[4]);
}

char* bics (char* arguments, char* address) {
    char** splitted = splitStringOnWhitespace(arguments);
    return logicalBitwise("bics",splitted[0], splitted[1], splitted[2], splitted[3], splitted[4]);
}
char* eon (char* arguments, char* address) {
    char** splitted = splitStringOnWhitespace(arguments);
    return logicalBitwise("eon",splitted[0], splitted[1], splitted[2], splitted[3], splitted[4]);
}

char* cmp (char* arguments, char* address) {
    arguments = strcat("11111 ", arguments);
    return subs(arguments, address);
}

char* cmn (char* arguments, char* address) {
    arguments = strcat("11111 ", arguments);
    return adds(arguments, address);
}

char* neg (char* arguments, char* address) {
    char** split = splitStringOnFirstSpace(arguments);
    arguments = strcat(split[0], strcat(" 11111 ", split[1]));
    return sub(arguments, address);
}

char* negs (char* arguments, char* address) {
    char** split = splitStringOnFirstSpace(arguments);
    arguments = strcat(split[0], strcat(" 11111 ", split[1]));
    return subs(arguments, address);
}

char* tst (char* arguments, char* address) {
    arguments = strcat("11111 ", arguments);
    return ands(arguments, address);
}

char* mvn (char* arguments, char* address) {
    char** split = splitStringOnFirstSpace(arguments);
    arguments = strcat(split[0], strcat(" 11111 ", split[1]));
    return orn(arguments, address);
}

char* mov (char* arguments, char* address) {
    char** split = splitStringOnFirstSpace(arguments);
    arguments = strcat(split[0], strcat(" 11111 ", split[1]));
    return orr(arguments, address);
}

char* mul (char* arguments, char* address) {
    arguments = strcat(arguments, " 11111");
    return madd(arguments, address);
}

char* mneg (char* arguments, char* address) {
    arguments = strcat(arguments, " 11111");
    return msub(arguments, address);
}

char* nop (char* arguments, char* address) {
    return "11010101000000110010000000011111";
}
