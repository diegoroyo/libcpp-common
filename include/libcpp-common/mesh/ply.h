/*
 * ply.h
 * Diego Royo Meneses - Dec. 2023
 *
 * Polygon File Format loader
 */
#pragma once

#include <fstream>

namespace common {

struct Mesh;
bool test_ply(std::ifstream& file);
Mesh load_ply(std::ifstream& file);

};  // namespace common