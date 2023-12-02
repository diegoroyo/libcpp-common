#include "libcpp-common/geometry.h"

#include <iostream>

int main() {
    VecList4f list(3);
    list[0].x() = 6;
    list[0].y() = 4;
    list[0].z() = 2;
    list[0].w() = 2;
    list[1] = Vec4f(10, 8, 6, 2);
    list[2] = Vec4f(15, 10, 5, 0) + Vec4f(0, 0, 0, 5);

    std::cout << list << std::endl;

    VecList3f list_flatten = list.divide_by_homogeneous();

    std::cout << list_flatten << std::endl;
}