/*
 * png.h
 * Diego Royo Meneses - Dec. 2023
 *
 * Portable Network Graphics loader
 */
#pragma once

#include <fstream>

namespace common {

template <typename T>
class Grid2D;

template <typename T>
bool test_png(std::ifstream& file);

template <typename T>
Grid2D<T> load_png(std::ifstream& file);

};  // namespace common

#include "bitmap/png.tpp"