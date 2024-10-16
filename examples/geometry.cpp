#include "libcpp-common/geometry.h"

#include <iostream>

using namespace common;

int main() {
    common::VecList4f list(3);
    list[0].x() = 6;
    list[0].y() = 4;
    list[0].z() = 2;
    list[0].w() = 2;
    list[1] = Vec4f(10, 8, 6, 2);
    list[2] = Vec4f(15, 10, 5, 0) + Vec4f(0, 0, 0, 5);

    std::cout << list << std::endl;

    VecList3f list_flatten = list.divide_by_homogeneous();

    std::cout << list_flatten << std::endl;

    Mat3f test(Vec3f(0), Vec3f(1), Vec3f(2));
    std::cout << test << std::endl;
    Mat3f test2(1, 2, 3, 4, 5, 6, 7, 8, 9);
    std::cout << test2 << std::endl;
}