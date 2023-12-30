/*
 * ppm.tpp
 * Diego Royo Meneses - Dec. 2023
 *
 * Portable PixMap Format loader and saver
 */
#include <cstdint>
#include <fstream>

#include "libcpp-common/bitmap.h"

namespace common {

template <typename T>
struct bitmap_channels;

// template <typename T>
// bool test_ppm(std::ifstream& file);

// template <typename T>
// Grid2D<T> load_ppm(std::ifstream& file, const uint8_t channels);

template <typename T>
void save_ppm(std::ofstream& file, const Grid2D<T>& image) {
    constexpr uint8_t channels = bitmap_channels<T>::value;
    if constexpr (channels != 3)
        throw detail::CommonBitmapException(
            "PPM save only supports three-channel Bitmap objects");

    size_t width = image.width();
    size_t height = image.height();
    float image_max;
    if constexpr (std::is_floating_point_v<typename T::type>) {
        image_max = 1.0f;
    } else {
        image_max = 255.0f;
    }
    auto saved_image = image.template map<Color3u>([image_max](const T& e) {
        return (e.template cast_to<float>() / image_max * 255.0f)
            .template cast_to<unsigned int>();
    });

#define COMMON_write(t) \
    file.write(std::string(t).c_str(), std::string(t).size())

    COMMON_write("P3\n");
    COMMON_write("# Created using libcpp-common\n");
    std::string line = std::to_string(width) + " " +  //
                       std::to_string(height) + "\n";
    COMMON_write(line);
    COMMON_write("255\n");
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            const Color3u pixel = saved_image(x, y);
            line = std::to_string(pixel.r()) + "\t" +  //
                   std::to_string(pixel.g()) + "\t" +  //
                   std::to_string(pixel.b());
            COMMON_write(line);
            if (x != width - 1) COMMON_write("\t\t");
        }
        COMMON_write("\n");
    }

#undef COMMON_write
}

};  // namespace common