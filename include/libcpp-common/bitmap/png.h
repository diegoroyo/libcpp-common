/*
 * png.h
 * Diego Royo Meneses - Dec. 2023
 *
 * Portable Network Graphics loader
 */
#pragma once

#include <cstdint>
#include <fstream>

#include "libcpp-common/bitmap.h"

namespace common {

template <typename T>
bool test_png(std::ifstream& file);

template <typename T>
Grid2D<T> load_png(std::ifstream& file, const uint8_t channels);

};  // namespace common

#include "bitmap/png.tpp"