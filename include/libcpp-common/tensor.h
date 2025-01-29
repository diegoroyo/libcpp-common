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

#include "libcpp-common/detail/exception.h"

namespace common {

namespace detail {

template <size_t I, size_t... Shape>
constexpr size_t get_dimension() {
    // static_assert(I < sizeof...(Shape), "Dimension index out of bounds");
    return std::array<size_t, sizeof...(Shape)>{Shape...}[I];
}

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
                m_data[index++] = val;
            }
        }
    }

   public:
    static Tensor<T, Shape...> zeros() { return Tensor<T, Shape...>(0); }
    static Tensor<T, Shape...> ones() { return Tensor<T, Shape...>(1); }

    template <unsigned int N,
              typename = std::enable_if_t<
                  ndim == 1 && detail::get_dimension<0, Shape...>() ==
                                   static_cast<size_t>(N)>>
    Tensor(const common::Vec<T, N>& vec) : m_data() {
        std::copy(vec.begin(), vec.end(), std::begin(m_data));
    }

    template <
        unsigned int N, unsigned int M,
        typename = std::enable_if_t<
            (ndim == 2) &&
            detail::get_dimension<0, Shape...>() == static_cast<size_t>(N) &&
            detail::get_dimension<1, Shape...>() == static_cast<size_t>(M)>>
    Tensor(const common::Mat<T, N, M>& mat) : m_data() {
        std::copy(mat.front().begin(), mat.back().end(), std::begin(m_data));
    }

    Tensor(const std::array<T, size>& data) : m_data() {
        std::copy(data.begin(), data.end(), std::begin(m_data));
    }

    Tensor(const T* data) { std::memcpy(m_data, data, size * sizeof(T)); }

    /* Access operators */
   public:
    template <typename... Indices,
              typename = std::enable_if_t<
                  (sizeof...(Indices) == ndim) &&
                  (std::is_convertible_v<size_t, Indices> && ...)>>
    T& operator()(Indices... indices) {
        auto indices_arr = std::array{static_cast<size_t>(indices)...};
        return m_data[compute_1d_index(indices_arr)];
    }

    template <typename... Indices,
              typename = std::enable_if_t<
                  (sizeof...(Indices) == ndim) &&
                  (std::is_convertible_v<size_t, Indices> && ...)>>
    const T& operator()(Indices... indices) const {
        auto indices_arr = std::array{static_cast<size_t>(indices)...};
        return m_data[compute_1d_index(indices_arr)];
    }

    /* Pretty print */
   public:
    friend std::ostream& operator<<(std::ostream& s,
                                    const Tensor<T, Shape...>& t) {
        for (size_t i = 0; i < ndim; ++i) s << "[ ";
        for (size_t i = 0; i < size; ++i) {
            s << t.m_data[i] << " ";
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
    Tensor<T, Shape...> operator op(const T& scalar) const {                  \
        Tensor<T, Shape...> result;                                           \
        for (size_t i = 0; i < size; ++i) {                                   \
            result.m_data[i] = m_data[i] op scalar;                           \
        }                                                                     \
        return result;                                                        \
    }                                                                         \
    Tensor<T, Shape...> operator op(const Tensor<T, Shape...>& other) const { \
        Tensor<T, Shape...> result;                                           \
        for (size_t i = 0; i < size; ++i) {                                   \
            result.m_data[i] = m_data[i] op other.m_data[i];                  \
        }                                                                     \
        return result;                                                        \
    }                                                                         \
    void operator op##=(const T& scalar) {                                    \
        for (size_t i = 0; i < size; ++i) {                                   \
            m_data[i] op## = scalar;                                          \
        }                                                                     \
    }                                                                         \
    void operator op##=(const Tensor<T, Shape...>& other) {                   \
        for (size_t i = 0; i < size; ++i) {                                   \
            m_data[i] op## = other.m_data[i];                                 \
        }                                                                     \
    }

    DEFINE_ARITHMETIC_OPERATOR(+)
    DEFINE_ARITHMETIC_OPERATOR(-)
    DEFINE_ARITHMETIC_OPERATOR(*)
    DEFINE_ARITHMETIC_OPERATOR(/)

    Tensor<T, Shape...> operator-() const {
        Tensor<T, Shape...> result;
        for (size_t i = 0; i < size; ++i) {
            result.m_data[i] = -m_data[i];
        }
        return result;
    }

    /* Reduce operations */
   public:
    T sum() const {
        T result = 0;
        for (size_t i = 0; i < size; ++i) {
            result += m_data[i];
        }
        return result;
    }
};

};  // namespace common