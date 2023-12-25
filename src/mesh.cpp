/*
 * mesh.cpp
 * Diego Royo Meneses - Dec. 2023
 *
 * 3D model loader
 */

#include "libcpp-common/mesh.h"

namespace common {

Mesh mesh_load(const char* filename) {
    std::ifstream file(filename);

    if (!file.is_open())
        throw detail::CommonMeshException("Could not open file " +
                                          std::string(filename));

    // Write here all the loaders
    if (test_ply(file)) return load_ply(file);

    throw detail::CommonMeshException("No mesh loader found for file " +
                                      std::string(filename));
}

};  // namespace common