cmake_minimum_required(VERSION 3.20)
project(MatrixMultiplication C)

set(CMAKE_C_STANDARD 99)

add_executable(MatrixMultiplication
        main.c
        Strings/Headers/String.h
        Strings/Sources/String.c
        Lists/Headers/Vector.h
        Lists/Sources/Vector.c
        FilesHandler/Headers/TxtFileLoader.h
        FilesHandler/Sources/TxtFileLoader.c
        System/ErrorCode.h
        System/Utils.h)
