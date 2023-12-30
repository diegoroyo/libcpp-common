/*
 * ppm.h
 * Diego Royo Meneses - Dec. 2023
 *
 * Portable PixMap Format loader and saver
 */
#pragma once

#include <cstdint>
#include <fstream>

#include "libcpp-common/bitmap.h"
#include "libcpp-common/color.h"

namespace common {

// template <typename T>
// bool test_ppm(std::ifstream& file);

// template <typename T>
// Grid2D<T> load_ppm(std::ifstream& file, const uint8_t channels);

template <typename T>
void save_ppm(std::ofstream& file, const Grid2D<T>& image);

};  // namespace common

#include "bitmap/ppm.tpp"