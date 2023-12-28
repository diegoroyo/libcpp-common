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
class Grid2D : protected std::vector<std::vector<T>> {
   private:
    using Base = std::vector<std::vector<T>>;
    size_t m_width, m_height;
    bool m_repeat;

    // allows for numpy-like indexing (e.g. -1 is last pixel)
    inline void idx(int& i, int& j) {
        if (!m_repeat && (i < 0 || j < 0 || i >= m_width || j >= m_height))
            throw detail::CommonBitmapException("Invalid index (" +
                                                std::to_string(i) + ", " +
                                                std::to_string(j));

        i = (i >= 0) ? (i % m_width)  //
                     : (m_width - 1 + ((i + 1) % (int)m_width));
        j = (j >= 0) ? (j % m_height)
                     : (m_height - 1 + ((j + 1) % (int)m_height));
    }

   public:
    using Base::Base;
    Grid2D(bool repeat = true)
        : m_width(0), m_height(0), m_repeat(repeat), Base() {}
    Grid2D(size_t width, size_t height, const T& value = 0, bool repeat = true)
        : m_width(width), m_height(height), m_repeat(repeat), Base() {
        this->resize(width, height, value);
    }

    void resize(size_t width, size_t height, const T& value = 0) {
        m_width = width;
        m_height = height;
        Base::resize(height);
        for (auto& row : *this) {
            row.resize(width, value);
        }
    }

    void fill(const T& value) {
        for (auto& row : *this) std::fill(row.begin(), row.end(), value);
    }

    inline size_t width() const { return m_width; }
    inline size_t height() const { return m_height; }

    constexpr inline T& operator()(int i, int j) {
        idx(i, j);
        return (*this).at(j).at(i);
    }
    constexpr inline const T& operator()(int i, int j) const {
        idx(i, j);
        return (*this).at(j).at(i);
    }
    constexpr inline T& operator()(const Vec2i& ij) {
        int i = ij.x(), j = ij.y();
        idx(i, j);
        return (*this).at(j).at(i);
    }
    constexpr inline const T& operator()(const Vec2i& ij) const {
        int i = ij.x(), j = ij.y();
        idx(i, j);
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

template <typename T>
Grid2D<T> bitmap_load(const char* filename);

};  // namespace common

#include "bitmap.tpp"