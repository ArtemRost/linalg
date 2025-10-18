#pragma once

#include <cstddef>
#include <initializer_list>
#include <iosfwd>

namespace linalg {

/**
 * @brief Матрица произвольного размера, использующая один непрерывный
 *        динамический массив для хранения элементов.
 *
 * Класс полностью владеет собственной памятью и поддерживает классическую
 * семантику значений (копирование и перемещение). В интерфейсе отражены все
 * требования лабораторной работы, а подробные описания помогают быстрее
 * ориентироваться при изучении кода.
 */
class Matrix {
public:
    /**
     * @brief Создать пустую матрицу с нулевым числом строк и столбцов.
     */
    Matrix() = default;

    /**
     * @brief Создать матрицу с @p rows строками и одним столбцом.
     *
     * Каждый элемент инициализируется значением по умолчанию (для double это 0).
     */
    explicit Matrix(std::size_t rows);

    /**
     * @brief Создать матрицу с @p rows строками и @p columns столбцами.
     *
     * Память выделяется как единый динамический массив размером rows * columns
     * и заполняется значениями по умолчанию.
     */
    Matrix(std::size_t rows, std::size_t columns);

    /**
     * @brief Конструктор копирования, выполняющий глубокое копирование элементов.
     */
    Matrix(const Matrix& other);

    /**
     * @brief Конструктор перемещения, передающий владение внутренним буфером.
     */
    Matrix(Matrix&& other) noexcept;

    /**
     * @brief Создать матрицу из списка списков значений.
     *
     * Внешний список трактуется как строки матрицы, внутренние списки — как
     * столбцы. Все строки должны иметь одинаковую длину, иначе будет брошено
     * исключение.
     */
    Matrix(std::initializer_list<std::initializer_list<double>> rows);

    /**
     * @brief Создать матрицу-столбец из линейного списка значений.
     *
     * Каждый элемент попадает в отдельную строку, число столбцов равно единице
     * (если список пуст, матрица остаётся пустой).
     */
    Matrix(std::initializer_list<double> values);

    /**
     * @brief Оператор копирующего присваивания. Повторно использует существующую
     *        вместимость, если она достаточна.
     */
    Matrix& operator=(const Matrix& other);

    /**
     * @brief Оператор перемещающего присваивания. Освобождает свои ресурсы и
     *        забирает буфер у объекта @p other.
     */
    Matrix& operator=(Matrix&& other) noexcept;

    /**
     * @brief Деструктор, освобождающий выделенную память.
     */
    ~Matrix();

    /**
     * @brief Проверить, содержит ли матрица ноль элементов.
     */
    [[nodiscard]] bool empty() const noexcept;

    /**
     * @brief Получить текущее количество строк.
     */
    [[nodiscard]] std::size_t rows() const noexcept;

    /**
     * @brief Получить текущее количество столбцов.
     */
    [[nodiscard]] std::size_t columns() const noexcept;

    /**
     * @brief Узнать резервируемую вместимость (в элементах) внутреннего буфера.
     */
    [[nodiscard]] std::size_t capacity() const noexcept;

    /**
     * @brief Получить фактическое количество элементов (rows * columns).
     */
    [[nodiscard]] std::size_t size() const noexcept;

    /**
     * @brief Вернуть указатель на первый элемент для поддержки range-based for.
     */
    double* begin() noexcept;

    /**
     * @brief Вернуть указатель на элемент, следующий за последним.
     */
    double* end() noexcept;

    /**
     * @brief Константная версия begin() — указатель на первый элемент.
     */
    const double* begin() const noexcept;

    /**
     * @brief Константная версия end() — указатель за последним элементом.
     */
    const double* end() const noexcept;

    /**
     * @brief Удобный алиас для begin() в константном контексте.
     */
    const double* cbegin() const noexcept;

    /**
     * @brief Удобный алиас для end() в константном контексте.
     */
    const double* cend() const noexcept;

    /**
     * @brief Изменить логический размер матрицы без перевыделения памяти.
     * @throws std::runtime_error если новый размер превышает текущую вместимость.
     */
    void reshape(std::size_t rows, std::size_t columns);

    /**
     * @brief Зарезервировать память минимум под @p capacity элементов.
     */
    void reserve(std::size_t capacity);

    /**
     * @brief Очистить матрицу, оставив выделенную память нетронутой.
     */
    void clear() noexcept;

    /**
     * @brief Урезать вместимость до текущего логического размера.
     */
    void shrink_to_fit();

    /**
     * @brief Обменять содержимое с другой матрицей.
     */
    void swap(Matrix& other) noexcept;

    /**
     * @brief Доступ к элементу с проверкой границ (изменяемый вариант).
     * @throws std::out_of_range при обращении за пределами размерности.
     */
    double& operator()(std::size_t row, std::size_t column);

    /**
     * @brief Доступ к элементу с проверкой границ (константный вариант).
     * @throws std::out_of_range при обращении за пределами размерности.
     */
    const double& operator()(std::size_t row, std::size_t column) const;

