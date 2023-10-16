#pragma once

#include <array>
#include <ostream>

/// VECTOR ///

template <typename T, unsigned int N>
class Vec : public std::array<T, N> {
   public:
    constexpr T x() const { return (*this)[0]; }
    constexpr T y() const {
        static_assert(N >= 2, "Vec does not have a Y component");
        return (*this)[1];
    }
    constexpr T z() const {
        static_assert(N >= 3, "Vec does not have a Z component");
        return (*this)[2];
    }
    constexpr T w() const {
        static_assert(N >= 4, "Vec does not have a W component");
        return (*this)[3];
    }

    constexpr inline bool operator==(const Vec<T, N> &o) const noexcept {
        for (int i = 0; i < N; ++i)
            if ((*this)[i] != o[i]) return false;
        return true;
    }
    constexpr inline Vec<T, N> operator+(const Vec<T, N> &o) const noexcept {
        Vec<T, N> result;
        for (int i = 0; i < N; ++i) result[i] = (*this)[i] + o[i];
        return result;
    }
    constexpr inline void operator+=(const Vec<T, N> &o) noexcept {
        for (int i = 0; i < N; ++i) (*this)[i] += o[i];
    }
    constexpr inline Vec<T, N> operator-(const Vec<T, N> &o) const noexcept {
        Vec<T, N> result;
        for (int i = 0; i < N; ++i) result[i] = (*this)[i] - o[i];
        return result;
    }
    constexpr inline Vec<T, N> operator-() const noexcept {
        Vec<T, N> result;
        for (int i = 0; i < N; ++i) result[i] = -(*this)[i];
        return result;
    }
    constexpr inline void operator-=(const Vec<T, N> &o) noexcept {
        for (int i = 0; i < N; ++i) (*this)[i] -= o[i];
    }
    constexpr inline Vec<T, N> operator*(const float f) const noexcept {
        Vec<T, N> result;
        for (int i = 0; i < N; ++i) result[i] = (*this)[i] * f;
        return result;
    }
    constexpr inline void operator*=(const float f) noexcept {
        for (int i = 0; i < N; ++i) (*this)[i] *= f;
    }
    constexpr inline Vec<T, N> operator*(const Vec<T, N> &o) const noexcept {
        Vec<T, N> result;
        for (int i = 0; i < N; ++i) result[i] = (*this)[i] * o[i];
        return result;
    }
    constexpr inline void operator*=(const Vec<T, N> &o) noexcept {
        for (int i = 0; i < N; ++i) (*this)[i] *= o[i];
    }
    constexpr inline Vec<T, N> operator/(const float f) const noexcept {
        Vec<T, N> result;
        for (int i = 0; i < N; ++i) result[i] = (*this)[i] / f;
        return result;
    }
    constexpr inline void operator/=(const float f) noexcept {
        for (int i = 0; i < N; ++i) (*this)[i] /= f;
    }
    constexpr inline T module2() const noexcept {
        T result = 0;
        for (int i = 0; i < N; ++i) result += (*this)[i] * (*this)[i];
        return result;
    }
    constexpr inline float module() const noexcept { return sqrt(module2()); }
    constexpr inline Vec<T, N> normalize(float l = 1) const noexcept {
        float mod = l / module();
        Vec<T, N> result;
        for (int i = 0; i < N; ++i) result[i] = (*this)[i] * mod;
        return result;
    }

    friend std::ostream &operator<<(std::ostream &s, const Vec<T, N> &v) {
        s << "(";
        for (int i = 0; i < N - 1; ++i) {
            s << v[i] << ", ";
        }
        s << v[N - 1] << ")";
        return s;
    }
};

template <typename T>
class Vec3 : public Vec<T, 3> {
   public:
    constexpr Vec3(T x = 0) : Vec<T, 3>{x, x, x} {}
    constexpr Vec3(T x, T y, T z) : Vec<T, 3>{x, y, z} {}
    constexpr Vec3(const Vec<T, 3> &&v) : Vec<T, 3>(v) {}
};

template <typename T>
class Vec4 : public Vec<T, 4> {
   public:
    constexpr Vec4(T x = 0) : Vec<T, 4>{x, x, x, x} {}
    constexpr Vec4(Vec3<T> v, T w = 0) : Vec<T, 4>{v.x(), v.y(), v.z(), w} {}
    constexpr Vec4(T x, T y, T z, T w = 0) : Vec<T, 4>{x, y, z, w} {}
    constexpr Vec4(const Vec<T, 4> &&v) : Vec<T, 4>(v) {}
};

template <typename T, unsigned int N>
constexpr T dot(const Vec<T, N> &u, const Vec<T, N> &v) {
    T result = 0;
    for (int i = 0; i < N; ++i) result += u[i] * v[i];
    return result;
}

template <typename T>
constexpr Vec<T, 3> cross(const Vec<T, 3> &u, const Vec<T, 3> &v) {
    Vec<T, 3> result{u.y() * v.z() - u.z() * v.y(),
                     u.z() * v.x() - u.x() * v.z(),
                     u.x() * v.y() - u.y() * v.x()};
    return result;
}

template <typename T>
constexpr Vec<T, 4> cross(const Vec<T, 4> &u, const Vec<T, 4> &v) {
    Vec<T, 4> result{u.y() * v.z() - u.z() * v.y(),
                     u.z() * v.x() - u.x() * v.z(),
                     u.x() * v.y() - u.y() * v.x(), 0.0f};
    return result;
}

using Vec3f = Vec3<float>;
using Vec3i = Vec3<int>;
using Vec3u = Vec3<unsigned int>;

using Vec4f = Vec4<float>;
using Vec4i = Vec4<int>;
using Vec4u = Vec4<unsigned int>;

