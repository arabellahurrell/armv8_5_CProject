//
// Created by Arabella Hurrell on 30/05/2023.
//


//char* singleData (char* rt, char* xn, char* val, char* shiftAmount) {
//    char* x = sf(rt);
//    int sizeOfValue = strlen(val);
//    if (val[sizeOfValue -1] == "!") {
//
//    }
//}


char* registerOffset(char* rt, char* xn, char* xm, char* l) {
    char* result = malloc(33 * sizeof(char));
    result[0] = '\0';

    // int decimal = binaryToDecimal(master(xm, "lsl", shiftAmount)) + binaryToDecimal(xn);
    //char* intToString_result = intToString(decimal);
    //char* convert_result = convert(intToString_result, 19);

    printf("Loading Literal");
    strcat(result, "1");
    strcat(result, sf(rt));
    strcat(result, "1110000");
    strcat(result, l);
    strcat(result, "1");
    strcat(result, registerConvert(xm));
    strcat(result, "011010");
    strcat(result, registerConvert(xn));
    strcat(result, registerConvert(rt));

    printf("register result: %s\n", result);
    fflush(stdout);

    return result;
//    convert(intToString(binaryToDecimal(master(xm, "lsl", shiftAmount)) + binaryToDecimal(xn)), 19);
//    return strcat(strcat(strcat("1", sf(rt)), strcat("1110000", l)), strcat(strcat("1", convert(xm, 5)), strcat("011010", strcat(convert(xn, 5), convert(rt, 5)))));
}

char* indexedOffset(char* rt, char* xn, char* value, char* l, char* i) {
    char* result = malloc(33 * sizeof(char));
    result[0] = '\0';

    strcat(result, "1");
    strcat(result, sf(rt));
    strcat(result, "1110000");
    strcat(result, l);
    strcat(result, "0");
    strcat(result, truncateString(value, 9));
    strcat(result, i);
    strcat(result, "1");
    strcat(result, registerConvert(xn, 5));
    strcat(result, registerConvert(rt, 5));

    printf("%s\n", result);
    fflush(stdout);

    return result;
    //return strcat(strcat(strcat("1", sf(rt)), strcat("1110000", l)), strcat(strcat(truncateString(value, 9), i), strcat(strcat("1", convert(xn, 5)), convert(rt, 5))));
}

char* unsignedOffset (char* rt, char* xn, char* value, char* l) {
    char* result = malloc(33 * sizeof(char));
    result[0] = '\0';

    //if (strcmp(sf(rt), "0");

    strcat(result, "1");
    strcat(result, sf(rt));
    strcat(result, "1110010");
    strcat(result, l);
    strcat(result, truncateString(value, 12));
    strcat(result, convert(xn, 5));
    strcat(result, convert(rt, 5));

    printf("%s\n", result);
    fflush(stdout);

    return result;
    //return strcat(strcat("0", strcat(sf(rt), "1110010")), strcat(strcat(l, truncateString(value, 12)), strcat(convert(xn, 5), convert(rt, 5))));
}

char* loadLiteral(char* rt, char* value) {
    char* result = malloc(33 * sizeof(char));
    result[0] = '\0';

    strcat(result, "0");
    printf("%s\n", result);
    strcat(result, sf(rt));
    printf("%s\n", result);
    strcat(result, "011000");
    printf("%s\n", result);
    strcat(result, truncateString(value, 19));
    printf("%s\n", result);
    strcat(result, registerConver(rt, 5));

    printf("%s\n", result);
    fflush(stdout);

    return result;
    //return strcat(strcat("0", sf(rt)), strcat("011000", strcat(truncateString(value, 19), convert(rt, 5))));
}

char* dataTransferParser (char** splitted, char* l) {
    int length = getStringArrayLength(splitted);
    if (length == 3) {
        if ((splitted[2])[strlen(splitted[2]) - 1] == '!') {
            printf("PRE-INDEX OFFSETTING\n");
            return indexedOffset(splitted[0], splitted[1], splitted[2], l, "1");
        } else if (splitted[2][0] == 'w' || splitted[2][0] == 'x') {
            printf("REGISTER OFFSETTING\n");
            return registerOffset(splitted[0], splitted[1], splitted[2], l);
        } else if ((splitted[2])[strlen(splitted[2]) - 1] == ']') {
            printf("UNSIGNED OFFSETTING\n");
            return unsignedOffset(splitted[0], splitted[1], splitted[2], l);
        } else {
            printf("POST-INDEX OFFSETTING\n");
            return indexedOffset(splitted[0], splitted[1], splitted[2], l, "0");
        }
    } else {
        if ((splitted[2])[strlen(splitted[2]) - 1] == ']') {
            printf("ZERO UNSIGNED OFFSETTING\n");
            return unsignedOffset(splitted[0], splitted[1], "0", l);
        } else {
            printf("LOADING LITERAL\n");
            return loadLiteral(splitted[0], splitted[1]);
        }
    }
}

char* str (char* arguments, char* address) {
    char** splitted = splitStringOnWhitespace(arguments);
    return dataTransferParser(splitted, "0");
}

char* ldr (char* arguments, char* address) {
    char** splitted = splitStringOnWhitespace(arguments);
    for (int i = 0; i < getStringArrayLength(splitted); i++) {
        printf("%s\n",splitted[i]);
    }
    return dataTransferParser(splitted, "1");
}
