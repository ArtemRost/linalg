cmake_minimum_required(VERSION 3.20)

project(linalg LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(linalg
    source/Matrix.cpp
)

target_include_directories(linalg
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(linalg PUBLIC cxx_std_20)

# Enable stricter warnings for the library during development. Consumers can
# still adjust warning policies according to their needs.
if (MSVC)
    target_compile_options(linalg PRIVATE /W4 /permissive-)
else()
    target_compile_options(linalg PRIVATE -Wall -Wextra -Wpedantic)
endif()

option(LINALG_BUILD_EXAMPLES "Собирать вспомогательные примеры для ручных проверок" OFF)

if (LINALG_BUILD_EXAMPLES)
    add_executable(linalg_buffer_demo
        examples/buffer_management_demo.cpp
    )

    target_link_libraries(linalg_buffer_demo PRIVATE linalg)

    if (MSVC)
        target_compile_options(linalg_buffer_demo PRIVATE /W4 /permissive-)
    else()
        target_compile_options(linalg_buffer_demo PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endif()

