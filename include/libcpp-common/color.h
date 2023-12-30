/*
 * color.h
 * Diego Royo Meneses - Dec. 2023
 *
 * Implementation of Color class
 */
#pragma once

#include "libcpp-common/geometry.h"

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

    // convert color3 to color4
    template <unsigned int M = N, typename = std::enable_if_t<M == 4>>
    Color(const Color<T, 3>& c, T a = 0)
        : Color<T, 4>{c.r(), c.g(), c.b(), a} {};
    template <unsigned int M = N, typename = std::enable_if_t<M == 4>>
    Color(T r, T g, T b)
        : Color<T, 4>{r, g, b, std::is_floating_point_v<T> ? 1 : 255} {};

#define COMMON_color(name, r, g, b)                                            \
    static constexpr std::enable_if_t<N == 3 || N == 4, Color> name() {        \
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

using Color3f = Color<float, 3>;
using Color3u = Color<unsigned int, 3>;
using Color4f = Color<float, 4>;
using Color4u = Color<unsigned int, 4>;