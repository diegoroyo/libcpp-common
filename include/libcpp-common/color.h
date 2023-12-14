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
   public:
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
};

template <typename T>
class Color3 : public Color<T, 3> {
   public:
    constexpr Color3(T l = 0) : Color<T, 3>{l, l, l} {}
    constexpr Color3(T r, T g, T b) : Color<T, 3>{r, g, b} {}
    constexpr Color3(const Color<T, 3>&& c) : Color<T, 3>(c) {}

    static const Color3  //
        Black,           //
        White,           //
        Red,             //
        Green,           //
        Blue,            //
        Yellow,          //
        Magenta,         //
        Cyan;
};

template <typename T>
class Color4 : public Color<T, 4> {
   public:
    constexpr Color4(T l = 0) : Color<T, 4>{l, l, l, l} {}
    constexpr Color4(Color3<T> c, T a = 1)
        : Color<T, 4>{c.r(), c.g(), c.b(), a} {}
    constexpr Color4(T r, T g, T b, T a = 1) : Color<T, 4>{r, g, b, a} {}
    constexpr Color4(const Color<T, 4>&& v) : Color<T, 4>(v) {}

    static const Color4  //
        Black,           //
        White,           //
        Red,             //
        Green,           //
        Blue,            //
        Yellow,          //
        Magenta,         //
        Cyan;
};

using Color3f = Color3<float>;
using Color3u = Color3<unsigned int>;
using Color4f = Color4<float>;
using Color4u = Color4<unsigned int>;