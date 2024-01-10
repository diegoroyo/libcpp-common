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

#include "libcpp-common/bitmap/npy.h"
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

using Bitmap1f = Grid2D<Color1f>;
using Bitmap3f = Grid2D<Color3f>;
using Bitmap4f = Grid2D<Color4f>;

using Bitmap1u = Grid2D<Color1u>;
using Bitmap3u = Grid2D<Color3u>;
using Bitmap4u = Grid2D<Color4u>;

/// GRID3D / BITMAPLIST ///

// just handles temporal (new vector) dimension, Grid2D handles the other two
template <typename T>
class Grid3D : public std::vector<Grid2D<T>> {
   private:
    using Base = std::vector<Grid2D<T>>;
    size_t m_width, m_height, m_depth;
    bool m_repeat;

    constexpr inline std::vector<T>& operator[](const size_t i) {
        return Base::operator[](i);
    };

    // allows for numpy-like indexing (e.g. -1 is last pixel)
    inline void idx(int& t) const {
        if (!m_repeat && (t < 0 || t >= m_depth))
            throw detail::CommonBitmapException(
                "Invalid index " + std::to_string(t) + " in depth dimension.");

        t = (t >= 0) ? (t % m_depth)  //
                     : (m_depth - 1 + ((t + 1) % (int)m_depth));
    }

   public:
    using Base::Base;
    Grid3D(bool repeat = true)
        : m_depth(0), m_width(0), m_height(0), m_repeat(repeat), Base() {}
    Grid3D(size_t width, size_t height, size_t depth, const T& value = 0,
           bool repeat = true)
        : m_width(width), m_height(height), m_depth(depth), Base() {
        this->resize(width, height, depth, value);
        set_repeat(repeat);
    }
    void set_repeat(bool repeat) {
        m_repeat = repeat;
        for (auto& frame : *this) frame.set_repeat(repeat);
    }

    void resize(size_t width, size_t height, size_t depth, const T& value = 0) {
        m_width = width;
        m_height = height;
        m_depth = depth;
        Base::resize(depth);
        for (auto& frame : *this) {
            frame.resize(width, height, value);
        }
    }

    void fill(const T& value) {
        for (auto& frame : *this) frame.fill(value);
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
        for (auto& frame : *this)
            initial_value =
                frame.template reduce<Result>(initial_value, reduce_f);

        return initial_value;
    }

    Grid2D<T> reduce_depth(T initial_value,
                           T (*const reduce_f)(T, const T&)) const {
        return reduce_depth(initial_value, reduce_f);
    }
    template <typename ReduceFunc>
    Grid2D<T> reduce_depth(T initial_value, const ReduceFunc& reduce_f) const {
        Grid2D<T> result(m_width, m_height, initial_value);
        for (size_t z = 0; z < m_depth; ++z)
            for (size_t y = 0; y < m_height; ++y)
                for (size_t x = 0; x < m_width; ++x)
                    result(x, y) = reduce_f(result(x, y), (*this)(x, y, z));

        return result;
    }

    inline size_t width() const { return m_width; }
    inline size_t height() const { return m_height; }
    inline size_t depth() const { return m_depth; }
    inline Vec3u size() const { return Vec3u(m_width, m_height, m_depth); }

    constexpr inline T& operator()(int i, int j, int t) {
        idx(t);
        return (*this).at(t)(i, j);
    }
    constexpr inline const T& operator()(int i, int j, int t) const {
        idx(t);
        return (*this).at(t)(i, j);
    }
    constexpr inline T& operator()(const Vec3i& ijt) {
        return (*this)(ijt.x(), ijt.y(), ijt.z());
    }
    constexpr inline const T& operator()(const Vec3i& ijt) const {
        return (*this)(ijt.x(), ijt.y(), ijt.z());
    }
};

using BitmapList1f = Grid3D<Color1f>;
using BitmapList3f = Grid3D<Color3f>;
using BitmapList4f = Grid3D<Color4f>;

using BitmapList1u = Grid3D<Color1u>;
using BitmapList3u = Grid3D<Color3u>;
using BitmapList4u = Grid3D<Color4u>;

template <typename T>
struct bitmap_channels : std::integral_constant<uint8_t, T::size> {};
template <>
struct bitmap_channels<float> : std::integral_constant<uint8_t, 1> {};
template <>
struct bitmap_channels<unsigned int> : std::integral_constant<uint8_t, 1> {};

/// LOAD / SAVE ///

template <typename T>
Grid2D<T> load_bitmap(const std::string& filename);

template <typename T>
void save_bitmap(const std::string& filename, const Grid2D<T>& image);

};  // namespace common

#include "bitmap.tpp"