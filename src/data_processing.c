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

char* hw (char* size) {
    int num = atoi(size);
    if (num < 16) {
        return "00";
    } else if (num >=16 && num < 32) {
        return "01";
    } else if (num >= 32 && num < 48) {
        return "10";
    } else {
        return "11";
    }
}


char* arithmetics(char* opcode, char* rd, char* rn, char* op2, char* shiftType, char* shiftAmount) {
    char* result = malloc(33 * sizeof(char));
    result[0] = '\0';

    char* sf_rn = sf(rn);
    char* convert_op2 = convert(op2, 18);
    char* convert_rd = convert(rd, 5);
    char* convert_rn = convert(rn, 5);
    char* operand;
    char* sh;
    char* master_result;
    if (atoi(op2) > ((1 << 12) -1)) {
         master_result = master(convert(op2, 12), shiftType, "12");
    } else {
        master_result  = convert_op2;
    }
    if (strcmp(shiftAmount, "0") == 0) {
        sh = "0";
    } else {
        sh = "1";
    }
//    if (strcmp(op2, "") == 0) {
//        operand = master(master_result, "lsl", "12");
//        sh = "1";
//    } else {
//        operand = master_result;
//        sh = "0";
//    }

    strcat(result, sf_rn);
    printf("%d\n", strlen(opcode));
    fflush(stdout);
    strcat(result, opcode);
    strcat(result, "100010");
    // sh if bigger that 2^12 - 1
    strcat(result, sh);
    strcat(result, truncateString(master_result, 18));
    fflush(stdout);
    strcat(result, convert_rn);
    strcat(result, convert_rd);

    printf("%s\n", result);
    fflush(stdout);
    return result;
    //return strcat(strcat(sf(rn), strcat(opcode, "100010")), strcat(master(convert(op2,18), shiftType, shiftAmount) + 10, convert(rd, 5)));
}

char* registerArithmetic(char* opcode, char* rd, char* rn, char*rm, char* shiftType, char* value) {
    char* result = malloc(33 * sizeof(char));
    result[0] = '\0';

    strcat(result, sf(rd));
    strcat(result, opcode);
    strcat(result, "01011");
    strcat(result, shiftBits(shiftType));
    strcat(result, "0");
    strcat(result, convert(rm, 5));
    strcat(result, convert(value, 6));
    strcat(result, convert(rn, 5));
    strcat(result, convert(rd, 5));

    printf("%s\n", result);
    fflush(stdout);

    return result;
}

char* moveWides(char* opcode, char* rd, char* imm, char* sh, char* shiftType, char* shiftAmount) {
    char* imm16;
    if (imm[1] == 'x') {
        imm16 = master(truncateString(hexToBinary(imm), 16), shiftType, shiftAmount);
        printf("shifted : %s\n", master(hexToBinary(imm), shiftType, shiftAmount));
    } else {
        imm16 = master(convert(imm, 16), shiftType, shiftAmount);
        printf("shifted : %s\n", master(convert(imm, 16), shiftType, shiftAmount));
    }
    char* result = malloc(33 * sizeof(char));
    result[0] = '\0';
    strcat(result, sf(rd));
    strcat(result, opcode);
    strcat(result, "100101");
    strcat(result, hw(shiftAmount));
    printf("hw : %s\n", hw(shiftAmount));
    strcat(result, imm16);
    printf("imm: %s\n", imm);
    printf("imm16: %s\n", imm16);
    printf("rd: %s\n", rd);
    printf("converted: %s\n", convert(rd, 5));
    strcat(result, convert(rd, 5));
    printf("result : %s\n", result);
    return result;
    //return strcat(strcat(sf(rd), strcat(opcode, strcat("100101", convert(shiftAmount, 2)))), strcat(strcat(master(convert(imm, 16), shiftType, shiftAmount), sh), convert(rd, 16)));
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
        if (immOrHex(splitted[1]) > ((1 << 12) -1)) {
            printf("greater\n");
            splitted[1] = intToString(immOrHex(splitted[1]) << 12);
            return moveWides(opcode, splitted[0], splitted[1], "1" , "lsl", "0");
        } else {
            printf("not greater\n");
            return moveWides(opcode, splitted[0], splitted[1], "0" , "lsl", "0");
        }
    } else {
        if (immOrHex(splitted[1]) > ((1 << 12) -1)) {
            printf("greater\n");
            splitted[1] = intToString(immOrHex(splitted[1]) << 12);
            return moveWides(opcode, splitted[0], splitted[1], "1" , splitted[2], splitted[3]);
        } else {
            printf("not greater\n");
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

    //char* shifts = strcat(shiftBits(shiftType), N);
    char* result = malloc(33 * sizeof(char));
    strcat(result, sf(rd));
    printf("%s\n", result);
    strcat(result, opcode);
    printf("%s\n", result);
    strcat(result, "01010");
    printf("%s\n", result);
    if (!shiftType) {
        strcat(result, "00");
    } else {
        strcat(result, shiftType);
    }
    printf("%s\n", result);
    strcat(result, N);
    printf("%s\n", result);
    printf("rm : %s\n", rm);
    printf("rm converted: %s\n", convert(rm, 5));
    strcat(result, convert(rm, 5));
    printf("%s\n", result);
    if (!value) {
        strcat(result, "000000");
    } else {
        strcat(result, truncateString(value, 6));
    }
    printf("%s\n", result);
    printf("rn : %s\n", rn);
    printf("rn converted: %s\n", registerConvert(rn));
    strcat(result, registerConvert(rn));
    printf("%s\n", result);
    strcat(result, convert(rd, 5));
    printf("result = %s\n", result);
    return result;
}


char* multiply (char* negate, char* rd, char* rn, char* rm, char* ra) {
    char* result = malloc(33 * sizeof(char));
    result[0] = '\0';

    strcat(result, sf(rn));
    strcat(result, "0011011000");
    strcat(result, negate);
    strcat(result, convert(ra,5));
    strcat(result, convert(rn,5));
    strcat(result, convert(rd,5));
    printf("%s\n", result);
    fflush(stdout);
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
    return moveWideParser("11", splitted);
}

char* movn (char* arguments, char* address) {
    char** splitted = splitStringOnWhitespace(arguments);
    return moveWideParser("00", splitted);
}

char* movz (char* arguments, char* address) {
    char** splitted = splitStringOnWhitespace(arguments);
    return moveWideParser("10", splitted);
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
    // TODO: string cat won't work here
    arguments = strcat(split[0], strcat(" 11111 ", split[1]));
    return sub(arguments, address);
}

char* negs (char* arguments, char* address) {
    char** split = splitStringOnFirstSpace(arguments);
    // TODO: string cat won't work here
    arguments = strcat(split[0], strcat(" 11111 ", split[1]));
    return subs(arguments, address);
}

char* tst (char* arguments, char* address) {
    arguments = strcat("11111 ", arguments);
    return ands(arguments, address);
}

char* mvn (char* arguments, char* address) {
    char** split = splitStringOnFirstSpace(arguments);
    // TODO: string cat won't work here
    arguments = strcat(split[0], strcat(" 11111 ", split[1]));
    return orn(arguments, address);
}

char* mov (char* arguments, char* address) {
    char** split = splitStringOnFirstSpace(arguments);
    // TODO: string cat won't work here
    for (int i = 0; i < getStringArrayLength(split); i++) {
        printf("%s\n", split[i]);
    }
    fflush(stdout);
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
