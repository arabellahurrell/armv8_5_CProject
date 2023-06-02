cmake_minimum_required(VERSION 3.25)
project(armv8_5 C)

set(CMAKE_C_STANDARD 11)

include_directories(src)

add_executable(armv8_5
        src/assemble.c
        src/emulate.c src/symbol_table.c src/symbol_table.h src/reading_source_file.c src/reading_source_file.h src/function_decoder.c src/function_decoder.h src/data_processing.c src/data_processing.h src/single_data_transfer.c src/single_data_transfer.h src/branching.c src/branching.h src/denary_to_binary.c src/denary_to_binary.h src/shifts.c src/shifts.h src/branch.c src/branch.h src/passing.c src/passing.h)
