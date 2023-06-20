//
// Created by Arabella Hurrell on 30/05/2023.
//

// Function to shift bits based on shift type

//sprintf(varInto, %0x6d, input)
//strtol(input, NULL, 16)

char* shiftBits(char* shiftType) {
    static char shifts[2];
    if (strcmp(shiftType, "lsl") == 0) {
        shifts[0] = '0';  // Set the shift bits to "00" for logical shift left
        shifts[1] = '0';
    } else if (strcmp(shiftType, "lsr") == 0) {
        shifts[0] = '0';  // Set the shift bits to "01" for logical shift right
        shifts[1] = '1';
    } else if (strcmp(shiftType, "asr") == 0) {
        shifts[0] = '1';  // Set the shift bits to "10" for arithmetic shift right
        shifts[1] = '0';
    } else {
        shifts[0] = '1';  // Set the shift bits to "11" for rotate right
        shifts[1] = '1';
    }
    return shifts;
}

char* hw(char* size) {
    int num = atoi(size);
    if (num < 16) {
        return "00";  // If the number is less than 16, return "00" for the size bits
    } else if (num >= 16 && num < 32) {
        return "01";  // If the number is between 16 and 31, return "01" for the size bits
    } else if (num >= 32 && num < 48) {
        return "10";  // If the number is between 32 and 47, return "10" for the size bits
    } else {
        return "11";  // For any other number, return "11" for the size bits
    }
}

// Function for normal arithmetic operations
char* arithmetics(char* opcode, char* rd, char* rn, char* op2, char* shiftType, char* shiftAmount) {
    printf("normal arithmetic\n");
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

    if (op2[1] == 'x') {  // Check if op2 is a hexadecimal value
        op = binaryToDecimal(hexToBinary(op2));  // Convert hexadecimal to decimal
        //convert_op2 = truncateString(hexToBinary(op2), 12);
        convert_op2 = convert(intToString(strtol(op2, NULL, 16)), 12);// Convert hexadecimal to 12-bit binary string
        printf("hex op2 convert : %s\n", convert_op2);
        printf("op : %d\n", op);
    } else {
        op = atoi(op2);  // Convert op2 to decimal
        convert_op2 = convert(op2, 12);  // Convert op2 to 12-bit binary string
        printf("dec op2 convert : %s\n", convert_op2);
        printf("op : %d\n", op);
    }
    char* sf_rn = sf(rn); // Get the "sf" bits based on the register size
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


    // Concatenate the opcode, "sf" bits, destination register, source register,
    // "size" bits, shift bits, shift amount, and operand 2 to form the resulting instruction
    strcat(result, sf_rn);
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
}

