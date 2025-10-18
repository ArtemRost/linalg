#include "linalg/Matrix.hpp"

#include <algorithm>
#include <stdexcept>

// Файл реализации класса linalg::Matrix. Каждая функция сопровождается
// подробными комментариями, объясняющими работу с ресурсами и логику шагов,
// чтобы студент мог проследить все решения по управлению памятью.

namespace linalg {

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

Matrix& Matrix::operator=(const Matrix& other) {
    if (this == &other) {
        return *this;
    }

    // Увеличиваем вместимость только при необходимости, сохраняя амортизированную стратегию.
    if (m_capacity < other.size()) {
        double* new_storage = other.size() > 0 ? new double[other.size()] : nullptr;
        delete[] m_ptr;
        m_ptr = new_storage;
        m_capacity = other.size();
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

void swap(Matrix& lhs, Matrix& rhs) noexcept {
    // Вызываем метод swap, чтобы централизовать логику обмена.
    lhs.swap(rhs);
}

}  // namespace linalg

