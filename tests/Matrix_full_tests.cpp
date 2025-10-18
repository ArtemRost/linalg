#include <gtest/gtest.h>

#include <cmath>
#include <stdexcept>

#include "linalg/Matrix.hpp"

namespace {

constexpr double kEps = 1e-9;

}  // namespace

TEST(Constructors, DefaultAndSized) {
    linalg::Matrix empty;
    EXPECT_TRUE(empty.empty());
    EXPECT_EQ(empty.rows(), 0U);
    EXPECT_EQ(empty.columns(), 0U);

    linalg::Matrix column(3);
    EXPECT_EQ(column.rows(), 3U);
    EXPECT_EQ(column.columns(), 1U);
    for (double value : column) {
        EXPECT_DOUBLE_EQ(value, 0.0);
    }

    linalg::Matrix rect(2, 4);
    EXPECT_EQ(rect.rows(), 2U);
    EXPECT_EQ(rect.columns(), 4U);
    EXPECT_EQ(rect.size(), 8U);
}

TEST(Constructors, InitializerLists) {
    linalg::Matrix rows = {{1.0, 2.0}, {3.0, 4.0}};
    EXPECT_EQ(rows.rows(), 2U);
    EXPECT_EQ(rows.columns(), 2U);
    EXPECT_DOUBLE_EQ(rows(0, 1), 2.0);
    EXPECT_DOUBLE_EQ(rows(1, 0), 3.0);

    linalg::Matrix column = {1.0, 2.0, 3.0};
    EXPECT_EQ(column.rows(), 3U);
    EXPECT_EQ(column.columns(), 1U);
    EXPECT_DOUBLE_EQ(column(2, 0), 3.0);

    EXPECT_THROW((linalg::Matrix{{1.0}, {2.0, 3.0}}), std::runtime_error);
}

TEST(Semantics, CopyAndMove) {
    linalg::Matrix base = {{1.0, 2.0}, {3.0, 4.0}};
    linalg::Matrix copy(base);
    EXPECT_TRUE(base == copy);

    linalg::Matrix assigned;
    assigned = copy;
    EXPECT_TRUE(assigned == copy);

    linalg::Matrix moved(std::move(copy));
    EXPECT_TRUE(moved == base);

    linalg::Matrix move_assigned;
    move_assigned = std::move(moved);
    EXPECT_TRUE(move_assigned == base);
}

TEST(BufferManagement, ReserveAndReshape) {
    linalg::Matrix matrix(2, 2);
    matrix.reserve(16);
    EXPECT_TRUE(matrix.capacity() >= 16U);

    matrix.reshape(4, 4);
    EXPECT_EQ(matrix.rows(), 4U);
    EXPECT_EQ(matrix.columns(), 4U);
    EXPECT_EQ(matrix.size(), 16U);

    matrix.clear();
    EXPECT_TRUE(matrix.empty());
    EXPECT_EQ(matrix.capacity(), 16U);

    matrix.shrink_to_fit();
    EXPECT_EQ(matrix.capacity(), 0U);
}

TEST(BufferManagement, Swap) {
    linalg::Matrix lhs = {{1.0, 2.0}, {3.0, 4.0}};
    linalg::Matrix rhs = {{5.0, 6.0}, {7.0, 8.0}};

    lhs.swap(rhs);
    EXPECT_DOUBLE_EQ(lhs(0, 0), 5.0);
    EXPECT_DOUBLE_EQ(rhs(1, 1), 4.0);

    swap(lhs, rhs);
    EXPECT_DOUBLE_EQ(lhs(0, 0), 1.0);
}

TEST(ElementAccess, BoundsChecking) {
    linalg::Matrix matrix = {{1.0, 2.0}, {3.0, 4.0}};
    EXPECT_DOUBLE_EQ(matrix(1, 1), 4.0);
    EXPECT_THROW(matrix(2, 0), std::out_of_range);
}

TEST(Arithmetic, UnaryAndScalar) {
    linalg::Matrix matrix = {{1.0, -2.0}, {3.0, -4.0}};

    auto plus = +matrix;
    EXPECT_TRUE(plus == matrix);

    auto minus = -matrix;
    EXPECT_DOUBLE_EQ(minus(0, 0), -1.0);
    EXPECT_DOUBLE_EQ(minus(1, 1), 4.0);

    auto scaled = matrix * 2.0;
    EXPECT_DOUBLE_EQ(scaled(0, 1), -4.0);

    auto scaled2 = 2.0 * matrix;
    EXPECT_TRUE(scaled == scaled2);

    matrix *= 0.5;
    EXPECT_DOUBLE_EQ(matrix(0, 0), 0.5);
}

TEST(Arithmetic, ElementwiseOperations) {
    linalg::Matrix lhs = {{1.0, 2.0}, {3.0, 4.0}};
    linalg::Matrix rhs = {{5.0, 6.0}, {7.0, 8.0}};

    auto sum = lhs + rhs;
    EXPECT_DOUBLE_EQ(sum(0, 0), 6.0);
    EXPECT_DOUBLE_EQ(sum(1, 1), 12.0);

    lhs += rhs;
    EXPECT_TRUE(lhs == sum);

    const linalg::Matrix expected = {{1.0, 2.0}, {3.0, 4.0}};

    auto diff = sum - rhs;
    EXPECT_TRUE(diff == expected);

    sum -= rhs;
    EXPECT_TRUE(sum == expected);
}

