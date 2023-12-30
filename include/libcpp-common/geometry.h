/*
 * geometry.h
 * Diego Royo Meneses - Dec. 2023
 *
 * Defintions of Vec, VecList and Mat classes
 */

#pragma once

#include <math.h>

#include <array>
#include <ostream>
#include <type_traits>
#include <vector>

/// VECTOR ///

template <typename T, unsigned int N>
class Vec : public std::array<T, N> {
   private:
    using Base = std::array<T, N>;

   public:
    using type = T;
    static constexpr unsigned int size = N;

    Vec(T x = 0) : std::array<T, N>() { Base::fill(x); }
    template <typename... Args,
              typename = std::enable_if_t<sizeof...(Args) == N>>
    Vec(Args&&... args)
        : std::array<T, N>{static_cast<T>(std::forward<Args>(args))...} {}

    constexpr T& x() { return (*this)[0]; }
    constexpr const T& x() const { return (*this)[0]; }
    constexpr T& y() {
        static_assert(N >= 2, "Vec does not have a Y component");
        return (*this)[1];
    }
    constexpr const T& y() const {
        static_assert(N >= 2, "Vec does not have a Y component");
        return (*this)[1];
    }
    constexpr T& z() {
        static_assert(N >= 3, "Vec does not have a Z component");
        return (*this)[2];
    }
    constexpr const T& z() const {
        static_assert(N >= 3, "Vec does not have a Z component");
        return (*this)[2];
    }
    constexpr T& w() {
        static_assert(N >= 4, "Vec does not have a W component");
        return (*this)[3];
    }
    constexpr const T& w() const {
        static_assert(N >= 4, "Vec does not have a W component");
        return (*this)[3];
    }

    constexpr inline bool operator==(const Vec<T, N>& o) const noexcept {
        for (unsigned int i = 0; i < N; ++i)
            if ((*this)[i] != o[i]) return false;
        return true;
    }
    constexpr inline Vec<T, N> operator+(const T v) const noexcept {
        Vec<T, N> result;
        for (unsigned int i = 0; i < N; ++i) result[i] = (*this)[i] + v;
        return result;
    }
    constexpr inline void operator+=(const T v) noexcept {
        for (unsigned int i = 0; i < N; ++i) (*this)[i] += v;
    }
    constexpr inline Vec<T, N> operator+(const Vec<T, N>& o) const noexcept {
        Vec<T, N> result;
        for (unsigned int i = 0; i < N; ++i) result[i] = (*this)[i] + o[i];
        return result;
    }
    constexpr inline void operator+=(const Vec<T, N>& o) noexcept {
        for (unsigned int i = 0; i < N; ++i) (*this)[i] += o[i];
    }
    constexpr inline Vec<T, N> operator-() const noexcept {
        Vec<T, N> result;
        for (unsigned int i = 0; i < N; ++i) result[i] = -(*this)[i];
        return result;
    }
    constexpr inline Vec<T, N> operator-(const T v) const noexcept {
        Vec<T, N> result;
        for (unsigned int i = 0; i < N; ++i) result[i] = (*this)[i] - v;
        return result;
    }
    constexpr inline void operator-=(const T v) noexcept {
        for (unsigned int i = 0; i < N; ++i) (*this)[i] -= v;
    }
    constexpr inline Vec<T, N> operator-(const Vec<T, N>& o) const noexcept {
        Vec<T, N> result;
        for (unsigned int i = 0; i < N; ++i) result[i] = (*this)[i] - o[i];
        return result;
    }
    constexpr inline void operator-=(const Vec<T, N>& o) noexcept {
        for (unsigned int i = 0; i < N; ++i) (*this)[i] -= o[i];
    }
    constexpr inline Vec<T, N> operator*(const T v) const noexcept {
        Vec<T, N> result;
        for (unsigned int i = 0; i < N; ++i) result[i] = (*this)[i] * v;
        return result;
    }
    constexpr inline void operator*=(const T f) noexcept {
        for (unsigned int i = 0; i < N; ++i) (*this)[i] *= f;
    }
    constexpr inline Vec<T, N> operator*(const Vec<T, N>& o) const noexcept {
        Vec<T, N> result;
        for (unsigned int i = 0; i < N; ++i) result[i] = (*this)[i] * o[i];
        return result;
    }
    constexpr inline void operator*=(const Vec<T, N>& o) noexcept {
        for (unsigned int i = 0; i < N; ++i) (*this)[i] *= o[i];
    }
    constexpr inline Vec<T, N> operator/(const T v) const noexcept {
        Vec<T, N> result;
        for (unsigned int i = 0; i < N; ++i) result[i] = (*this)[i] / v;
        return result;
    }
    constexpr inline void operator/=(const T v) noexcept {
        for (unsigned int i = 0; i < N; ++i) (*this)[i] /= v;
    }
    constexpr inline T module2() const noexcept {
        T result = 0;
        for (unsigned int i = 0; i < N; ++i) result += (*this)[i] * (*this)[i];
        return result;
    }
    constexpr inline float module() const noexcept { return sqrt(module2()); }
    constexpr inline Vec<T, N> normalized(const T l = 1) const noexcept {
        static_assert(std::is_floating_point_v<T>,
                      "Type must be a floating point");
        float mod = l / module();
        Vec<T, N> result;
        for (unsigned int i = 0; i < N; ++i) result[i] = (*this)[i] * mod;
        return result;
    }
    constexpr inline T max() const noexcept {
        T current_max = 0;
        for (unsigned int i = 0; i < N; ++i)
            current_max = std::max(current_max, (*this)[i]);
        return current_max;
    }

