#include "linalg/Matrix.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iomanip>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <vector>

// Файл реализации класса linalg::Matrix. Каждая функция сопровождается
// подробными комментариями, объясняющими работу с ресурсами и логику шагов,
// чтобы студент мог проследить все решения по управлению памятью.

namespace linalg {

namespace {

// Допуск для сравнения элементов с плавающей точкой.
constexpr double kEqualityEpsilon = 1e-9;

// Проверка на «почти ноль», чтобы стабилизировать вычисления.
bool is_almost_zero(double value) {
    return std::abs(value) <= kEqualityEpsilon;
}

// Принудительно зануляем элементы, если они оказались слишком маленькими.
void zero_if_small(double& value) {
    if (is_almost_zero(value)) {
        value = 0.0;
    }
}

}  // namespace

Matrix::Matrix(std::size_t rows)
    : Matrix(rows, rows == 0 ? 0 : 1) {}

Matrix::Matrix(std::size_t rows, std::size_t columns)
    : m_ptr(nullptr), m_rows(rows), m_columns(columns), m_capacity(rows * columns) {
    // Выделяем память только когда действительно нужно хранить хотя бы один элемент.
    if (m_capacity > 0) {
        // Значения инициализируются по умолчанию, чтобы пользователь получал нули.
        m_ptr = new double[m_capacity]{};
    }
}

Matrix::Matrix(const Matrix& other)
    : m_ptr(nullptr), m_rows(other.m_rows), m_columns(other.m_columns), m_capacity(other.m_capacity) {
    if (m_capacity > 0) {
        // Выделяем память под полный объём вместимости, чтобы сохранить стратегию роста.
        m_ptr = new double[m_capacity];
        // Копируем только активные элементы, остальная память будет использована при расширении.
        std::copy(other.begin(), other.begin() + other.size(), m_ptr);
    }
}

Matrix::Matrix(Matrix&& other) noexcept
    : m_ptr(other.m_ptr), m_rows(other.m_rows), m_columns(other.m_columns), m_capacity(other.m_capacity) {
    // Перемещённый объект приводим к безопасному состоянию, чтобы повторные перемещения и
    // разрушение не нарушали инварианты.
    other.m_ptr = nullptr;
    other.m_rows = 0;
    other.m_columns = 0;
    other.m_capacity = 0;
}

Matrix::Matrix(std::initializer_list<std::initializer_list<double>> rows)
    : m_ptr(nullptr), m_rows(rows.size()), m_columns(0), m_capacity(0) {
    // Определяем количество столбцов по первой строке и проверяем прямоугольность матрицы.
    for (const auto& row : rows) {
        if (m_columns == 0) {
            m_columns = row.size();
        } else if (row.size() != m_columns) {
            throw std::runtime_error("initializer_list содержит строки разной длины");
        }
    }

    // Если хотя бы одна строка непуста, выделяем память и копируем элементы по строкам.
    std::size_t total_elements = m_rows * m_columns;
    m_capacity = total_elements;
    if (total_elements == 0) {
        return;
    }

    m_ptr = new double[total_elements];
    double* destination = m_ptr;
    for (const auto& row : rows) {
        destination = std::copy(row.begin(), row.end(), destination);
    }
}

Matrix::Matrix(std::initializer_list<double> values)
    : m_ptr(nullptr), m_rows(values.size()), m_columns(values.size() == 0 ? 0 : 1), m_capacity(values.size()) {
    // Матрица-столбец просто копирует значения в единственный столбец.
    if (m_capacity == 0) {
        return;
    }

    m_ptr = new double[m_capacity];
    std::copy(values.begin(), values.end(), m_ptr);
}

Matrix& Matrix::operator=(const Matrix& other) {
    if (this == &other) {
        return *this;
    }

    // Увеличиваем вместимость только при необходимости, сохраняя амортизированную стратегию.
    if (m_capacity < other.size()) {
        const std::size_t required_capacity = other.m_capacity;
        double* new_storage = required_capacity > 0 ? new double[required_capacity] : nullptr;
        delete[] m_ptr;
        m_ptr = new_storage;
        m_capacity = required_capacity;
    }

    m_rows = other.m_rows;
    m_columns = other.m_columns;

    if (other.size() > 0) {
        // Копируем активные элементы из источника.
        std::copy(other.begin(), other.begin() + other.size(), m_ptr);
    }

    return *this;
}

Matrix& Matrix::operator=(Matrix&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    // Освобождаем собственные ресурсы, прежде чем забрать буфер у другого объекта.
    delete[] m_ptr;

    m_ptr = other.m_ptr;
    m_rows = other.m_rows;
    m_columns = other.m_columns;
    m_capacity = other.m_capacity;

    // Перемещённый объект оставляем в корректном пустом состоянии.
    other.m_ptr = nullptr;
    other.m_rows = 0;
    other.m_columns = 0;
    other.m_capacity = 0;

    return *this;
}

Matrix::~Matrix() {
    // Пара к выделениям из конструкторов и reserve/shrink_to_fit.
    delete[] m_ptr;
}

bool Matrix::empty() const noexcept {
    return size() == 0;
}

std::size_t Matrix::rows() const noexcept {
    return m_rows;
}

std::size_t Matrix::columns() const noexcept {
    return m_columns;
}

std::size_t Matrix::capacity() const noexcept {
    return m_capacity;
}

std::size_t Matrix::size() const noexcept {
    return m_rows * m_columns;
}

double* Matrix::begin() noexcept {
    return m_ptr;
}

double* Matrix::end() noexcept {
    return m_ptr + size();
}

const double* Matrix::begin() const noexcept {
    return m_ptr;
}

const double* Matrix::end() const noexcept {
    return m_ptr + size();
}

const double* Matrix::cbegin() const noexcept {
    return begin();
}

const double* Matrix::cend() const noexcept {
    return end();
}

void Matrix::reshape(std::size_t rows, std::size_t columns) {
    std::size_t new_size = rows * columns;
    // reshape не должен вызывать перевыделение; при необходимости памяти нужно вызвать reserve().
    if (new_size > m_capacity) {
        throw std::runtime_error("reshape превышает зарезервированную вместимость");
    }

    m_rows = rows;
    m_columns = columns;
}

void Matrix::reserve(std::size_t capacity) {
    if (capacity <= m_capacity) {
        return;
    }

    // Выделяем больший буфер, копируем существующие элементы и освобождаем старое хранилище.
    double* new_storage = new double[capacity];
    std::size_t current_size = size();
    std::copy(begin(), begin() + current_size, new_storage);

    delete[] m_ptr;
    m_ptr = new_storage;
    m_capacity = capacity;
}

void Matrix::clear() noexcept {
    // Вместимость сохраняем, чтобы при последующих вставках не тратить время на выделение памяти.
    m_rows = 0;
    m_columns = 0;
}

void Matrix::shrink_to_fit() {
    std::size_t current_size = size();
    if (current_size == m_capacity) {
        return;
    }

    if (current_size == 0) {
        // Если матрица стала пустой, полностью освобождаем память.
        delete[] m_ptr;
        m_ptr = nullptr;
        m_capacity = 0;
        return;
    }

    // Выделяем буфер точного размера и переносим активные элементы.
    double* new_storage = new double[current_size];
    std::copy(begin(), begin() + current_size, new_storage);
    delete[] m_ptr;
    m_ptr = new_storage;
    m_capacity = current_size;
}

void Matrix::swap(Matrix& other) noexcept {
    using std::swap;
    // Обмениваем каждый ресурс отдельно, что обеспечивает строгую гарантию
    // исключений (новое выделение памяти не выполняется).
    swap(m_ptr, other.m_ptr);
    swap(m_rows, other.m_rows);
    swap(m_columns, other.m_columns);
    swap(m_capacity, other.m_capacity);
}

double& Matrix::operator()(std::size_t row, std::size_t column) {
    // Проверка границ защищает инварианты при отладке и в учебных сценариях.
    if (row >= m_rows || column >= m_columns) {
        throw std::out_of_range("индекс матрицы выходит за пределы");
    }

    return m_ptr[row * m_columns + column];
}

const double& Matrix::operator()(std::size_t row, std::size_t column) const {
    // Константная версия повторяет логику и позволяет читать элементы.
    if (row >= m_rows || column >= m_columns) {
        throw std::out_of_range("индекс матрицы выходит за пределы");
    }

    return m_ptr[row * m_columns + column];
}

Matrix Matrix::operator+() const {
    // Возвращаем копию текущей матрицы без модификаций.
    return Matrix(*this);
}

Matrix Matrix::operator-() const {
    Matrix result(*this);
    for (std::size_t index = 0; index < result.size(); ++index) {
        result.m_ptr[index] = -result.m_ptr[index];
    }
    return result;
}

Matrix& Matrix::operator+=(const Matrix& other) {
    if (m_rows != other.m_rows || m_columns != other.m_columns) {
        throw std::runtime_error("сложение возможно только для матриц одинакового размера");
    }

    for (std::size_t index = 0; index < size(); ++index) {
        m_ptr[index] += other.m_ptr[index];
    }

    return *this;
}

Matrix& Matrix::operator-=(const Matrix& other) {
    if (m_rows != other.m_rows || m_columns != other.m_columns) {
        throw std::runtime_error("вычитание возможно только для матриц одинакового размера");
    }

    for (std::size_t index = 0; index < size(); ++index) {
        m_ptr[index] -= other.m_ptr[index];
    }

    return *this;
}

Matrix& Matrix::operator*=(const Matrix& other) {
    *this = linalg::operator*(*this, other);
    return *this;
}

Matrix& Matrix::operator*=(double scalar) noexcept {
    for (std::size_t index = 0; index < size(); ++index) {
        m_ptr[index] *= scalar;
    }
    return *this;
}

double Matrix::norm() const noexcept {
    double sum = 0.0;
    for (std::size_t index = 0; index < size(); ++index) {
        sum += m_ptr[index] * m_ptr[index];
    }
    return std::sqrt(sum);
}

double Matrix::trace() const {
    if (m_rows != m_columns) {
        throw std::runtime_error("след определён только для квадратных матриц");
    }

    double result = 0.0;
    for (std::size_t index = 0; index < m_rows; ++index) {
        result += m_ptr[index * m_columns + index];
    }
    return result;
}

double Matrix::det() const {
    if (m_rows != m_columns) {
        throw std::runtime_error("определитель определён только для квадратных матриц");
    }

    const std::size_t dimension = m_rows;
    if (dimension == 0) {
        // Определитель пустой матрицы по соглашению равен единице.
        return 1.0;
    }

    Matrix temp(*this);
    int swap_sign = 1;

    for (std::size_t column = 0; column < dimension; ++column) {
        // Ищем ведущий элемент в текущем столбце (частичный выбор главного элемента).
        std::size_t pivot_row = column;
        double pivot_value = std::abs(temp.m_ptr[column * temp.m_columns + column]);
        for (std::size_t row = column + 1; row < dimension; ++row) {
            double candidate = std::abs(temp.m_ptr[row * temp.m_columns + column]);
            if (candidate > pivot_value) {
                pivot_value = candidate;
                pivot_row = row;
            }
        }

        if (is_almost_zero(pivot_value)) {
            return 0.0;
        }

        if (pivot_row != column) {
            for (std::size_t col = 0; col < temp.m_columns; ++col) {
                std::swap(temp.m_ptr[pivot_row * temp.m_columns + col],
                          temp.m_ptr[column * temp.m_columns + col]);
            }
            swap_sign = -swap_sign;
        }

        double pivot = temp.m_ptr[column * temp.m_columns + column];
        for (std::size_t row = column + 1; row < dimension; ++row) {
            double factor = temp.m_ptr[row * temp.m_columns + column] / pivot;
            if (is_almost_zero(factor)) {
                temp.m_ptr[row * temp.m_columns + column] = 0.0;
                continue;
            }
            for (std::size_t col = column; col < dimension; ++col) {
                temp.m_ptr[row * temp.m_columns + col] -=
                    factor * temp.m_ptr[column * temp.m_columns + col];
                zero_if_small(temp.m_ptr[row * temp.m_columns + col]);
            }
        }
    }

    double determinant = static_cast<double>(swap_sign);
    for (std::size_t index = 0; index < dimension; ++index) {
        determinant *= temp.m_ptr[index * temp.m_columns + index];
    }
    return determinant;
}

std::size_t Matrix::rank() const {
    Matrix temp(*this);
    temp.gauss_forward();

    std::size_t rank = 0;
    for (std::size_t row = 0; row < temp.m_rows; ++row) {
        bool has_nonzero = false;
        for (std::size_t column = 0; column < temp.m_columns; ++column) {
            if (!is_almost_zero(temp.m_ptr[row * temp.m_columns + column])) {
                has_nonzero = true;
                break;
            }
        }
        if (has_nonzero) {
            ++rank;
        }
    }
    return rank;
}

Matrix& Matrix::gauss_forward() {
    std::size_t pivot_row = 0;
    for (std::size_t column = 0; column < m_columns && pivot_row < m_rows; ++column) {
        // Находим строку с максимальным модулем элемента в текущем столбце.
        std::size_t best_row = pivot_row;
        double best_value = std::abs(m_ptr[pivot_row * m_columns + column]);
        for (std::size_t row = pivot_row + 1; row < m_rows; ++row) {
            double candidate = std::abs(m_ptr[row * m_columns + column]);
            if (candidate > best_value) {
                best_value = candidate;
                best_row = row;
            }
        }

        if (is_almost_zero(best_value)) {
            continue;
        }

        if (best_row != pivot_row) {
            for (std::size_t col = 0; col < m_columns; ++col) {
                std::swap(m_ptr[pivot_row * m_columns + col],
                          m_ptr[best_row * m_columns + col]);
            }
        }

        double pivot = m_ptr[pivot_row * m_columns + column];
        for (std::size_t col = column; col < m_columns; ++col) {
            m_ptr[pivot_row * m_columns + col] /= pivot;
            zero_if_small(m_ptr[pivot_row * m_columns + col]);
        }

        for (std::size_t row = pivot_row + 1; row < m_rows; ++row) {
            double factor = m_ptr[row * m_columns + column];
            if (is_almost_zero(factor)) {
                m_ptr[row * m_columns + column] = 0.0;
                continue;
            }
            for (std::size_t col = column; col < m_columns; ++col) {
                m_ptr[row * m_columns + col] -= factor * m_ptr[pivot_row * m_columns + col];
                zero_if_small(m_ptr[row * m_columns + col]);
            }
        }

        ++pivot_row;
    }

    return *this;
}

Matrix& Matrix::gauss_backward() {
    if (m_rows == 0 || m_columns == 0) {
        return *this;
    }

    for (std::ptrdiff_t row = static_cast<std::ptrdiff_t>(m_rows) - 1; row >= 0; --row) {
        double* row_data = m_ptr + static_cast<std::size_t>(row) * m_columns;
        std::size_t pivot_column = m_columns;
        for (std::size_t column = 0; column < m_columns; ++column) {
            if (!is_almost_zero(row_data[column])) {
                pivot_column = column;
                break;
            }
        }

        if (pivot_column == m_columns) {
            continue;
        }

        double pivot = row_data[pivot_column];
        for (std::size_t column = pivot_column; column < m_columns; ++column) {
            row_data[column] /= pivot;
            zero_if_small(row_data[column]);
        }

        for (std::ptrdiff_t upper = row - 1; upper >= 0; --upper) {
            double* upper_row = m_ptr + static_cast<std::size_t>(upper) * m_columns;
            double factor = upper_row[pivot_column];
            if (is_almost_zero(factor)) {
                upper_row[pivot_column] = 0.0;
                continue;
            }
            for (std::size_t column = pivot_column; column < m_columns; ++column) {
                upper_row[column] -= factor * row_data[column];
                zero_if_small(upper_row[column]);
            }
        }
    }

    return *this;
}

void swap(Matrix& lhs, Matrix& rhs) noexcept {
    // Вызываем метод swap, чтобы централизовать логику обмена.
    lhs.swap(rhs);
}

std::ostream& operator<<(std::ostream& os, const Matrix& matrix) {
    // Пустая матрица выводится как пара вертикальных черт без содержимого.
    if (matrix.empty()) {
        os << "| |";
        return os;
    }

    const std::size_t rows = matrix.rows();
    const std::size_t columns = matrix.columns();
    const double* data = matrix.begin();

    // Для выравнивания вычисляем максимальную ширину строки в каждом столбце.
    std::vector<std::size_t> column_widths(columns, 0);
    for (std::size_t column = 0; column < columns; ++column) {
        for (std::size_t row = 0; row < rows; ++row) {
            std::ostringstream formatter;
            formatter << data[row * columns + column];
            column_widths[column] = std::max(column_widths[column], formatter.str().size());
        }
    }

    // Последовательно выводим каждую строку, выравнивая элементы по вычисленным ширинам.
    for (std::size_t row = 0; row < rows; ++row) {
        os << '|';
        for (std::size_t column = 0; column < columns; ++column) {
            std::ostringstream formatter;
            formatter << data[row * columns + column];
            os << ' ' << std::setw(static_cast<int>(column_widths[column])) << formatter.str();
        }
        os << '|';
        if (row + 1 != rows) {
            os << '\n';
        }
    }

    return os;
}

Matrix operator+(const Matrix& lhs, const Matrix& rhs) {
    Matrix result(lhs);
    result += rhs;
    return result;
}

Matrix operator-(const Matrix& lhs, const Matrix& rhs) {
    Matrix result(lhs);
    result -= rhs;
    return result;
}

Matrix operator*(const Matrix& lhs, const Matrix& rhs) {
    if (lhs.columns() != rhs.rows()) {
        throw std::runtime_error("для матричного произведения число столбцов левой матрицы должно равняться числу строк правой");
    }

    Matrix result(lhs.rows(), rhs.columns());
    const std::size_t shared = lhs.columns();
    const std::size_t lhs_columns = lhs.columns();
    const std::size_t rhs_columns = rhs.columns();

    const double* lhs_data = lhs.begin();
    const double* rhs_data = rhs.begin();
    double* result_data = result.begin();

    for (std::size_t row = 0; row < lhs.rows(); ++row) {
        for (std::size_t column = 0; column < rhs_columns; ++column) {
            double value = 0.0;
            for (std::size_t k = 0; k < shared; ++k) {
                value += lhs_data[row * lhs_columns + k] * rhs_data[k * rhs_columns + column];
            }
            result_data[row * rhs_columns + column] = value;
        }
    }

    return result;
}

Matrix operator*(const Matrix& matrix, double scalar) {
    Matrix result(matrix);
    result *= scalar;
    return result;
}

Matrix operator*(double scalar, const Matrix& matrix) {
    return matrix * scalar;
}

bool operator==(const Matrix& lhs, const Matrix& rhs) noexcept {
    if (lhs.rows() != rhs.rows() || lhs.columns() != rhs.columns()) {
        return false;
    }

    const std::size_t total = lhs.size();
    const double* lhs_data = lhs.begin();
    const double* rhs_data = rhs.begin();
    for (std::size_t index = 0; index < total; ++index) {
        if (std::abs(lhs_data[index] - rhs_data[index]) > kEqualityEpsilon) {
            return false;
        }
    }

    return true;
}

bool operator!=(const Matrix& lhs, const Matrix& rhs) noexcept {
    return !(lhs == rhs);
}

Matrix concatenate(const Matrix& left, const Matrix& right) {
    if (left.rows() != right.rows()) {
        throw std::runtime_error("для конкатенации требуется одинаковое число строк");
    }

    Matrix result(left.rows(), left.columns() + right.columns());
    const double* left_data = left.begin();
    const double* right_data = right.begin();
    const std::size_t left_columns = left.columns();
    const std::size_t right_columns = right.columns();
    for (std::size_t row = 0; row < left.rows(); ++row) {
        // Сначала копируем элементы левой матрицы.
        for (std::size_t column = 0; column < left_columns; ++column) {
            result(row, column) = left_data[row * left_columns + column];
        }
        // Затем добавляем элементы правой матрицы.
        for (std::size_t column = 0; column < right_columns; ++column) {
            result(row, left_columns + column) = right_data[row * right_columns + column];
        }
    }
    return result;
}

Matrix transpose(const Matrix& matrix) {
    Matrix result(matrix.columns(), matrix.rows());
    const double* data = matrix.begin();
    const std::size_t rows = matrix.rows();
    const std::size_t columns = matrix.columns();
    for (std::size_t row = 0; row < rows; ++row) {
        for (std::size_t column = 0; column < columns; ++column) {
            result(column, row) = data[row * columns + column];
        }
    }
    return result;
}

Matrix invert(const Matrix& matrix) {
    if (matrix.rows() != matrix.columns()) {
        throw std::runtime_error("обратная матрица существует только для квадратных матриц");
    }

    const std::size_t dimension = matrix.rows();
    Matrix augmented(dimension, dimension * 2);
    const double* source = matrix.begin();

    for (std::size_t row = 0; row < dimension; ++row) {
        for (std::size_t column = 0; column < dimension; ++column) {
            augmented(row, column) = source[row * dimension + column];
        }
        for (std::size_t column = 0; column < dimension; ++column) {
            augmented(row, dimension + column) = (row == column) ? 1.0 : 0.0;
        }
    }

    augmented.gauss_forward();
    augmented.gauss_backward();

    // Проверяем, что левая часть стала единичной матрицей; иначе исходная матрица вырождена.
    for (std::size_t row = 0; row < dimension; ++row) {
        for (std::size_t column = 0; column < dimension; ++column) {
            double value = augmented(row, column);
            if (row == column) {
                if (!is_almost_zero(value - 1.0)) {
                    throw std::runtime_error("матрица вырождена, обратной не существует");
                }
            } else if (!is_almost_zero(value)) {
                throw std::runtime_error("матрица вырождена, обратной не существует");
            }
        }
    }

    Matrix inverse(dimension, dimension);
    for (std::size_t row = 0; row < dimension; ++row) {
        for (std::size_t column = 0; column < dimension; ++column) {
            inverse(row, column) = augmented(row, dimension + column);
        }
    }

    return inverse;
}

Matrix power(const Matrix& matrix, std::size_t exponent) {
    if (matrix.rows() != matrix.columns()) {
        throw std::runtime_error("возведение в степень определено только для квадратных матриц");
    }

    const std::size_t dimension = matrix.rows();
    Matrix result(dimension, dimension);
    for (std::size_t index = 0; index < dimension; ++index) {
        result(index, index) = 1.0;
    }

    if (exponent == 0) {
        return result;
    }

    Matrix base(matrix);
    std::size_t current_power = exponent;
    while (current_power > 0) {
        if (current_power & 1U) {
            result = result * base;
        }
        current_power >>= 1U;
        if (current_power > 0) {
            base = base * base;
        }
    }

    return result;
}

Matrix solve(const Matrix& a, const Matrix& f) {
    if (a.rows() != a.columns()) {
        throw std::runtime_error("для решения системы требуется квадратная матрица коэффициентов");
    }
    if (a.rows() != f.rows()) {
        throw std::runtime_error("число строк в матрицах A и f должно совпадать");
    }

    const std::size_t dimension = a.rows();
    const std::size_t rhs_columns = f.columns();
    Matrix augmented(dimension, dimension + rhs_columns);
    const double* a_data = a.begin();
    const double* f_data = f.begin();

    for (std::size_t row = 0; row < dimension; ++row) {
        for (std::size_t column = 0; column < dimension; ++column) {
            augmented(row, column) = a_data[row * dimension + column];
        }
        for (std::size_t column = 0; column < rhs_columns; ++column) {
            augmented(row, dimension + column) = f_data[row * rhs_columns + column];
        }
    }

    augmented.gauss_forward();
    augmented.gauss_backward();

    for (std::size_t row = 0; row < dimension; ++row) {
        for (std::size_t column = 0; column < dimension; ++column) {
            double value = augmented(row, column);
            if (row == column) {
                if (!is_almost_zero(value - 1.0)) {
                    throw std::runtime_error("матрица коэффициентов вырождена, систему решить нельзя");
                }
            } else if (!is_almost_zero(value)) {
                throw std::runtime_error("матрица коэффициентов вырождена, систему решить нельзя");
            }
        }
    }

    Matrix solution(dimension, rhs_columns);
    for (std::size_t row = 0; row < dimension; ++row) {
        for (std::size_t column = 0; column < rhs_columns; ++column) {
            solution(row, column) = augmented(row, dimension + column);
        }
    }

    return solution;
}

}  // namespace linalg

