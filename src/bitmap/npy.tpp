/*
 * npy.tpp
 * Diego Royo Meneses - Jan. 2024
 *
 * Image saver with a format compatible with numpy's np.load(...)
 */
#include <cmath>
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
    if constexpr (sizeof(typename T::type) != 4)
        throw detail::CommonBitmapException(
            "NPY save only supports Bitmaps where the underlying type is 4 "
            "bytes long");
    size_t width = image.width();
    size_t height = image.height();
    std::string descr = "";
    if constexpr (std::is_floating_point_v<typename T::type>) {
        descr = "<f4";
    } else if constexpr (std::is_signed_v<typename T::type>) {
        descr = "<i4";
    } else {
        descr = "<u4";
    }
    const uint8_t magic[] = {0x93, 'N', 'U', 'M', 'P', 'Y', 0x01, 0x00};
    const std::string header_str = "{'descr': '" + descr + "', " +          //
                                   "'fortran_order': False, " +             //
                                   "'shape': (" + std::to_string(height) +  //
                                   ", " + std::to_string(width) +           //
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

    for (size_t y = 0; y < image.height(); ++y) {
        for (size_t x = 0; x < image.width(); ++x) {
            for (size_t c = 0; c < channels; ++c) {
                // store 4 bytes of float in little endian
                const uint32_t p =
                    *reinterpret_cast<const uint32_t*>(&image(x, y)[c]);
                for (int b = 0; b < 4; ++b) {
                    char v = (p >> b * 8) & 0xFF;
                    file.write(&v, 1);
                }
            }
        }
    }
}

};  // namespace common