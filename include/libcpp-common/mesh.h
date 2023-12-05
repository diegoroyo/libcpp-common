/*
 * mesh.h
 * Diego Royo Meneses - Dec. 2023
 *
 * 3D model loader
 */

#pragma once

#include <fstream>
#include <string>

#include "libcpp-common/detail/exception.h"
#include "libcpp-common/geometry.h"
#include "libcpp-common/mesh/ply.h"

namespace common {

struct Mesh {
    VecList4f vertices;
    VecList3u faces;
};

class CommonMeshException : public detail::CommonException {
   public:
    CommonMeshException(const std::string& msg)
        : detail::CommonException(msg) {}
};

Mesh mesh_load(const char* filename);

};  // namespace common