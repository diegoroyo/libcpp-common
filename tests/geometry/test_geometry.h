#pragma once

#include "libcpp-common/geometry.h"
#include "libcpp-common/test.h"

#define TEMPLATE_TEST_CASE(name, code, class) \
    TEST_CASE(name##_##class, using T = class; code)
#define TEST_CASE_VEC2(name, code)        \
    TEMPLATE_TEST_CASE(name, code, Vec2f) \
    TEMPLATE_TEST_CASE(name, code, Vec2i) \
    TEMPLATE_TEST_CASE(name, code, Vec2u)
#define TEST_CASE_VEC3(name, code)        \
    TEMPLATE_TEST_CASE(name, code, Vec3f) \
    TEMPLATE_TEST_CASE(name, code, Vec3i) \
    TEMPLATE_TEST_CASE(name, code, Vec3u)
#define TEST_CASE_VEC4(name, code)        \
    TEMPLATE_TEST_CASE(name, code, Vec4f) \
    TEMPLATE_TEST_CASE(name, code, Vec4i) \
    TEMPLATE_TEST_CASE(name, code, Vec4u)
#define TEST_CASE_VEC234(name, code) \
    TEST_CASE_VEC2(name, code)       \
    TEST_CASE_VEC3(name, code)       \
    TEST_CASE_VEC4(name, code)

TEST_CASE(00_type_size, {
    TEST_TRUE(std::is_same_v<Vec2f::type, float>);
    TEST_TRUE(std::is_same_v<Vec2i::type, int>);
    TEST_TRUE(std::is_same_v<Vec2u::type, unsigned int>);
    TEST_TRUE(Vec2f::size == 2);
    TEST_TRUE(Vec3f::size == 3);
    TEST_TRUE(Vec4f::size == 4);
})

COMPILE_TIME_TEST(Vec2f)

TEST_CASE(01_constructors, {
    TEST_TRUE(Vec2f_compiles_from_type<Vec2f, float, float>);
    TEST_TRUE(Vec2f_compiles_from_type<Vec2f, int, float>);
    TEST_TRUE(!Vec2f_compiles_from_type<Vec2f, std::string, float, float>);
    TEST_TRUE(!Vec2f_compiles_from_type<Vec2f, std::string, float>);
})

TEST_CASE_VEC234(02_constructor_zero_or_one_value, {
    T zero;
    for (const auto& x : zero) {
        TEST_TRUE(zero == 0);
    }
    T ten(10);
    for (const auto& x : ten) {
        TEST_TRUE(ten == 10);
    }
})

TEST_CASE(03_constructor_n_values, {
    Vec2i v2(1, 2);
    Vec3i v3(1, 2, 3);
    Vec4i v4(1, 2, 3, 4);
    Vec<int, 10> v10(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
    for (int i = 1; i <= 2; ++i) TEST_TRUE(v2[i] = i);
    for (int i = 1; i <= 3; ++i) TEST_TRUE(v3[i] = i);
    for (int i = 1; i <= 4; ++i) TEST_TRUE(v4[i] = i);
    for (int i = 1; i <= 10; ++i) TEST_TRUE(v10[i] = i);
})

TEST_CASE(04_conversion_vec3_vec4, {
    Vec3f v3(1, 2, 3);
    Vec4f v4_zero(v3);
    Vec4f v4_custom(v3, 4);
    for (int i = 0; i < 3; ++i) {
        TEST_TRUE(v3[i] == v4_zero[i]);
        TEST_TRUE(v3[i] == v4_custom[i]);
        TEST_TRUE(v4_custom.xyz()[i] == v3[i]);
    }
    TEST_TRUE(v4_zero[3] == 0);
    TEST_TRUE(v4_custom[3] == 4);
})