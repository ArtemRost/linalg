#pragma once

#include <cstddef>
#include <initializer_list>

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

}  // namespace linalg

