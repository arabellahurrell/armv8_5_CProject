cmake_minimum_required(VERSION 3.25)
project(armv8_5 C)

set(CMAKE_C_STANDARD 11)

include_directories(src)

add_executable(armv8_5
        src/assemble.c
        src/emulate.c src/symbol_table.c src/symbol_table.h src/reading_source_file.c src/reading_source_file.h src/function_decoder.c src/function_decoder.h src/assemble/enums.c src/assemble/enums.h)
