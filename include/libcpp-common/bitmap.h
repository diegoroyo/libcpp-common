/*
 * bitmap.h
 * Diego Royo Meneses - Dec. 2023
 *
 * Image data structure with read/write operations
 */
#pragma once

#include <type_traits>

#include "libcpp-common/bitmap/png.h"
#include "libcpp-common/color.h"
#include "libcpp-common/detail/exception.h"

namespace common {

/// GRID2D / BITMAP ///

template <typename T>
class Grid2D : public std::vector<std::vector<T>> {
   private:
    using Base = std::vector<std::vector<T>>;

   public:
    using Base::Base;
    Grid2D(size_t width, size_t height, T value = 0) : Base(height) {
        this->resize(height);
        for (auto& row : *this) {
            row.resize(width, value);
        }
    }

    constexpr inline T& operator()(const size_t i, const size_t j) {
        return (*this).at(j).at(i);
    }
    constexpr inline const T& operator()(const size_t i, const size_t j) const {
        return (*this).at(j).at(i);
    }
    constexpr inline T& operator[](const size_t i) = delete;
};

using Bitmap1f = Grid2D<float>;
using Bitmap3f = Grid2D<Color3f>;
using Bitmap4f = Grid2D<Color4f>;

using Bitmap1u = Grid2D<unsigned int>;
using Bitmap3u = Grid2D<Color3u>;
using Bitmap4u = Grid2D<Color4u>;

/// LOAD / WRITE ///

class CommonBitmapException : public detail::CommonException {
   public:
    CommonBitmapException(const std::string& msg)
        : detail::CommonException(msg) {}
};

template <typename T>
Grid2D<T> bitmap_load(const char* filename);

};  // namespace common

#include "bitmap.tpp"