template <typename T, unsigned int N, unsigned int M>
class VecArray : public std::array<Vec<T, N>, M> {
   public:
    constexpr inline Vec<T, N * M> flatten() const {
        Vec<T, N * M> result;
        auto it = result.begin();
        for (int k = 0; k < M; ++k)
            for (int i = 0; i < N; ++i) *(it++) = (*this)[k][i];
        return result;
    }

    constexpr inline Vec<T, 3 * M> flatten_homogeneous() const {
        static_assert(N == 4,
                      "flatten_homogeneous only works with Vec4 objects");
        Vec<T, 3 * M> result;
        auto it = result.begin();
        for (int k = 0; k < M; ++k)
            for (int i = 0; i < 3; ++i) *(it++) = (*this)[k][i] / (*this)[k][3];
        return result;
    }

    friend std::ostream &operator<<(std::ostream &s,
                                    const VecArray<T, N, M> &a) {
        s << "[" << std::endl;
        for (int k = 0; k < M; ++k) s << "  " << k << ": " << a[k] << std::endl;
        s << "]";
        return s;
    }
};

template <unsigned int M>
using VecArray3f = VecArray<float, 3, M>;
template <unsigned int M>
using VecArray3i = VecArray<int, 3, M>;
template <unsigned int M>
using VecArray3u = VecArray<unsigned int, 3, M>;

template <unsigned int M>
using VecArray4f = VecArray<float, 4, M>;
template <unsigned int M>
using VecArray4i = VecArray<int, 4, M>;
template <unsigned int M>
using VecArray4u = VecArray<unsigned int, 4, M>;

/// MATRIX ///

template <typename T, unsigned int N>
class Mat : public std::array<Vec<T, N>, N> {
   public:
    static constexpr Mat<T, N> identity() {
        Mat<T, N> result;
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j) result(i, j) = i == j ? 1 : 0;
        return result;
    }

    constexpr inline T &operator()(const size_t i, const size_t j) {
        return (*this).at(j).at(i);
    }
    constexpr inline const T &operator()(const size_t i, const size_t j) const {
        return (*this).at(j).at(i);
    }
    constexpr inline T &operator[](const size_t i) = delete;

    constexpr inline Mat<T, N> operator*(const Mat<T, N> &o) const {
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
    constexpr inline Vec<T, N> operator*(const Vec<T, N> &v) const {
        Vec<T, N> result;
        result.fill(0);
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j) result[i] += (*this)(i, j) * v[j];
        return result;
    }
    template <unsigned int M>
    constexpr inline VecArray<T, N, M> operator*(
        const VecArray<T, N, M> &a) const {
        VecArray<T, N, M> result;
        for (int k = 0; k < M; ++k) result[k].fill(0);
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

    friend std::ostream &operator<<(std::ostream &s, const Mat<T, N> &v) {
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
    constexpr Mat3(const Vec<T, 3> &u, const Vec<T, 3> &v, const Vec<T, 3> &w,
                   const Vec<T, 3> &o)
        : Mat<T, 3>{u, v, w} {}
    constexpr Mat3(T a, T b, T c, T d, T e, T f, T g, T h, T i)
        : Mat<T, 3>{Vec<T, 3>{a, d, g}, Vec<T, 3>{b, e, h},
                    Vec<T, 3>{c, f, i}} {}
    constexpr Mat3(const Mat<T, 3> &&m) : Mat<T, 3>(m) {}
};

template <typename T>
class Mat4 : public Mat<T, 4> {
   public:
    constexpr Mat4(T x = 0) : Mat<T, 4>() {
        for (int i = 0; i < 4; ++i) (*this).at(i).fill(x);
    }
    constexpr Mat4(const Vec<T, 4> &u, const Vec<T, 4> &v, const Vec<T, 4> &w,
                   const Vec<T, 4> &o)
        : Mat<T, 4>{u, v, w, o} {}
    constexpr Mat4(T a, T b, T c, T d, T e, T f, T g, T h, T i, T j, T k, T l,
                   T m, T n, T o, T p)
        : Mat<T, 4>{Vec<T, 4>{a, e, i, m}, Vec<T, 4>{b, f, j, n},
                    Vec<T, 4>{c, g, k, o}, Vec<T, 4>{d, h, l, p}} {}
    constexpr Mat4(const Mat<T, 4> &&m) : Mat<T, 4>(m) {}

    static constexpr Mat4<T> translation(float x, float y, float z) {
        return {0, 0, 0, x,  //
                0, 0, 0, y,  //
                0, 0, 0, z,  //
                0, 0, 0, 1};
    }
    static constexpr Mat4<T> rotationX(float rad) {
        return {1, 0,         0,          0,  //
                0, cosf(rad), -sinf(rad), 0,  //
                0, sinf(rad), cosf(rad),  0,  //
                0, 0,         0,          1};
    }
    static constexpr Mat4<T> rotationY(float rad) {
        return {cosf(rad),  0, sinf(rad), 0,  //
                0,          1, 0,         0,  //
                -sinf(rad), 0, cosf(rad), 0,  //
                0,          0, 0,         1};
    }
    static constexpr Mat4<T> rotationZ(float rad) {
        return {cosf(rad), -sinf(rad), 0, 0,  //
                sinf(rad), cosf(rad),  0, 0,  //
                0,         0,          1, 0,  //
                0,         0,          0, 1};
    }
    static constexpr Mat4<T> rotationAxisAngle(Vec4f axis, float rad) {
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
    static constexpr Mat4<T> changeOfBasis(const Vec<T, 4> &u,
                                           const Vec<T, 4> &v,
                                           const Vec<T, 4> &w,
                                           const Vec<T, 4> &o) {
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