    /**
     * @brief Унарный плюс возвращает копию матрицы без изменений.
     */
    Matrix operator+() const;

    /**
     * @brief Унарный минус создаёт новую матрицу с противоположными элементами.
     */
    Matrix operator-() const;

    /**
     * @brief Сложить текущую матрицу с @p other поэлементно.
     * @throws std::runtime_error при несовпадающих размерах.
     */
    Matrix& operator+=(const Matrix& other);

    /**
     * @brief Вычесть матрицу @p other поэлементно из текущей.
     * @throws std::runtime_error при несовпадающих размерах.
     */
    Matrix& operator-=(const Matrix& other);

    /**
     * @brief Умножить текущую матрицу на @p other (матричное произведение).
     * @throws std::runtime_error при несовместимых размерах.
     */
    Matrix& operator*=(const Matrix& other);

    /**
     * @brief Поэлементно умножить матрицу на скаляр @p scalar.
     */
    Matrix& operator*=(double scalar) noexcept;

    /**
     * @brief Вычислить норму Фробениуса, то есть квадратный корень из суммы
     *        квадратов всех элементов.
     */
    [[nodiscard]] double norm() const noexcept;

    /**
     * @brief Найти след матрицы как сумму диагональных элементов.
     * @throws std::runtime_error если матрица не является квадратной.
     */
    [[nodiscard]] double trace() const;

    /**
     * @brief Вычислить определитель текущей матрицы.
     * @throws std::runtime_error если матрица не квадратная.
     */
    [[nodiscard]] double det() const;

    /**
     * @brief Определить ранг матрицы методом Гаусса.
     */
    [[nodiscard]] std::size_t rank() const;

    /**
     * @brief Выполнить прямой ход метода Гаусса (приведение к верхнетреугольному виду).
     *
     * Строки нормализуются так, чтобы ведущие элементы равнялись единице.
     */
    Matrix& gauss_forward();

    /**
     * @brief Выполнить обратный ход метода Гаусса, доведя матрицу до
     *        приведённого ступенчатого вида.
     */
    Matrix& gauss_backward();

private:
    double* m_ptr = nullptr;
    std::size_t m_rows = 0;
    std::size_t m_columns = 0;
    std::size_t m_capacity = 0;
};

/**
 * @brief Глобальная функция swap для поддержки ADL и std::swap.
 */
void swap(Matrix& lhs, Matrix& rhs) noexcept;

/**
 * @brief Вывести матрицу в поток с выравниванием столбцов.
 *
 * Каждая строка окружена символами вертикальной черты, а элементы в столбцах
 * выровнены по правому краю относительно самого широкого представления в этом
 * столбце. Такой формат удобен для визуальной проверки результатов вычислений.
 */
std::ostream& operator<<(std::ostream& os, const Matrix& matrix);

/**
 * @brief Поэлементное сложение двух матриц одинакового размера.
 * @throws std::runtime_error при несовпадающих размерах.
 */
Matrix operator+(const Matrix& lhs, const Matrix& rhs);

/**
 * @brief Поэлементное вычитание матриц одинакового размера.
 * @throws std::runtime_error при несовпадающих размерах.
 */
Matrix operator-(const Matrix& lhs, const Matrix& rhs);

/**
 * @brief Матричное произведение совместимых матриц.
 * @throws std::runtime_error при несовместимых размерах.
 */
Matrix operator*(const Matrix& lhs, const Matrix& rhs);

/**
 * @brief Поэлементное умножение матрицы на скаляр.
 */
Matrix operator*(const Matrix& matrix, double scalar);

/**
 * @brief Поэлементное умножение скаляра на матрицу (коммутативный случай).
 */
Matrix operator*(double scalar, const Matrix& matrix);

/**
 * @brief Проверка матриц на равенство с учётом допуска по вещественным числам.
 */
bool operator==(const Matrix& lhs, const Matrix& rhs) noexcept;

/**
 * @brief Проверка матриц на неравенство с учётом допуска по вещественным числам.
 */
bool operator!=(const Matrix& lhs, const Matrix& rhs) noexcept;

/**
 * @brief Соединить две матрицы по горизонтали (слева направо).
 * @throws std::runtime_error если количество строк не совпадает.
 */
Matrix concatenate(const Matrix& left, const Matrix& right);

/**
 * @brief Построить транспонированную матрицу.
 */
Matrix transpose(const Matrix& matrix);

/**
 * @brief Вычислить обратную матрицу с помощью метода Гаусса-Жордана.
 * @throws std::runtime_error если матрица не квадратная или вырождена.
 */
Matrix invert(const Matrix& matrix);

/**
 * @brief Возвести матрицу в натуральную степень.
 * @throws std::runtime_error если матрица не квадратная.
 */
Matrix power(const Matrix& matrix, std::size_t exponent);

/**
 * @brief Решить систему уравнений A * x = f методом Гаусса.
 * @throws std::runtime_error при несовместимых размерностях или вырожденной матрице A.
 */
Matrix solve(const Matrix& a, const Matrix& f);

}  // namespace linalg

