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

// test conditions
#define _TEST_FAIL                                         \
    {                                                      \
        _current_test_ok = false;                          \
        std::cout << "failed on line " << __LINE__ << " "; \
    }
#define TEST_TRUE(...) \
    if (!(__VA_ARGS__)) _TEST_FAIL

// test functions
using TestFunction = bool (*)();
struct TestFunctionData {
    std::string name;
    TestFunction f;
};
std::vector<std::string> _test_order;
std::unordered_map<std::string, std::vector<TestFunctionData>> _test_functions;
#define COMMON_BASENAME(file) (file.substr(file.find_last_of('/') + 1))
void test_register(std::string file, const std::string &function_name,
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
        bool _current_test_ok = true;                                 \
        __VA_ARGS__;                                                  \
        return _current_test_ok;                                      \
    }                                                                 \
    static struct _register_test##name {                              \
        _register_test##name() {                                      \
            common::test::test_register(__FILE__, #name, test##name); \
        }                                                             \
    } _register_test##name##_obj;

void run_tests() {
    for (const auto &file : _test_order) {
        std::cout << file << std::endl;
        const auto &functions = _test_functions[file];
        int num_ok = 0;
        for (const auto &test : functions) {
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
