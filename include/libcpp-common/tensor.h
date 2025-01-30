/*
 * tensor.h
 * Diego Royo Meneses - Jan. 2025
 *
 * NumPy-like tensor type
 */

#include <array>
#include <cstring>
#include <iostream>
#include <tuple>
#include <type_traits>
#include <utility>

#include "libcpp-common/bitmap.h"
#include "libcpp-common/detail/exception.h"
#include "libcpp-common/geometry.h"

namespace common {

namespace detail {

template <typename T, size_t N>
struct NestedInitializerList {
    using type =
        std::initializer_list<typename NestedInitializerList<T, N - 1>::type>;
};

template <typename T>
struct NestedInitializerList<T, 0> {
    using type = T;
};

};  // namespace detail

template <typename T, size_t... Shape>
class Tensor {
   public:
    using type = T;
    static constexpr size_t ndim = sizeof...(Shape);
    static constexpr size_t size = (Shape * ...);
    static constexpr std::array<size_t, ndim> shape = {Shape...};

    template <size_t I>
    static constexpr size_t get_dim() {
        return std::array<size_t, sizeof...(Shape)>{Shape...}[I];
    }

   private:
    static constexpr std::array<size_t, ndim> compute_strides() {
        std::array<size_t, ndim> strides = {0};
        size_t stride = 1;

        for (int i = ndim - 1; i >= 0; --i) {
            strides[i] = stride;
            stride *= shape[i];
        }
        return strides;
    }

   public:
    static constexpr std::array<size_t, ndim> strides = compute_strides();

   private:
    T m_data[size];

    static constexpr size_t compute_1d_index(
        const std::array<size_t, ndim>& indices) {
        size_t idx = 0;
        for (size_t i = 0; i < ndim; ++i) {
            idx += indices[i] * strides[i];
        }
        return idx;
    }

    /* Constructors */
   public:
    // Fill constructor
    Tensor(T initial_value = 0) {
        std::fill(std::begin(m_data), std::end(m_data), initial_value);
    }

    // Nested initializer list constructor
    // e.g. Tensor<float, 3, 2> can be constructed with
    // Tensor<float, 3, 2> t({{1, 2}, {3, 4}, {5, 6}});
   private:
    template <size_t Depth>
    using InitializerType =
        typename detail::NestedInitializerList<T, ndim - Depth>::type;

   public:
    Tensor(const InitializerType<0>& init) {
        size_t index = 0;
        check_sizes<0>(init);
        copy_data<0>(init, index);
    }

   private:
    template <size_t Depth>
    void check_sizes(const InitializerType<Depth>& list) {
        if constexpr (Depth >= ndim) {
            throw detail::CommonTensorException(
                "Tensor's argument data has " + std::to_string(Depth) +
                " dimensions, but its shape expects " + std::to_string(ndim));
        }
        if (list.size() != shape[Depth]) {
            throw detail::CommonTensorException(
                "Tensor's argument data has length " +
                std::to_string(list.size()) + " in dimension " +
                std::to_string(Depth) + ", but its shape expects length " +
                std::to_string(shape[Depth]));
        }
        if constexpr (Depth + 1 < ndim) {  // recurse if not last dimension
            for (const InitializerType<Depth + 1>& sublist : list) {
                check_sizes<Depth + 1>(sublist);
            }
        }
    }

    // Recursively copy data into flat array
    template <size_t Depth>
    void copy_data(const InitializerType<Depth>& list, size_t& index) {
        if constexpr (Depth + 1 < ndim) {  // recurse if not last dimension
            for (const InitializerType<Depth + 1>& sublist : list) {
                copy_data<Depth + 1>(sublist, index);
            }
        } else {  // array of T: copy them to memory
            for (const T& val : list) {
                if (index >= size)
                    throw detail::CommonTensorException("Too many elements");
                (*this).at(index++) = val;
            }
        }
    }

