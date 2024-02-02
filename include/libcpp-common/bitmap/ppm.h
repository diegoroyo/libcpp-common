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

namespace common {

template <typename T>
bool test_ppm(std::ifstream& file);

template <typename T>
Grid2D<T> load_ppm(std::ifstream& file);

template <typename T>
std::enable_if_t<bitmap_channels<T>::value == 3 &&
                 !std::is_same_v<typename T::type, std::complex<float>>>
save_ppm(std::ofstream& file, const Grid2D<T>& image);

};  // namespace common

#include "bitmap/ppm.tpp"