/*
 * npy.h
 * Diego Royo Meneses - Jan. 2024
 *
 * Image saver with a format compatible with numpy's np.load(...)
 */
#pragma once

#include <fstream>

#include "libcpp-common/bitmap.h"

namespace common {

template <typename T>
void save_npy(std::ofstream& file, const Grid2D<T>& image);

};  // namespace common

#include "bitmap/npy.tpp"