    template <typename T2>
    inline Vec<T2, N> cast_to() const {
        Vec<T2, N> result;
        for (unsigned int i = 0; i < N; ++i) result[i] = (T2)(*this)[i];
        return result;
    }

    template <std::size_t Index>
    const T get() const {
        return (*this)[Index];
    }

    friend std::ostream& operator<<(std::ostream& s, const Vec<T, N>& v) {
        s << "(";
        for (unsigned int i = 0; i < N - 1; ++i) {
            s << v[i] << ", ";
        }
        s << v[N - 1] << ")";
        return s;
    }
};

template <typename T, unsigned int N>
struct std::tuple_size<Vec<T, N>> : std::integral_constant<std::size_t, N> {};

template <std::size_t Index, typename T, unsigned int N>
struct std::tuple_element<Index, Vec<T, N>> {
    using type = T;
};

template <typename T, unsigned int N>
constexpr T dot(const Vec<T, N>& u, const Vec<T, N>& v) {
    T result = 0;
    for (int i = 0; i < N; ++i) result += u[i] * v[i];
    return result;
}

template <typename T>
constexpr Vec<T, 3> cross(const Vec<T, 3>& u, const Vec<T, 3>& v) {
    Vec<T, 3> result{u.y() * v.z() - u.z() * v.y(),
                     u.z() * v.x() - u.x() * v.z(),
                     u.x() * v.y() - u.y() * v.x()};
    return result;
}

template <typename T>
constexpr Vec<T, 4> cross(const Vec<T, 4>& u, const Vec<T, 4>& v) {
    Vec<T, 4> result{u.y() * v.z() - u.z() * v.y(),
                     u.z() * v.x() - u.x() * v.z(),
                     u.x() * v.y() - u.y() * v.x(), 0.0f};
    return result;
}

using Vec2f = Vec<float, 2>;
using Vec2i = Vec<int, 2>;
using Vec2u = Vec<unsigned int, 2>;

using Vec3f = Vec<float, 3>;
using Vec3i = Vec<int, 3>;
using Vec3u = Vec<unsigned int, 3>;

using Vec4f = Vec<float, 4>;
using Vec4i = Vec<int, 4>;
using Vec4u = Vec<unsigned int, 4>;

/// VECTOR OF VECTORS (based on flowering vector theme) ///

template <typename T, unsigned int N>
class VecList : public std::vector<Vec<T, N>> {
   private:
    using Base = std::vector<Vec<T, N>>;

   public:
    VecList() : Base() {}
    VecList(size_t count, const Vec<T, N>& value = Vec<T, N>())
        : Base(count, value) {}

    T* data_flat() { return (T*)this->Base::data(); }
    const T* data_flat() const { return (const T*)this->Base::data(); }
    size_t size_flat() const { return this->Base::size() * N; }

    constexpr inline void append(const VecList<T, N>& other) {
        this->Base::insert(this->Base::end(), other.begin(), other.end());
    }

    constexpr inline VecList<T, 3> divide_by_homogeneous() const {
        static_assert(N == 4,
                      "divide_by_homogeneous only works with Vec4 objects");
        const size_t M = this->Base::size();
        VecList<T, 3> result(M);
        auto origin = this->Base::begin();
        auto target = result.begin();
        for (size_t i = 0; i < M; ++i, ++origin, ++target) {
            target->x() = origin->x() / origin->w();
            target->y() = origin->y() / origin->w();
            target->z() = origin->z() / origin->w();
        }
        return result;
    }

    friend std::ostream& operator<<(std::ostream& s, const VecList<T, N>& a) {
        s << "[" << std::endl;
        for (int k = 0; k < a.Base::size(); ++k)
            s << "  " << k << ": " << a[k] << std::endl;
        s << "]";
        return s;
    }
};

using VecList3f = VecList<float, 3>;
using VecList3i = VecList<int, 3>;
using VecList3u = VecList<unsigned int, 3>;

