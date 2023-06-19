//
// Created by Arabella Hurrell on 30/05/2023.
//
#define zeroregister "31";

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
    printf("normal arithemtic\n");
    char* result = malloc(33 * sizeof(char));
    result[0] = '\0';
    printf("opcode : %s\n", opcode);
    printf("rd : %s\n", rd);
    printf("rn : %s\n", rn);
    printf("op2 : %s\n", op2);
    printf("shiftType : %s\n", shiftType);
    printf("shiftAmount : %s\n", shiftAmount);
    char* convert_op2;
    int op;
    if(op2[1] == 'x') {
        op = binaryToDecimal(hexToBinary(op2));
        convert_op2 = truncateString(hexToBinary(op2), 12);
        printf("hex op2 convert : %s\n", convert_op2);
        printf("op : %d\n", op);
    } else {
        op = atoi(op2);
        convert_op2 = convert(op2, 12);
        printf("dec op2 convert : %s\n", convert_op2);
        printf("op : %d\n", op);
    }
    char* sf_rn = sf(rn);
    char* convert_rd = registerConvert(rd);
    char* convert_rn = registerConvert(rn);
    char* sh;
    char* master_result;
    if (op > ((1 << 12) -1)) {
        //sh = "1";
         master_result = master(truncateString(intToString(op),12), shiftType, shiftAmount);
         printf("master 1: %s\n", master_result);
    } else {
        //sh = "0";
        master_result  = convert_op2;
        printf("master 2 : %s\n", master_result);
    }
    if ((strcmp(shiftType, "lsl") == 0 || strcmp(shiftType, "asl") == 0) && atoi(shiftAmount) % 16 >= 12) {
        sh = "1";
    } else {
        sh = "0";
    }
//    if (strcmp(shiftAmount, "0") == 0) {
//        sh = "0";
//    } else {
//        sh = "1";
//    }
//    if (strcmp(op2, "") == 0) {
//        operand = master(master_result, "lsl", "12");
//        sh = "1";
//    } else {
//        operand = master_result;
//        sh = "0";
//    }

    strcat(result, sf_rn);
    printf("%s\n", result);
    strcat(result, opcode);
    printf("%s\n", result);
    strcat(result, "100010");
    printf("%s\n", result);
    // sh if bigger that 2^12 - 1
    strcat(result, sh);
    printf("%s\n", result);
    strcat(result, truncateString(master_result,12));
    strcat(result, convert_rn);
    strcat(result, convert_rd);
    printf("result : %s\n", result);
    fflush(stdout);
    return result;
    //return strcat(strcat(sf(rn), strcat(opcode, "100010")), strcat(master(convert(op2,18), shiftType, shiftAmount) + 10, convert(rd, 5)));
}

char* registerArithmetic(char* opcode, char* rd, char* rn, char* rm, char* shiftType, char* value) {
    printf("register arithmetic\n");
    char* result = malloc(33 * sizeof(char));
    result[0] = '\0';
    char* val;
    if (value[1] == 'x') {
        val = truncateString(hexToBinary(value), 6);
    } else {
        val = convert(value, 6);
    }
    printf(hexToBinary(value));


    strcat(result, sf(rd));
    printf("%s\n", result);
    strcat(result, opcode);
    printf("%s\n", result);
    strcat(result, "01011");
    printf("%s\n", result);
    strcat(result, shiftBits(shiftType));
    printf("%s\n", result);
    strcat(result, "0");
    printf("%s\n", result);
    strcat(result, registerConvert(rm));
    printf("%s\n", result);
    strcat(result, val);
    printf("%s\n", result);
    strcat(result, registerConvert(rn));
    printf("%s\n", result);
    strcat(result, registerConvert(rd));

    printf("%s\n", result);
    fflush(stdout);

    return result;
}

char* moveWides(char* opcode, char* rd, char* imm, char* sh, char* shiftType, char* shiftAmount) {
    char* imm16;
    printf("opcode : %s\n", opcode);
    printf("rd : %s\n", rd);
    printf("imm %s\n", imm);
    printf("sh : %s\n", sh);
    printf("shiftType %s\n", shiftType);
    printf("shiftAmount %s\n", shiftAmount);

    if (imm[1] == 'x') {
        printf("hex value\n");
        imm16 = master(truncateString(hexToBinary(imm), 16), shiftType, shiftAmount);
        printf("shifted : %s\n", master(hexToBinary(imm), shiftType, shiftAmount));
    } else {
        printf("%s\n", imm);
        printf("%s\n", convert(imm, 16));
        imm16 = master(convert(imm, 16), shiftType, shiftAmount);
        printf("shifted : %s\n", master(convert(imm, 16), shiftType, shiftAmount));
    }
    char* result = malloc(33 * sizeof(char));
    result[0] = '\0';
    strcat(result, sf(rd));
    printf("%s\n", result);
    strcat(result, opcode);
    printf("%s\n", result);
    strcat(result, "100101");
    printf("%s\n", result);
    strcat(result, hw(shiftAmount));
    printf("%s\n", result);
    printf("hw : %s\n", hw(shiftAmount));
    strcat(result, imm16);
    printf("%s\n", result);
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
            splitted[1] = intToString(immOrHex(splitted[1]));
            printf(splitted[1]);
            return moveWides(opcode, splitted[0], splitted[1], "1" , "lsl", "0");
        } else {
            printf("not greater\n");
            return moveWides(opcode, splitted[0], splitted[1], "0" , "lsl", "0");
        }
    } else {
        if (immOrHex(splitted[1]) > ((1 << 12) -1)) {
            printf("greater\n");
            splitted[1] = intToString(immOrHex(splitted[1]));
            return moveWides(opcode, splitted[0], splitted[1], "1" , splitted[2], splitted[3]);
        } else {
            printf("not greater\n");
            return moveWides(opcode, splitted[0], splitted[1], "0" , splitted[2], splitted[3]);
        }
    }
}

