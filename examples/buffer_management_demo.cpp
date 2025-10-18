#include "linalg/Matrix.hpp"

#include <iostream>

int main()
{
    // Демонстрационный пример для второго шага лабораторной работы.
    // Показывает, как изменяются размеры и вместимость матрицы.
    linalg::Matrix matrix = {{1.0, 2.0}, {3.0, 4.0}};

    std::cout << "Стартовый размер: " << matrix.rows() << "x" << matrix.columns()
              << ", вместимость: " << matrix.capacity() << '\n';

    matrix.reserve(12);
    std::cout << "После reserve(12) вместимость: " << matrix.capacity() << '\n';

    matrix.reshape(3, 2);
    std::cout << "После reshape(3, 2) размер: " << matrix.rows() << "x" << matrix.columns()
              << ", вместимость: " << matrix.capacity() << '\n';

    double value = 0.0;
    for (std::size_t r = 0; r < matrix.rows(); ++r) {
        for (std::size_t c = 0; c < matrix.columns(); ++c) {
            matrix(r, c) = value++;
        }
    }

    matrix.clear();
    std::cout << "После clear() размер: " << matrix.rows() << "x" << matrix.columns()
              << ", вместимость сохраняется: " << matrix.capacity() << '\n';

    matrix.reshape(2, 6);
    std::cout << "После reshape(2, 6) размер: " << matrix.rows() << "x" << matrix.columns()
              << ", вместимость: " << matrix.capacity() << '\n';

    matrix.shrink_to_fit();
    std::cout << "После shrink_to_fit() вместимость: " << matrix.capacity() << '\n';

    linalg::Matrix other(1, 12);
    matrix.swap(other);
    std::cout << "После swap размеры поменялись местами: matrix = "
              << matrix.rows() << "x" << matrix.columns() << ", other = "
              << other.rows() << "x" << other.columns() << '\n';

    return 0;
}
