//
// Created by Arabella Hurrell on 01/06/2023.
//

// Helper function to perform left circular shift on a string
char *shiftLeftCircular(char *value, int amount, int length) {
    amount %= length; // Ensure the amount is within the string length

    // Copy the characters from the original string to the shifted string
    char *shifted = malloc((length + 1) * sizeof(char));
    strncpy(shifted, value + amount, length - amount);
    strncpy(shifted + length - amount, value, amount);
    shifted[length] = '\0';

    return shifted;
}

// Helper function to perform right arithmetic or logical shift on a string
char *shiftRightArithmeticLogical(char *value, int amount, int length) {
    int num = strtol(value, NULL, 2);
    int shiftedNum = num >> amount;

    // Create a string to store the shifted value
    char *shifted = malloc((length + 1) * sizeof(char));
    for (int i = length - 1; i >= 0; i--) {
        shifted[i] = (shiftedNum & 1) + '0';
        shiftedNum >>= 1;
    }
    shifted[length] = '\0';

    return shifted;
}

// Function to perform shift operations based on the type and amount
char *master(char *value, char *type, char *amount) {
    if (strcmp(amount, "0") == 0) {
        return value; // No shift required, return the original value
    }

    int length = strlen(value);
    int am = atoi(amount);
    char *shifted;

    if (strcmp(type, "lsl") == 0) {
        // Perform left circular shift
        shifted = shiftLeftCircular(value, am, length);
    } else if (strcmp(type, "lsr") == 0 || strcmp(type, "asr") == 0) {
        // Perform right shift
        shifted = shiftRightArithmeticLogical(value, am, length);
    } else {
        am %= length; // Ensure the amount is within the string length
        shifted = malloc((length + 1) * sizeof(char));
        strncpy(shifted, value + length - am, am);
        strncpy(shifted + am, value, length - am);
        shifted[length] = '\0';
    }

    return shifted;
}
