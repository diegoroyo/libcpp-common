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

namespace common {

template <typename T, size_t... Shape>
class Tensor {
   public:
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

    Tensor(T initial_value = 0) {
        std::fill(std::begin(m_data), std::end(m_data), initial_value);
    }

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
    static Tensor<T, Shape...> zeros() { return Tensor<T, Shape...>(0); }
    static Tensor<T, Shape...> ones() { return Tensor<T, Shape...>(1); }

    Tensor(const std::array<T, size>& data) : m_data(data) {}

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
            for (size_t j = 0; j < ndim - 1; ++j) {
                if (i == size - 1) {
                    for (size_t i = 0; i < ndim - 1; ++i) s << "] ";
                    s << "]";
                } else if ((i + 1) % strides[j] == 0) {
                    s << "] [ ";
                }
            }
        }
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
};

};  // namespace common