   public:
    static Tensor<T, Shape...> zeros() { return Tensor<T, Shape...>(0); }
    static Tensor<T, Shape...> ones() { return Tensor<T, Shape...>(1); }

    template <unsigned int N,
              typename = std::enable_if_t<
                  ndim == 1 && get_dim<0>() == static_cast<size_t>(N)>>
    Tensor(const common::Vec<T, N>& vec) : m_data() {
        std::copy(vec.begin(), vec.end(), std::begin(m_data));
    }

    template <unsigned int N, unsigned int M,
              typename = std::enable_if_t<
                  ndim == 2 && (get_dim<0>() == static_cast<size_t>(N) &&
                                get_dim<1>() == static_cast<size_t>(M))>>
    Tensor(const common::Mat<T, N, M>& mat) : m_data() {
        std::copy(mat.front().begin(), mat.back().end(), std::begin(m_data));
    }

    Tensor(const std::array<T, size>& data) : m_data() {
        std::copy(data.begin(), data.end(), std::begin(m_data));
    }

    /* Access operators */
   public:
    template <typename... Indices,
              typename = std::enable_if_t<
                  (sizeof...(Indices) == ndim) &&
                  (std::is_convertible_v<size_t, Indices> && ...)>>
    T& operator()(Indices... indices) {
        auto indices_arr = std::array{static_cast<size_t>(indices)...};
        return (*this).at(compute_1d_index(indices_arr));
    }

    template <typename... Indices,
              typename = std::enable_if_t<
                  (sizeof...(Indices) == ndim) &&
                  (std::is_convertible_v<size_t, Indices> && ...)>>
    const T& operator()(Indices... indices) const {
        auto indices_arr = std::array{static_cast<size_t>(indices)...};
        return (*this).at(compute_1d_index(indices_arr));
    }

    T& at(size_t i) { return m_data[i]; }
    const T& at(size_t i) const { return m_data[i]; }

    /* Pretty print */
   public:
    friend std::ostream& operator<<(std::ostream& s,
                                    const Tensor<T, Shape...>& t) {
        for (size_t i = 0; i < ndim; ++i) s << "[ ";
        for (size_t i = 0; i < size; ++i) {
            s << t.at(i) << " ";
            if (i == size - 1) continue;
            for (size_t j = 0; j < ndim - 1; ++j) {
                if ((i + 1) % strides[j] == 0) {
                    s << "] [ ";
                }
            }
        }
        for (size_t i = 0; i < ndim - 1; ++i) s << "] ";
        s << "]";
        return s;
    }