char* registerArithmetic(char* opcode, char* rd, char* rn, char* rm, char* shiftType, char* value) {
    printf("register arithmetic\n");
    printf(" opcode : %s\n", opcode);
    printf(" rd : %s\n", rd );
    printf("rm  : %s\n", rm);
    printf(" rn : %s\n", rn);
    printf(" shiftType : %s\n", shiftType);
    printf(" shift Amount : %s\n", value);

    // Allocate memory for the result string
    char* result = malloc(33 * sizeof(char));
    if (!result) {
        printf("malloc error\n");
        fflush(stdout);
    }
    result[0] = '\0';

    // Variable to store the converted value
    char* val;

    if(strcmp(value, "0") == 0) {
        printf("value is 0\n");
        fflush(stdout);
        val = "000000";
    } else if (value[1] == 'x') { // Check if the value is in hexadecimal format
        // Convert hexadecimal value to binary and truncate it to 6 bits
        val = convert(intToString(strtol(value, NULL, 16)), 6);
        //val = truncateString(hexToBinary(value), 6);
    } else {
        // Convert decimal value to binary representation with 6 bits
        val = convert(value, 6);
    }

    // Concatenate the sign flag (SF) of the destination register to the result string
    strcat(result, sf(rn));
    printf("%s\n", result);

    // Concatenate the opcode to the result string
    strcat(result, opcode);
    printf("%s\n", result);

    // Concatenate "01011" (fixed bits) to the result string
    strcat(result, "01011");
    printf("%s\n", result);

    // Concatenate the shift bits based on the shift type to the result string
    strcat(result, shiftBits(shiftType));
    printf("%s\n", result);

    // Concatenate "0" (fixed bit) to the result string
    strcat(result, "0");
    printf("%s\n", result);

    // Concatenate the converted shift register (rm) to the result string
    strcat(result, registerConvert(rm));
    printf("%s\n", result);

    // Concatenate the converted value (val) to the result string
    strcat(result, val);
    printf("%s\n", result);

    // Concatenate the converted source register (rn) to the result string
    strcat(result, registerConvert(rn));
    printf("%s\n", result);

    // Concatenate the converted destination register (rd) to the result string
    strcat(result, registerConvert(rd));
    printf("%s\n", result);

    // Flush the output stream to ensure the printed text is displayed immediately
    fflush(stdout);

    // Return the result string
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

    // Check if the immediate value is in hexadecimal format
    if (imm[1] == 'x') {
        printf("hex value\n");
        // Convert hexadecimal value to 16-bit binary and perform shifting
        char* d = convert(intToString(strtol(imm, NULL, 16)), 16);
        imm16 = master(d, shiftType, shiftAmount);
        //imm16 = master(truncateString(hexToBinary(imm), 16), shiftType, shiftAmount);
        printf("shifted : %s\n", imm16);
    } else {
        printf("%s\n", imm);
        printf("%s\n", convert(imm, 16));
        // Convert decimal value to 16-bit binary and perform shifting
        imm16 = master(convert(imm, 16), shiftType, shiftAmount);
        printf("shifted : %s\n", master(convert(imm, 16), shiftType, shiftAmount));
    }

    // Allocate memory for the result string
    char* result = malloc(33 * sizeof(char));
    result[0] = '\0';

    // Concatenate the sign flag (SF) of the destination register to the result string
    strcat(result, sf(rd));
    printf("%s\n", result);

    // Concatenate the opcode to the result string
    strcat(result, opcode);
    printf("%s\n", result);

    // Concatenate "100101" (fixed bits) to the result string
    strcat(result, "100101");
    printf("%s\n", result);

    // Concatenate the hardware bits based on the shift amount to the result string
    strcat(result, hw(shiftAmount));
    printf("%s\n", result);
    printf("hw : %s\n", hw(shiftAmount));

    // Concatenate the converted 16-bit immediate value (imm16) to the result string
    strcat(result, imm16);
    printf("%s\n", result);

    printf("imm: %s\n", imm);
    printf("imm16: %s\n", imm16);
    printf("rd: %s\n", rd);

    // Convert the destination register (rd) to a 5-bit binary representation and concatenate it to the result string
    strcat(result, convert(rd, 5));
    printf("converted: %s\n", convert(rd, 5));
    printf("result : %s\n", result);

    // Return the result string
    return result;
}


char* arithmeticParser (char* opcode, char** splitted) {
    // Check the format of the third element in the 'splitted' array
    if ((splitted[2])[0] != 'x' && (splitted[2])[0] != 'w' ) {
        // Check the length of the 'splitted' array
        if (getStringArrayLength(splitted) == 3) {
            // Call the 'arithmetics' function with default shift type and shift amount
            return arithmetics(opcode, splitted[0], splitted[1], splitted[2], "lsl", "0");
        } else {
            // Call the 'arithmetics' function with provided shift type and shift amount
            return arithmetics(opcode, splitted[0], splitted[1], splitted[2], splitted[3], splitted[4]);
        }
    } else {
        // Check the length of the 'splitted' array
        if (getStringArrayLength(splitted) == 3) {
            // Call the 'registerArithmetic' function with default shift type and shift amount
            return registerArithmetic(opcode, splitted[0], splitted[1], splitted[2], "lsl", "0");
        } else {
            // Call the 'registerArithmetic' function with provided shift type and shift amount
            return registerArithmetic(opcode, splitted[0], splitted[1], splitted[2], splitted[3], splitted[4]);
        }
    }
}

