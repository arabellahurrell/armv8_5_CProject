/*
 * Emulate header file
 */

#define WORD_BYTES       4
#define NO_REGISTERS     31
#define ZERO_REGISTER    0b11111
#define NO_BYTES_MEMORY  0x200000
#define NOP_INSTRUCTION  0xD503201F
#define HALT_INSTRUCTION 0x8A000000

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "structures.c"
#include "binary_helpers.c"
#include "arithmetic.c"
#include "file_helpers.c"
#include "emulate_branch.c"
#include "emulate_shifts.c"
#include "dp_immediate.c"
#include "dp_register.c"
#include "load_or_store.c"
#include "emulate_tests.c"