/*
 * test.h
 * Diego Royo Meneses - Jan. 2024
 *
 * Testing utilities
 */
#pragma once

#include <cstring>
#include <iostream>
#include <unordered_map>
#include <vector>

namespace common {
namespace test {

/// Extracted from
/// https://stackoverflow.com/questions/7282350/how-to-unit-test-deliberate-compilation-errors-of-template-code
/// Main idea: outside a test write
/// COMPILE_TIME_TEST(func) with the name of func you want to test
/// Then, if that function accepts parameters use func_compiles_from_type
/// otherwise use func_compiles
#define COMPILE_TIME_TEST(func) COMPILE_TIME_TEST_FUNCTION(func, func)
#define COMPILE_TIME_TEST_FUNCTION(name, func)                                 \
    namespace detail {                                                         \
    template <typename R, auto... args>                                        \
    struct name##FromArgs : std::false_type {};                                \
    template <auto... args>                                                    \
    struct name##FromArgs<decltype(func(args...)), args...> : std::true_type { \
    };                                                                         \
    template <typename R, typename... Args>                                    \
    struct name##FromType : std::false_type {};                                \
    template <typename... Args>                                                \
    struct name##FromType<decltype(func(std::declval<Args>()...)), Args...>    \
        : std::true_type {};                                                   \
    };                                                                         \
    template <typename R, auto... Args>                                        \
    static constexpr auto name##_compiles =                                    \
        detail::name##FromArgs<R, Args...>::value;                             \
    template <typename... Args>                                                \
    static constexpr auto name##_compiles_from_type =                          \
        detail::name##FromType<Args...>::value;

// test conditions
#define TEST_TRUE(...)                                                     \
    if (!(__VA_ARGS__)) {                                                  \
        _current_test_ok = false;                                          \
        std::cout << "failed on line " << __LINE__ << " (false != true) "; \
    }
#define TEST_EQ(A, B)                                             \
    if ((A) != (B)) {                                             \
        _current_test_ok = false;                                 \
        std::cout << "failed on line " << __LINE__ << " (" << (A) \
                  << " != " << (B) << ") ";                       \
    }

// test functions
using TestFunction = bool (*)();
struct TestFunctionData {
    std::string name;
    TestFunction f;
};
std::vector<std::string> _test_order;
std::unordered_map<std::string, std::vector<TestFunctionData>> _test_functions;
#define COMMON_BASENAME(file) (file.substr(file.find_last_of('/') + 1))
void test_register(std::string file, const std::string& function_name,
                   TestFunction f) {
    file = COMMON_BASENAME(file);
    if (_test_functions.find(file) == std::end(_test_functions)) {
        _test_order.push_back(file);
        _test_functions[file] = std::vector<TestFunctionData>();
    }
    _test_functions[file].push_back({function_name, f});
}
#undef COMMON_BASENAME

#define TEST_CASE(name, ...)                                          \
    bool test##name() {                                               \
        bool _current_test_ok = true; /* FIXME */                     \
        __VA_ARGS__;                                                  \
        return _current_test_ok;                                      \
    }                                                                 \
    static struct _register_test##name {                              \
        _register_test##name() {                                      \
            common::test::test_register(__FILE__, #name, test##name); \
        }                                                             \
    } _register_test##name##_obj;

void run_tests() {
    for (const auto& file : _test_order) {
        std::cout << file << std::endl;
        const auto& functions = _test_functions[file];
        int num_ok = 0;
        for (const auto& test : functions) {
            std::cout << "    " << test.name << " ... ";
            bool test_ok = test.f();
            if (test_ok) {
                ++num_ok;
                std::cout << "\r 🟢 " << test.name << "    " << std::endl;
            } else {
                std::cout << "\r 🔴 " << std::endl;
            }
        }
        std::cout << "Total: " << num_ok << "/" << functions.size();
        if (num_ok == functions.size()) {
            std::cout << " 🟢" << std::endl;
        } else {
            std::cout << " 🔴" << std::endl;
        }
    }
}

};  // namespace test
};  // namespace common
