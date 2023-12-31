#include "libcpp-common/mesh.h"

#include <iostream>

int main() {
    common::Mesh mesh =
        common::load_mesh("/home/diego/desarrollo/cpp/quad.ply");

    std::cout << mesh.vertices << std::endl;
    std::cout << mesh.faces << std::endl;
    std::cout << mesh.vertices.size() << std::endl;
    std::cout << mesh.faces.size() << std::endl;
    return 0;
}