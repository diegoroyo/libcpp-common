/*
 * bitmap.h
 * Diego Royo Meneses - Dec. 2023
 *
 * Image data structure with read/write operations
 */
#pragma once

#include <functional>
#include <type_traits>

namespace common {
template <typename T>
class Grid2D;
};

#include "libcpp-common/bitmap/png.h"
#include "libcpp-common/bitmap/ppm.h"
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

    constexpr inline std::vector<T>& operator[](const size_t i) {
        return Base::operator[](i);
    };

    // allows for numpy-like indexing (e.g. -1 is last pixel)
    inline void idx(int& i, int& j) const {
        if (!m_repeat && (i < 0 || j < 0 || i >= m_width || j >= m_height))
            throw detail::CommonBitmapException("Invalid index (" +
                                                std::to_string(i) + ", " +
                                                std::to_string(j) + ")");

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
    void set_repeat(bool repeat) { m_repeat = repeat; }

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

    // This alternative does not use templated functions so that
    // reduce_f's signature is visible to the user
    template <typename Result>
    Result reduce(Result initial_value,
                  Result (*const reduce_f)(Result, const T&)) const {
        return reduce(initial_value, reduce_f);
    }
    template <typename Result, typename ReduceFunc>
    Result reduce(Result initial_value, const ReduceFunc& reduce_f) const {
        for (auto& row : *this)
            for (auto& element : row)
                initial_value = reduce_f(initial_value, element);

        return initial_value;
    }

    // This alternative does not use templated functions so that
    // map_f's signature is visible to the user
    template <typename Result>
    Grid2D<Result> map(Result (*const map_f)(const T&)) const {
        return map(map_f);
    }
    template <typename Result, typename MapFunction>
    Grid2D<Result> map(const MapFunction& map_f) const {
        Grid2D<Result> result(width(), height());
        for (size_t y = 0; y < height(); ++y)
            for (size_t x = 0; x < width(); ++x)
                result(x, y) = map_f((*this)(x, y));

        return result;
    }

    void map_in_place(void (*const map_f)(T&)) { return map_in_place(map_f); }
    template <typename MapFunction>
    void map_in_place(const MapFunction& map_f) {
        for (size_t y = 0; y < height(); ++y)
            for (size_t x = 0; x < width(); ++x) map_f((*this)(x, y));
    }

    inline size_t width() const { return m_width; }
    inline size_t height() const { return m_height; }
    inline Vec2u size() const { return Vec2u(m_width, m_height); }

    constexpr inline T& operator()(int i, int j) {
        idx(i, j);
        return (*this).at(j).at(i);
    }
    constexpr inline const T& operator()(int i, int j) const {
        idx(i, j);
        return (*this).at(j).at(i);
    }
    constexpr inline T& operator()(const Vec2i& ij) {
        return (*this)(ij.x(), ij.y());
    }
    constexpr inline const T& operator()(const Vec2i& ij) const {
        return (*this)(ij.x(), ij.y());
    }

    T interpolate_linear(float i, float j) const {
        i -= 0.5f;
        j -= 0.5f;
        int xa = std::floor(i), xb = std::ceil(i);
        int ya = std::floor(j), yb = std::ceil(j);

        float xi = i - xa, yi = j - ya;
        return (*this)(xa, ya) * (1 - xi) * (1 - yi) +  //
               (*this)(xb, ya) * xi * (1 - yi) +        //
               (*this)(xa, yb) * (1 - xi) * yi +        //
               (*this)(xb, yb) * xi * yi;
    }
    T interpolate_linear(const Vec2f& ij) const {
        return this->interpolate_linear(ij.x(), ij.y());
    }
};

using Bitmap1f = Grid2D<float>;
using Bitmap3f = Grid2D<Color3f>;
using Bitmap4f = Grid2D<Color4f>;

using Bitmap1u = Grid2D<unsigned int>;
using Bitmap3u = Grid2D<Color3u>;
using Bitmap4u = Grid2D<Color4u>;

template <typename T>
struct bitmap_channels : std::integral_constant<uint8_t, T::size> {};
template <>
struct bitmap_channels<float> : std::integral_constant<uint8_t, 1> {};
template <>
struct bitmap_channels<unsigned int> : std::integral_constant<uint8_t, 1> {};

/// LOAD / SAVE ///

template <typename T>
Grid2D<T> bitmap_load(const std::string& filename);

template <typename T>
void bitmap_save(const std::string& filename, const Grid2D<T>& image);

};  // namespace common

#include "bitmap.tpp"