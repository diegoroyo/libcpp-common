#include "libcpp-common/mesh.h"

#include <iostream>

int main() {
    common::Mesh mesh = common::mesh_load(
        "/media/pleiades/vault/apps/nova-engine/ext/libcpp-common/example/"
        "quad.ply");

    std::cout << mesh.vertices << std::endl;
    std::cout << mesh.faces << std::endl;
    std::cout << mesh.vertices.size() << std::endl;
    std::cout << mesh.faces.size() << std::endl;
    return 0;
}