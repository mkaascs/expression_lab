cmake_minimum_required(VERSION 3.28)
project(2 C)

set(CMAKE_C_STANDARD 11)

add_executable(2 main.c
        memory/stats.c
        memory/stats.h
        application/domain/models.h
        application/domain/models.c
        application/domain/commands/expressions.h
        application/domain/commands/expressions.c
        application/parsing/parser.h
        application/parsing/parser.c
        application/domain/converting/converter.h
        application/domain/converting/converter.c
)