using VecList4f = VecList<float, 4>;
using VecList4i = VecList<int, 4>;
using VecList4u = VecList<unsigned int, 4>;

/// MATRIX ///

template <typename T, unsigned int N>
class Mat : public std::array<Vec<T, N>, N> {
   private:
    using Base = std::array<Vec<T, N>, N>;

   public:
    static constexpr Mat<T, N> identity() {
        Mat<T, N> result;
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j) result(i, j) = i == j ? 1 : 0;
        return result;
    }

    constexpr inline T& operator()(const size_t i, const size_t j) {
        return (*this).at(j).at(i);
    }
    constexpr inline const T& operator()(const size_t i, const size_t j) const {
        return (*this).at(j).at(i);
    }
    constexpr inline T& operator[](const size_t i) = delete;

    constexpr inline Mat<T, N> operator*(const Mat<T, N>& o) const {
        Mat<T, N> result;
        for (int i = 0; i < N; ++i) result.at(i).fill(0);
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j)
                for (int k = 0; k < N; ++k)
                    result(i, j) += (*this)(i, k) * o(k, j);
        return result;
    }
    constexpr inline Mat<T, N> operator*(const T f) const {
        Mat<T, N> result;
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j) result(i, j) = (*this)(i, j) * f;
        return result;
    }
    constexpr inline Vec<T, N> operator*(const Vec<T, N>& v) const {
        Vec<T, N> result;
        result.fill(0);
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j) result[i] += (*this)(i, j) * v[j];
        return result;
    }
    constexpr inline VecList<T, N> operator*(const VecList<T, N>& a) const {
        const size_t M = a.size();
        VecList<T, N> result(M);
        for (int k = 0; k < M; ++k)
            for (int i = 0; i < N; ++i)
                for (int j = 0; j < N; ++j)
                    result[k][i] += (*this)(i, j) * a[k][j];
        return result;
    }
    constexpr inline void operator*=(const T f) {
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j) (*this)(i, j) *= f;
    }
    constexpr inline Mat<T, N> operator/(const T f) const {
        Mat<T, N> result;
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j) result(i, j) = (*this)(i, j) * f;
        return result;
    }
    constexpr inline void operator/=(const T f) {
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j) (*this)(i, j) /= f;
    }

    constexpr inline Mat<T, N> transpose() const {
        Mat<T, N> result;
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j) result(i, j) = (*this)(j, i);
        return result;
    }

    template <typename T2>
    constexpr inline operator Mat<T2, N>() const {
        Mat<T2, N> result;
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j) result(i, j) = (T2)((*this)(i, j));
        return result;
    }

    friend std::ostream& operator<<(std::ostream& s, const Mat<T, N>& v) {
        for (int i = 0; i < N; ++i) {
            if (i == 0)
                s << "/ ";
            else if (i == N - 1)
                s << "\\ ";
            else
                s << "| ";

            for (int j = 0; j < N - 1; ++j) {
                s << v(i, j) << " ";
            }
            s << v(i, N - 1);

            if (i == 0)
                s << " \\" << std::endl;
            else if (i == N - 1)
                s << " /" << std::endl;
            else
                s << " |" << std::endl;
        }
        return s;
    }
};

template <typename T>
class Mat3 : public Mat<T, 3> {
   public:
    constexpr Mat3(T x = 0) : Mat<T, 3>() {
        for (int i = 0; i < 3; ++i) (*this).at(i).fill(x);
    }
    constexpr Mat3(const Vec<T, 3>& u, const Vec<T, 3>& v, const Vec<T, 3>& w,
                   const Vec<T, 3>& o)
        : Mat<T, 3>{u, v, w} {}
    constexpr Mat3(T a, T b, T c, T d, T e, T f, T g, T h, T i)
        : Mat<T, 3>{Vec<T, 3>{a, d, g}, Vec<T, 3>{b, e, h},
                    Vec<T, 3>{c, f, i}} {}
    constexpr Mat3(const Mat<T, 3>&& m) : Mat<T, 3>(m) {}
};

template <typename T>
class Mat4 : public Mat<T, 4> {
   public:
    constexpr Mat4(T x = 0) : Mat<T, 4>() {
        for (int i = 0; i < 4; ++i) (*this).at(i).fill(x);
    }
    constexpr Mat4(const Vec<T, 4>& u, const Vec<T, 4>& v, const Vec<T, 4>& w,
                   const Vec<T, 4>& o)
        : Mat<T, 4>{u, v, w, o} {}
    constexpr Mat4(T a, T b, T c, T d, T e, T f, T g, T h, T i, T j, T k, T l,
                   T m, T n, T o, T p)
        : Mat<T, 4>{Vec<T, 4>{a, e, i, m}, Vec<T, 4>{b, f, j, n},
                    Vec<T, 4>{c, g, k, o}, Vec<T, 4>{d, h, l, p}} {}
    constexpr Mat4(const Mat<T, 4>&& m) : Mat<T, 4>(m) {}

