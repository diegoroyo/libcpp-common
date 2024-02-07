/*
 * npy.tpp
 * Diego Royo Meneses - Jan. 2024
 *
 * Image saver with a format compatible with numpy's np.load(...)
 */
#include <cmath>
#include <complex>
#include <cstdint>
#include <iostream>

#include "libcpp-common/bitmap.h"
#include "libcpp-common/detail/exception.h"

namespace common {

template <typename T>
struct bitmap_channels;

template <typename T>
void save_npy(std::ofstream& file, const Grid2D<T>& image) {
    constexpr uint8_t channels = bitmap_channels<T>::value;
    size_t width = image.width();
    size_t height = image.height();
    char element_size = 0;
    std::string descr = "";
#define IS_TYPE(x) std::is_same_v<typename T::type, x>
    if constexpr (IS_TYPE(std::complex<float>)) {
        descr = "<c8";
    } else if constexpr (IS_TYPE(float)) {
        descr = "<f4";
    } else if constexpr (IS_TYPE(int)) {
        descr = "<i4";
    } else if constexpr (IS_TYPE(unsigned int)) {
        descr = "<u4";
    } else {
        throw detail::CommonBitmapException(
            "Unsupported data type for NPY save.");
    }
#undef IS_TYPE
    const uint8_t magic[] = {0x93, 'N', 'U', 'M', 'P', 'Y', 0x01, 0x00};
    const std::string header_str = "{'descr': '" + descr + "', " +         //
                                   "'fortran_order': False, " +            //
                                   "'shape': (" + std::to_string(width) +  //
                                   ", " + std::to_string(height) +         //
                                   ", " + std::to_string(channels) + "), }";
    const char* header = header_str.c_str();
    const uint16_t header_len = header_str.size();
    const uint16_t content_len = sizeof(magic) + 2 + header_len + 1;
    // round to next multiple of 64
    uint16_t padded_len = (uint16_t)(std::ceil(content_len / 64.0f)) * 64;

    file.write(reinterpret_cast<const char*>(magic), sizeof(magic));
    // write in little endian
    uint16_t written_len = padded_len - sizeof(magic) - 2;
    char len_1 = written_len & 0xFF;
    char len_2 = written_len >> 8;
    file.write(&len_1, 1);
    file.write(&len_2, 1);
    file.write(header, header_len);
    char space = '\x20', newline = '\n';
    for (int i = 0; i < padded_len - content_len; ++i) file.write(&space, 1);
    file.write(&newline, 1);

    for (size_t x = 0; x < image.width(); ++x) {
        for (size_t y = 0; y < image.height(); ++y) {
            for (size_t c = 0; c < channels; ++c) {
                // store X bytes of float in little endian
                const char* p = reinterpret_cast<const char*>(&image(x, y)[c]);
                for (int b = 0; b < sizeof(typename T::type); ++b) {
                    file.write(p++, 1);
                }
            }
        }
    }
}

};  // namespace common