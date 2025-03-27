cmake_minimum_required(VERSION 3.28)
project(2 C)

set(CMAKE_C_STANDARD 11)

add_executable(2 main.c
        memory/stats.c
        memory/stats.h
        application/domain/models.h
        application/domain/models.c
        application/operations.h
        application/operations.c)