char* moveWideParser (char* opcode, char** splitted) {
    // Check the length of the 'splitted' array
    if (getStringArrayLength(splitted) == 2) {
        // Check if the value exceeds the immediate range of 12 bits
        if (immOrHex(splitted[1]) > ((1 << 12) - 1)) {
            printf("greater\n");
            // Convert the value to string and call the 'moveWides' function with immediate flag set to "1"
            splitted[1] = intToString(immOrHex(splitted[1]));
            printf(splitted[1]);
            return moveWides(opcode, splitted[0], splitted[1], "1", "lsl", "0");
        } else {
            printf("not greater\n");
            // Call the 'moveWides' function with immediate flag set to "0"
            return moveWides(opcode, splitted[0], splitted[1], "0", "lsl", "0");
        }
    } else {
        // Check if the value exceeds the immediate range of 12 bits
        if (immOrHex(splitted[1]) > ((1 << 12) - 1)) {
            printf("greater\n");
            // Convert the value to string and call the 'moveWides' function with immediate flag set to "1" and provided shift type and shift amount
            splitted[1] = intToString(immOrHex(splitted[1]));
            return moveWides(opcode, splitted[0], splitted[1], "1", splitted[2], splitted[3]);
        } else {
            printf("not greater\n");
            // Call the 'moveWides' function with immediate flag set to "0" and provided shift type and shift amount
            return moveWides(opcode, splitted[0], splitted[1], "0", splitted[2], splitted[3]);
        }
    }
}


char* logicalBitwise(char* mnemonic, char* rd, char* rn, char* rm, char* shiftType, char* value) {
    printf("mnemonic : %s\n", mnemonic);
    printf("rd : %s\n", rd);
    printf("rn : %s\n", rn);
    printf("rm : %s\n", rm);
    printf("shiftType : %s\n", shiftType);
    printf("value : %s\n", value);

    char* v;
    // Check if value is provided
    if (!value) {
        v = "000000";  // Default value if not provided
    } else {
        v = immHexToBinary(value, 6);  // Convert the value from hex/immediate to binary
    }

    char* N;
    char* opcode;
    // Determine N and opcode based on the mnemonic
    if (strcmp(mnemonic, "and") == 0 || strcmp(mnemonic, "orr") == 0 || strcmp(mnemonic, "eor") == 0 || strcmp(mnemonic, "ands") == 0) {
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

    char* result = malloc(33 * sizeof(char));
    strcat(result, sf(rd));  // Append the condition code for the destination register
    printf("%s\n", result);
    strcat(result, opcode);  // Append the opcode
    printf("%s\n", result);
    strcat(result, "01010");  // Append the fixed bits
    printf("%s\n", result);
    if (!shiftType) {
        strcat(result, "00");  // Default shift type if not provided
    } else {
        strcat(result, shiftBits(shiftType));  // Append the shift bits based on the shift type
    }
    printf("%s\n", result);
    strcat(result, N);  // Append the N bit
    printf("%s\n", result);
    printf("rm : %s\n", rm);
    printf("rm converted: %s\n", registerConvert(rm));
    strcat(result, registerConvert(rm));  // Append the converted rm register
    printf("%s\n", result);
    printf("value : %s\n", v);
    strcat(result, v);  // Append the value
    printf("%s\n", result);
    printf("rn : %s\n", rn);
    printf("rn converted: %s\n", registerConvert(rn));
    strcat(result, registerConvert(rn));  // Append the converted rn register
    printf("%s\n", result);
    strcat(result, registerConvert(rd));  // Append the converted rd register
    printf("result = %s\n", result);
    return result;
}



char* multiply(char* negate, char* rd, char* rn, char* rm, char* ra) {
    char* result = malloc(33 * sizeof(char));
    result[0] = '\0';

    printf("ra : %s\n", ra);
    printf("rd : %s\n", rd);
    printf("rn : %s\n", rn);

    strcat(result, sf(rn));  // Append the condition code for the rn register
    printf("%s\n", result);
    strcat(result, "0011011000");  // Append the fixed bits
    printf("%s\n", result);
    strcat(result, registerConvert(rm));  // Append the converted rm register
    printf("%s\n", result);
    strcat(result, negate);  // Append the negate flag
    printf("%s\n", result);
    strcat(result, registerConvert(ra));  // Append the converted ra register
    printf("%s\n", result);
    strcat(result, registerConvert(rn));  // Append the converted rn register
    printf("%s\n", result);
    strcat(result, registerConvert(rd));  // Append the converted rd register
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
    printf("inside cmp\n");
    fflush(stdout);
    char* args = malloc(strlen(arguments) + 3);
    args[0] = '\0';
    args = strcat(args, "31 ");
    args = strcat(args, arguments);
    return subs(args, address);
}

char* cmn (char* arguments, char* address) {
    char* args = malloc(strlen(arguments) + 3);
    args[0] = '\0';
    args = strcat(args, "31 ");
    args = strcat(args, arguments);
    return adds(args, address);
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