char* logicalBitwise (char* mnemonic, char* rd, char* rn, char* rm, char* shiftType, char* value) {
    printf("mnemonic : %s\n", mnemonic);
    printf("rd : %s\n", rd);
    printf("rn : %s\n", rn);
    printf("rm : %s\n", rm);
    printf("shiftType : %s\n", shiftType);
    printf("value : %s\n", value);
    char* v;
    if (!value) {
        v = "000000";
    }
    else  {
        v = immHexToDenary(value, 6);
    }
    char* N;
    char* opcode;
    if (strcmp(mnemonic, "and") ==0 || strcmp(mnemonic, "orr") ==0 || strcmp(mnemonic, "eor") ==0 || strcmp(mnemonic, "ands") ==0) {
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
        strcat(result, shiftBits(shiftType));
    }
    printf("%s\n", result);
    strcat(result, N);
    printf("%s\n", result);
    printf("rm : %s\n", rm);
    printf("rm converted: %s\n", registerConvert(rm));
    strcat(result, registerConvert(rm));
    printf("%s\n", result);
    strcat(result, v);//truncateString(value, 6)printf("value : %s\n", v);
    printf("%s\n", result);
    printf("rn : %s\n", rn);
    printf("rn converted: %s\n", registerConvert(rn));
    strcat(result, registerConvert(rn));
    printf("%s\n", result);
    strcat(result, registerConvert(rd));
    printf("result = %s\n", result);
    return result;
}


char* multiply (char* negate, char* rd, char* rn, char* rm, char* ra) {
    char* result = malloc(33 * sizeof(char));
    result[0] = '\0';

    printf("ra : %s\n", ra);
    printf("rd : %s\n", rd);
    printf("rn : %s\n", rn);

    strcat(result, sf(rn));
    printf("%s\n", result);
    strcat(result, "0011011000");
    printf("%s\n", result);
    strcat(result, registerConvert(rm));
    printf("%s\n", result);
    strcat(result, negate);
    printf("%s\n", result);
    strcat(result, registerConvert(ra));
    printf("%s\n", result);
    strcat(result, registerConvert(rn));
    printf("%s\n", result);
    strcat(result, registerConvert(rd));
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
    if (strcmp(splitted[0], "x0") == 0 && strcmp(splitted[1], "x0") == 0 && strcmp(splitted[2], "x0") == 0) {
//    if (getStringArrayLength(splitted) == 3) {
        return "10001010000000000000000000000000";
        //return logicalBitwise("and", splitted[0], splitted[1], splitted[2], NULL, NULL);
    } else if (getStringArrayLength(splitted) == 3) {
        return logicalBitwise("and", splitted[0], splitted[1], splitted[2], NULL, NULL);
    } else {
        return logicalBitwise("and", splitted[0], splitted[1], splitted[2], splitted[3], splitted[4]);
    }
}

char* ands (char* arguments, char* address) {
    char** splitted = splitStringOnWhitespace(arguments);
    return logicalBitwise("ands",splitted[0], splitted[1], splitted[2], splitted[3], splitted[4]);
}

char* orr (char* arguments, char* address) {
    char** splitted = splitStringOnWhitespace(arguments);
    for (int i = 0; i < getStringArrayLength(splitted); i++) {
        printf("%s\n", splitted[i]);
    }
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
    arguments = strcat("31 ", arguments);
    return subs(arguments, address);
}

char* cmn (char* arguments, char* address) {
    arguments = strcat("31 ", arguments);
    return adds(arguments, address);
}

char* neg (char* arguments, char* address) {
    char** split = splitStringOnFirstSpace(arguments);
    arguments = strcat(split[0], " 31 ");
    arguments = strcat(arguments, split[1]);
    return sub(arguments, address);
}

char* negs (char* arguments, char* address) {
    char** split = splitStringOnFirstSpace(arguments);
    arguments = strcat(split[0], " 31 ");
    arguments = strcat(arguments, split[1]);
    return subs(arguments, address);
}

char* tst (char* arguments, char* address) {
    arguments = strcat("31 ", arguments);
    return ands(arguments, address);
}

char* mvn (char* arguments, char* address) {
    char** split = splitStringOnFirstSpace(arguments);
    arguments = strcat(split[0], " 31 ");
    arguments = strcat(arguments, split[1]);
    return orn(arguments, address);
}

char* mov (char* arguments, char* address) {
    char** split = splitStringOnFirstSpace(arguments);
    for (int i = 0; i < getStringArrayLength(split); i++) {
        printf("%s\n", split[i]);
    }
    fflush(stdout);
    arguments = strcat(split[0], " 31 ");
    arguments = strcat(arguments, split[1]);
    return orr(arguments, address);
}

char* mul (char* arguments, char* address) {
    arguments = strcat(arguments, " 31");
    return madd(arguments, address);
}

char* mneg (char* arguments, char* address) {
    arguments = strcat(arguments, " 31");
    return msub(arguments, address);
}

char* nop (char* arguments, char* address) {
    return "11010101000000110010000000011111";
}
