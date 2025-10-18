add_executable(matrix_full_tests
    Matrix_full_tests.cpp
)

target_link_libraries(matrix_full_tests
    PRIVATE
        linalg
        gtest
)

target_compile_features(matrix_full_tests PRIVATE cxx_std_20)

target_compile_options(matrix_full_tests PRIVATE -Wall -Wextra -Wpedantic)

add_test(NAME matrix_full_tests COMMAND matrix_full_tests)