    static constexpr Mat4<T> translation(float x, float y, float z) {
        return {0, 0, 0, x,  //
                0, 0, 0, y,  //
                0, 0, 0, z,  //
                0, 0, 0, 1};
    }
    static constexpr Mat4<T> rotation_X(float rad) {
        return {1, 0,         0,          0,  //
                0, cosf(rad), -sinf(rad), 0,  //
                0, sinf(rad), cosf(rad),  0,  //
                0, 0,         0,          1};
    }
    static constexpr Mat4<T> rotation_Y(float rad) {
        return {cosf(rad),  0, sinf(rad), 0,  //
                0,          1, 0,         0,  //
                -sinf(rad), 0, cosf(rad), 0,  //
                0,          0, 0,         1};
    }
    static constexpr Mat4<T> rotation_Z(float rad) {
        return {cosf(rad), -sinf(rad), 0, 0,  //
                sinf(rad), cosf(rad),  0, 0,  //
                0,         0,          1, 0,  //
                0,         0,          0, 1};
    }
    static inline Mat4<T> rotation_axis_angle(Vec4f axis, float rad) {
        float c = cosf(rad), mc = 1 - c;
        float s = sinf(rad), ms = 1 - s;
        float x = axis.x(), y = axis.y(), z = axis.z();
        // https://en.wikipedia.org/wiki/Rotation_matrix
        return Mat4(c + x * x * mc, x * y * mc - z * s, x * z * mc + y * s, 0,
                    y * x * mc + z * s, c + y * y * mc, y * z * mc - x * s, 0,
                    z * x * mc - y * s, z * y * mc + x * s, c + z * z * mc, 0,
                    0, 0, 0, 1);
    }
    static constexpr Mat4<T> scale(T x, T y, T z) {
        return {x, 0, 0, 0,  //
                0, y, 0, 0,  //
                0, 0, z, 0,  //
                0, 0, 0, 1};
    }
    static inline Mat4<T> change_of_basis(const Vec<T, 4>& u,
                                          const Vec<T, 4>& v,
                                          const Vec<T, 4>& w,
                                          const Vec<T, 4>& o) {
        return Mat4<T>(u, v, w, o);
    }

   private:
    // (row, col) cofactor of the matrix
    // https://en.wikipedia.org/wiki/Minor_(linear_algebra)
    T cofactor(const size_t row, const size_t col) const {
        // a, b and c are indices for accessing the 3x3 minor
        // using the 4x4 original matrix
        // for example, if row = 1 (second row) then the 3x3
        // matrix is made from rows ax = 0, bx = 2 and cx = 3
        int ax = row > 0 ? 0 : 1;
        int bx = row > 1 ? 1 : 2;
        int cx = row > 2 ? 2 : 3;
        int ay = col > 0 ? 0 : 1;
        int by = col > 1 ? 1 : 2;
        int cy = col > 2 ? 2 : 3;
        // determinant of 3x3 minor
        float determinant =
            (*this)(ay, ax) * (*this)(by, bx) * (*this)(cy, cx) +
            (*this)(ay, bx) * (*this)(by, cx) * (*this)(cy, ax) +
            (*this)(ay, cx) * (*this)(by, ax) * (*this)(cy, bx) -
            (*this)(ay, cx) * (*this)(by, bx) * (*this)(cy, ax) -
            (*this)(ay, ax) * (*this)(by, cx) * (*this)(cy, bx) -
            (*this)(ay, bx) * (*this)(by, ax) * (*this)(cy, cx);
        // cofactor calculation (see wikipedia link above)
        float sign = ((row + col) % 2 == 0 ? 1.0f : -1.0f);
        return sign * determinant;
    }

   public:
    Mat4<T> inverse() const {
        // for now, transposed adjugate, re-transposed at the end
        Mat4<T> result;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j) result(i, j) = cofactor(i, j);

        // determinant of A using first row/col of A/adjugate
        T determinant =
            result(0, 0) * (*this)(0, 0) + result(0, 1) * (*this)(1, 0) +
            result(0, 2) * (*this)(2, 0) + result(0, 3) * (*this)(3, 0);

        // Minv = Madj / det(A)
        result /= determinant;
        return result;
    }
};

using Mat3f = Mat3<float>;
using Mat3i = Mat3<int>;
using Mat3u = Mat3<unsigned int>;

using Mat4f = Mat4<float>;
using Mat4i = Mat4<int>;
using Mat4u = Mat4<unsigned int>;