TEST(Arithmetic, MatrixMultiplication) {
    linalg::Matrix lhs = {{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}};
    linalg::Matrix rhs = {{7.0, 8.0}, {9.0, 10.0}, {11.0, 12.0}};

    auto result = lhs * rhs;
    ASSERT_EQ(result.rows(), 2U);
    ASSERT_EQ(result.columns(), 2U);
    EXPECT_DOUBLE_EQ(result(0, 0), 58.0);
    EXPECT_DOUBLE_EQ(result(0, 1), 64.0);
    EXPECT_DOUBLE_EQ(result(1, 0), 139.0);
    EXPECT_DOUBLE_EQ(result(1, 1), 154.0);

    lhs *= rhs;
    EXPECT_TRUE(lhs == result);

    EXPECT_THROW((linalg::Matrix{{1.0, 2.0}} * linalg::Matrix{{1.0, 2.0}}), std::runtime_error);
}

TEST(Comparisons, EqualityWithTolerance) {
    linalg::Matrix base = {{1.0, 1.0}, {1.0, 1.0}};
    linalg::Matrix perturbed = {{1.0 + 1e-10, 1.0}, {1.0, 1.0 - 1e-10}};

    EXPECT_TRUE(base == perturbed);

    linalg::Matrix different = {{2.0, 1.0}, {1.0, 1.0}};
    EXPECT_TRUE(base != different);
}

TEST(Algorithms, NormTraceDeterminant) {
    linalg::Matrix matrix = {{1.0, 2.0}, {3.0, 4.0}};
    EXPECT_NEAR(matrix.norm(), std::sqrt(30.0), kEps);
    EXPECT_DOUBLE_EQ(matrix.trace(), 5.0);
    EXPECT_DOUBLE_EQ(matrix.det(), -2.0);

    linalg::Matrix non_square(2, 3);
    EXPECT_THROW((void)non_square.trace(), std::runtime_error);
    EXPECT_THROW((void)non_square.det(), std::runtime_error);
}

TEST(Algorithms, RankComputation) {
    linalg::Matrix full_rank = {{1.0, 2.0}, {3.0, 4.0}};
    EXPECT_EQ(full_rank.rank(), 2U);

    linalg::Matrix low_rank = {{1.0, 2.0}, {2.0, 4.0}};
    EXPECT_EQ(low_rank.rank(), 1U);
}

TEST(Algorithms, GaussElimination) {
    linalg::Matrix matrix = {{1.0, 2.0, 3.0}, {2.0, 5.0, 3.0}, {1.0, 0.0, 8.0}};
    auto forward = matrix;
    forward.gauss_forward();
    EXPECT_DOUBLE_EQ(forward(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(forward(1, 1), 1.0);

    auto backward = matrix;
    backward.gauss_forward().gauss_backward();
    EXPECT_NEAR(backward(0, 2), 0.0, 1e-9);
    EXPECT_NEAR(backward(1, 0), 0.0, 1e-9);
}

TEST(FreeFunctions, ConcatenateAndTranspose) {
    linalg::Matrix left = {{1.0, 2.0}, {3.0, 4.0}};
    linalg::Matrix right = {{5.0, 6.0}, {7.0, 8.0}};

    auto joined = concatenate(left, right);
    ASSERT_EQ(joined.rows(), 2U);
    ASSERT_EQ(joined.columns(), 4U);
    EXPECT_DOUBLE_EQ(joined(0, 2), 5.0);

    auto transposed = transpose(joined);
    ASSERT_EQ(transposed.rows(), 4U);
    ASSERT_EQ(transposed.columns(), 2U);
    EXPECT_DOUBLE_EQ(transposed(2, 0), 5.0);
}

TEST(FreeFunctions, InverseAndPower) {
    linalg::Matrix matrix = {{4.0, 7.0}, {2.0, 6.0}};
    auto inverse = invert(matrix);
    linalg::Matrix identity = matrix * inverse;

    EXPECT_NEAR(identity(0, 0), 1.0, kEps);
    EXPECT_NEAR(identity(1, 1), 1.0, kEps);
    EXPECT_NEAR(identity(0, 1), 0.0, kEps);
    EXPECT_NEAR(identity(1, 0), 0.0, kEps);

    auto squared = power(matrix, 2);
    EXPECT_TRUE(squared == matrix * matrix);

    EXPECT_THROW(power(linalg::Matrix(2, 3), 2), std::runtime_error);
    EXPECT_THROW(invert(linalg::Matrix{{1.0, 2.0}, {2.0, 4.0}}), std::runtime_error);
}

TEST(FreeFunctions, SolveLinearSystem) {
    linalg::Matrix a = {{3.0, 2.0}, {1.0, 2.0}};
    linalg::Matrix b = {2.0, 0.0};

    auto solution = solve(a, b);
    ASSERT_EQ(solution.rows(), 2U);
    ASSERT_EQ(solution.columns(), 1U);
    EXPECT_NEAR(solution(0, 0), 1.0, kEps);
    EXPECT_NEAR(solution(1, 0), -0.5, kEps);

    EXPECT_THROW(solve(linalg::Matrix(2, 3), b), std::runtime_error);
}
