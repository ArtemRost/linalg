#ifndef MINIMAL_GTEST_GTEST_H
#define MINIMAL_GTEST_GTEST_H

#include <cmath>
#include <exception>
#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace testing {

struct TestInfo {
    std::string suite;
    std::string name;
    std::function<void()> body;
};

inline std::vector<TestInfo>& registry() {
    static std::vector<TestInfo> tests;
    return tests;
}

class Test {
public:
    virtual ~Test() = default;
    virtual void SetUp() {}
    virtual void TearDown() {}
};

inline int& failed_count() {
    static int failed = 0;
    return failed;
}

inline void RegisterTest(const std::string& suite, const std::string& name, std::function<void()> body) {
    registry().push_back({suite, name, std::move(body)});
}

inline void InitGoogleTest(int*, char**) {}

inline int RUN_ALL_TESTS() {
    for (const auto& test : registry()) {
        try {
            test.body();
        } catch (const std::exception& ex) {
            std::cerr << "[  FAILED  ] " << test.suite << '.' << test.name
                      << ": выброшено исключение: " << ex.what() << '\n';
            ++failed_count();
        } catch (...) {
            std::cerr << "[  FAILED  ] " << test.suite << '.' << test.name
                      << ": выброшено неизвестное исключение" << '\n';
            ++failed_count();
        }
    }

    if (failed_count() == 0) {
        std::cout << "[  PASSED  ] " << registry().size() << " тестов\n";
    } else {
        std::cout << "[  FAILED  ] " << failed_count() << " тестов из "
                  << registry().size() << '\n';
    }
    return failed_count();
}

class AssertionException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

inline void ReportFailure(const std::string& suite, const std::string& name,
                          const std::string& expr, const std::string& details,
                          bool fatal) {
    std::ostringstream os;
    os << "[  FAILED  ] " << suite << '.' << name << ": " << expr;
    if (!details.empty()) {
        os << " (" << details << ')';
    }
    std::cerr << os.str() << '\n';
    ++failed_count();
    if (fatal) {
        throw AssertionException(os.str());
    }
}

} // namespace testing

#define TEST(suite, name)                                                                    \
    void suite##_##name##_TestBody();                                                        \
    namespace {                                                                              \
    struct suite##_##name##_Registrator {                                                    \
        suite##_##name##_Registrator() {                                                     \
            ::testing::RegisterTest(#suite, #name, suite##_##name##_TestBody);               \
        }                                                                                    \
    };                                                                                       \
    static suite##_##name##_Registrator suite##_##name##_instance;                           \
    }                                                                                        \
    void suite##_##name##_TestBody()

#define EXPECT_TRUE(cond)                                                                    \
    do {                                                                                     \
        if (!(cond)) {                                                                       \
            ::testing::ReportFailure(__FUNCTION__, "EXPECT_TRUE", #cond, "", false);        \
        }                                                                                    \
    } while (false)

#define EXPECT_FALSE(cond) EXPECT_TRUE(!(cond))

#define ASSERT_TRUE(cond)                                                                    \
    do {                                                                                     \
        if (!(cond)) {                                                                       \
            ::testing::ReportFailure(__FUNCTION__, "ASSERT_TRUE", #cond, "", true);         \
        }                                                                                    \
    } while (false)

#define ASSERT_FALSE(cond) ASSERT_TRUE(!(cond))

#define EXPECT_EQ(lhs, rhs)                                                                  \
    do {                                                                                     \
        auto lhs_val = (lhs);                                                                \
        auto rhs_val = (rhs);                                                                \
        if (!(lhs_val == rhs_val)) {                                                         \
            std::ostringstream oss;                                                          \
            oss << lhs_val << " != " << rhs_val;                                           \
            ::testing::ReportFailure(__FUNCTION__, "EXPECT_EQ", #lhs " == " #rhs,          \
                                      oss.str(), false);                                     \
        }                                                                                    \
    } while (false)

#define ASSERT_EQ(lhs, rhs)                                                                  \
    do {                                                                                     \
        auto lhs_val = (lhs);                                                                \
        auto rhs_val = (rhs);                                                                \
        if (!(lhs_val == rhs_val)) {                                                         \
            std::ostringstream oss;                                                          \
            oss << lhs_val << " != " << rhs_val;                                           \
            ::testing::ReportFailure(__FUNCTION__, "ASSERT_EQ", #lhs " == " #rhs,         \
                                      oss.str(), true);                                      \
        }                                                                                    \
    } while (false)

#define EXPECT_NEAR(lhs, rhs, eps)                                                           \
    do {                                                                                     \
        auto lhs_val = static_cast<double>(lhs);                                             \
        auto rhs_val = static_cast<double>(rhs);                                             \
        auto eps_val = static_cast<double>(eps);                                             \
        if (std::fabs(lhs_val - rhs_val) > eps_val) {                                       \
            std::ostringstream oss;                                                          \
            oss << lhs_val << " != " << rhs_val << " ± " << eps_val;                      \
            ::testing::ReportFailure(__FUNCTION__, "EXPECT_NEAR", #lhs " ~= " #rhs,       \
                                      oss.str(), false);                                     \
        }                                                                                    \
    } while (false)

#define EXPECT_DOUBLE_EQ(lhs, rhs) EXPECT_NEAR(lhs, rhs, 1e-9)

#define EXPECT_THROW(statement, exception_type)                                              \
    do {                                                                                     \
        bool thrown = false;                                                                 \
        try {                                                                                \
            statement;                                                                       \
        } catch (const exception_type&) {                                                    \
            thrown = true;                                                                   \
        } catch (...) {                                                                      \
            ::testing::ReportFailure(__FUNCTION__, "EXPECT_THROW", #statement,              \
                                      "выброшено исключение другого типа", false);         \
            thrown = true;                                                                   \
        }                                                                                    \
        if (!thrown) {                                                                       \
            ::testing::ReportFailure(__FUNCTION__, "EXPECT_THROW", #statement,              \
                                      "исключение не выброшено", false);                   \
        }                                                                                    \
    } while (false)

#define EXPECT_NO_THROW(statement)                                                           \
    do {                                                                                     \
        try {                                                                                \
            statement;                                                                       \
        } catch (const std::exception& ex) {                                                 \
            ::testing::ReportFailure(__FUNCTION__, "EXPECT_NO_THROW", #statement,           \
                                      ex.what(), false);                                     \
        } catch (...) {                                                                      \
            ::testing::ReportFailure(__FUNCTION__, "EXPECT_NO_THROW", #statement,           \
                                      "неизвестное исключение", false);                    \
        }                                                                                    \
    } while (false)

#endif // MINIMAL_GTEST_GTEST_H
