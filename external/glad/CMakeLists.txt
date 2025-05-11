# external/glad/CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(glad C) # GLAD is a C library

add_library(glad STATIC
    src/glad.c
)

target_include_directories(glad PUBLIC
    include
)
