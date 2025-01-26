#include "libcpp-common/tensor.h"

#include <iostream>

int main() {
    auto test = common::Tensor<float, 3, 3>::ones();
    auto test2 = common::Tensor<float, 3, 3>({{1, 2, 3}, {4, 5, 6}, {7, 8, 9}});
    std::cout << test.strides[0] << " " << test.strides[1] << std::endl;
    test(1, 2) = 5;
    std::cout << test(1, 2) << std::endl;
    std::cout << test << std::endl;
    std::cout << test + 3 << std::endl;
    return 0;
}