cmake_minimum_required(VERSION 3.16)

add_library(gtest STATIC
    src/gtest_main.cpp
)

target_include_directories(gtest PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_compile_features(gtest PUBLIC cxx_std_20)

target_compile_options(gtest PRIVATE -Wall -Wextra -Wpedantic)
