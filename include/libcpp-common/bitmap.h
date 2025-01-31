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
#include "libcpp-common/detail/exception.h"
#include "libcpp-common/geometry.h"

namespace common {

/// COLOR ///

template <typename T, unsigned int N>
class Color : public Vec<T, N> {
   private:
    using Base = Vec<T, N>;

    // delete xyzw accessors
    using Base::w;
    using Base::x;
    using Base::y;
    using Base::z;

   public:
    using Base::Base;
    Color(const Vec<T, N>&& v) : Base(v) {}

    constexpr T& r() {
        static_assert(N == 3 || N == 4, "Color does not have a R component");
        return (*this)[0];
    }
    constexpr const T& r() const {
        static_assert(N == 3 || N == 4, "Color does not have a R component");
        return (*this)[0];
    }
    constexpr T& g() {
        static_assert(N == 3 || N == 4, "Color does not have a G component");
        return (*this)[1];
    }
    constexpr const T& g() const {
        static_assert(N == 3 || N == 4, "Color does not have a G component");
        return (*this)[1];
    }
    constexpr T& b() {
        static_assert(N == 3 || N == 4, "Color does not have a B component");
        return (*this)[2];
    }
    constexpr const T& b() const {
        static_assert(N == 3 || N == 4, "Color does not have a B component");
        return (*this)[2];
    }
    constexpr T& a() {
        static_assert(N == 4, "Color does not have an A component");
        return (*this)[3];
    }
    constexpr const T& a() const {
        static_assert(N == 4, "Color does not have an A component");
        return (*this)[3];
    }

    template <unsigned int M = N, typename = std::enable_if_t<M == 3>>
    inline T luminance() const {
        return 0.2126 * r() + 0.7152 * g() + 0.0722 * b();
    }

    // convert color3 to color4
    template <unsigned int M = N, typename = std::enable_if_t<M == 4>>
    Color(const Color<T, 3>& c, T a = 0)
        : Color<T, 4>{c.r(), c.g(), c.b(), a} {};
    template <unsigned int M = N, typename = std::enable_if_t<M == 4>>
    Color(T r, T g, T b)
        : Color<T, 4>{r, g, b, std::is_floating_point_v<T> ? 1 : 255} {};

#define COMMON_color(name, r, g, b)                                            \
    template <unsigned int M = N,                                              \
              typename = std::enable_if_t<M == 3 || M == 4>>                   \
    static constexpr Color name() {                                            \
        return std::is_floating_point_v<T> ? Color(r, g, b)                    \
                                           : Color(r * 255, g * 255, b * 255); \
    }

    COMMON_color(Red, 1, 0, 0);
    COMMON_color(Green, 0, 1, 0);
    COMMON_color(Blue, 0, 0, 1);
    COMMON_color(Cyan, 0, 1, 1);
    COMMON_color(Magenta, 1, 0, 1);
    COMMON_color(Yellow, 1, 1, 0);
    COMMON_color(White, 1, 1, 1);
    COMMON_color(Black, 0, 0, 0);

#undef COMMON_color
};

using Color1f = Color<float, 1>;
using Color1u = Color<unsigned int, 1>;
using Color1b = Color<unsigned char, 1>;
using Color3f = Color<float, 3>;
using Color3u = Color<unsigned int, 3>;
using Color3b = Color<unsigned char, 3>;
using Color4f = Color<float, 4>;
using Color4u = Color<unsigned int, 4>;
using Color4b = Color<unsigned char, 4>;

/// GRID2D / BITMAP ///

template <typename T>
class Grid2D : protected std::vector<T> {
   private:
    using Base = std::vector<T>;
    size_t m_width, m_height;
    bool m_repeat, m_flip_y;

    constexpr inline T& operator[](const size_t i) {
        return Base::operator[](i);
    };

    // allows for numpy-like indexing (e.g. -1 is last pixel)
    inline unsigned int idx(int i, int j) const {
        if (!m_repeat && (i < 0 || j < 0 || i >= m_width || j >= m_height))
            throw detail::CommonBitmapException("Invalid index (" +
                                                std::to_string(i) + ", " +
                                                std::to_string(j) + ")");

        i = (i >= 0) ? (i % m_width)  //
                     : (m_width - 1 + ((i + 1) % (int)m_width));
        j = (j >= 0) ? (j % m_height)
                     : (m_height - 1 + ((j + 1) % (int)m_height));
        if (m_flip_y)
            return (m_height - 1 - j) * m_width + i;
        else
            return j * m_width + i;
    }

   public:
    using Base::Base;
    Grid2D(bool repeat = true, bool flip_y = false)
        : m_width(0), m_height(0), m_repeat(repeat), m_flip_y(flip_y), Base() {}
    Grid2D(size_t width, size_t height, const T& value = 0, bool repeat = true,
           bool flip_y = false)
        : m_width(width),
          m_height(height),
          m_repeat(repeat),
          m_flip_y(flip_y),
          Base() {
        this->resize(width, height, value);
    }
    void set_repeat(bool repeat) { m_repeat = repeat; }
    void set_flip_y(bool flip_y) { m_flip_y = flip_y; }

    void resize(size_t width, size_t height, const T& value = 0) {
        m_width = width;
        m_height = height;
        Base::resize(width * height);
    }

    void fill(const T& value) {
        std::fill(this->Base::begin(), this->Base::end(), value);
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
    inline const void* data() const { return &Base::operator[](0); }
    inline Vec2u size() const { return Vec2u(m_width, m_height); }

    constexpr inline T& operator()(int i, int j) {
        return (*this).at(idx(i, j));
    }
    constexpr inline const T& operator()(int i, int j) const {
        return (*this).at(idx(i, j));
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

using Bitmap1b = Grid2D<Color1b>;
using Bitmap3b = Grid2D<Color3b>;
using Bitmap4b = Grid2D<Color4b>;

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
        Grid2D<T> result(width(), height(), initial_value);
        for (size_t z = 0; z < depth(); ++z)
            for (size_t y = 0; y < height(); ++y)
                for (size_t x = 0; x < width(); ++x)
                    result(x, y) = reduce_f(result(x, y), (*this)(x, y, z));

        return result;
    }

    void map_in_place(void (*const map_f)(T&)) { return map_in_place(map_f); }
    template <typename MapFunction>
    void map_in_place(const MapFunction& map_f) {
        for (size_t z = 0; z < depth(); ++z)
            for (size_t y = 0; y < height(); ++y)
                for (size_t x = 0; x < width(); ++x) map_f((*this)(x, y, z));
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
Grid2D<T> load_bitmap(const std::string& filename, const bool flip_y = false);

template <typename T>
void save_bitmap(const std::string& filename, const Grid2D<T>& image);

};  // namespace common

#include "bitmap.tpp"