    /* Arithmetic operators */
   public:
#define DEFINE_ARITHMETIC_OPERATOR(op)                                        \
    constexpr inline Tensor<T, Shape...> operator op(const T& scalar) const { \
        auto result = Tensor<T, Shape...>::zeros();                           \
        for (size_t i = 0; i < size; ++i)                                     \
            result.at(i) = (*this).at(i) op scalar;                           \
        return result;                                                        \
    }                                                                         \
    constexpr inline Tensor<T, Shape...> operator op(                         \
        const Tensor<T, Shape...>& other) const {                             \
        auto result = Tensor<T, Shape...>::zeros();                           \
        for (size_t i = 0; i < size; ++i)                                     \
            result.at(i) = (*this).at(i) op other.at(i);                      \
        return result;                                                        \
    }                                                                         \
    constexpr inline void operator op##=(const T& scalar) {                   \
        for (size_t i = 0; i < size; ++i) at(i) op## = scalar;                \
    }                                                                         \
    constexpr inline void operator op##=(const Tensor<T, Shape...>& other) {  \
        for (size_t i = 0; i < size; ++i) at(i) op## = other.at(i);           \
    }

    DEFINE_ARITHMETIC_OPERATOR(+)
    DEFINE_ARITHMETIC_OPERATOR(-)
    // DEFINE_ARITHMETIC_OPERATOR(*)  // defined below
    DEFINE_ARITHMETIC_OPERATOR(/)

    constexpr inline Tensor<T, Shape...> operator-() const {
        auto result = Tensor<T, Shape...>::zeros();
        for (size_t i = 0; i < size; ++i) result.at(i) = -at(i);
        return result;
    }
    constexpr inline Tensor<T, Shape...> operator*(const T& scalar) const {
        auto result = Tensor<T, Shape...>::zeros();
        for (size_t i = 0; i < size; ++i) result.at(i) = (*this).at(i) * scalar;
        return result;
    }
    constexpr inline void operator*=(const T& scalar) {
        for (size_t i = 0; i < size; ++i) (*this).at(i) *= scalar;
    }
    constexpr inline Tensor<T, Shape...> ewise_mult(
        const Tensor<T, Shape...>& other) const {
        auto result = Tensor<T, Shape...>::zeros();
        for (size_t i = 0; i < size; ++i)
            result.at(i) = (*this).at(i) * other.at(i);
        return result;
    }

    /* Matrix/vector-specific stuff */
    // Transpose
    template <size_t N = get_dim<0>(), size_t M = get_dim<1>()>
    constexpr inline std::enable_if_t<ndim == 2, Tensor<T, M, N>> transpose()
        const {
        auto result = Tensor<T, M, N>::zeros();
        for (size_t i = 0; i < N; ++i)
            for (size_t j = 0; j < M; ++j) result(j, i) = (*this)(i, j);
        return result;
    }

    // Vector-vector multiplication
    template <size_t N = get_dim<0>()>
    constexpr inline std::enable_if_t<ndim == 1, Tensor<T, N>> operator*(
        const Tensor<T, N>& vec) const {
        auto result = Tensor<T, N>::zeros();
        for (size_t i = 0; i < size; ++i) result(i) += (*this)(i)*vec(i);
        return result;
    }

    // Matrix-matrix multiplication
    template <size_t U,  //
              size_t N = get_dim<0>(), size_t M = get_dim<1>()>
    constexpr inline std::enable_if_t<ndim == 2, Tensor<T, N, U>> operator*(
        const Tensor<T, M, U>& mat) const {
        auto result = Tensor<T, N, U>::zeros();
        for (size_t i = 0; i < N; ++i)
            for (size_t j = 0; j < U; ++j)
                for (size_t k = 0; k < M; ++k)
                    result(i, j) += (*this)(i, k) * mat(k, j);
        return result;
    }

    // Matrix-vector multiplication
    template <size_t N = get_dim<0>(), size_t M = get_dim<1>()>
    constexpr inline std::enable_if_t<ndim == 2, Tensor<T, N>> operator*(
        const Tensor<T, M>& vec) const {
        auto result = Tensor<T, N>::zeros();
        for (size_t i = 0; i < N; ++i)
            for (size_t j = 0; j < M; ++j) result(i) += (*this)(i, j) * vec(j);
        return result;
    }

    /* Map/Reduce operations */
   public:
    template <typename MapFunc>
    constexpr inline Tensor<T, Shape...> map(const MapFunc& f) const {
        auto result = Tensor<T, Shape...>::zeros();
        for (unsigned int i = 0; i < size; ++i)
            result.at(i) = f((*this).at(i), i);
        return result;
    }

    constexpr inline T sum() const {
        T result = 0;
        for (size_t i = 0; i < size; ++i) result += (*this).at(i);
        return result;
    }

    /* Conversion to other types */
   public:
    template <size_t N = get_dim<0>()>
    operator std::enable_if_t<ndim == 1, common::Vec<T, N>>() const {
        common::Vec<T, N> result;
        std::copy(std::begin(m_data), std::end(m_data), result.begin());
        return result;
    }
    template <size_t N = get_dim<0>()>
    operator std::enable_if_t<ndim == 1, common::Color<T, N>>() const {
        common::Color<T, N> result;
        std::copy(std::begin(m_data), std::end(m_data), result.begin());
        return result;
    }
};

};  // namespace common