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

template <typename T>
bool test_ppm(std::ifstream& file) {
    char p = file.get();
    char three = file.get();

    bool header_ok = p == 'P' && three == '3';

    file.clear();
    file.seekg(0, std::ios::beg);

    return header_ok;
}

template <typename T>
Grid2D<T> load_ppm(std::ifstream& file) {
    constexpr uint8_t channels = bitmap_channels<T>::value;
    if constexpr (channels != 3)
        throw detail::CommonBitmapException(
            "PPM load only supports three-channel Bitmap objects");

    Grid2D<T> image;

    // Skip magic number header
    file.seekg(2, std::ios::beg);

    int phase = 0;
    size_t width, height;
    uint32_t depth;
    size_t x, y, c;
    while (phase != -1 && !file.eof()) {
        char peek = file.peek();
        if (std::isspace(peek)) {
            file.get();
            continue;
        }
        if (file.peek() == '#') {
            // comment, ignore until end of line
            std::string comment;
            std::getline(file, comment);
            continue;
        }
        switch (phase) {
            case 0:  // read width
                file >> width;
                phase = 1;
                break;
            case 1:  // read height
                file >> height;
                image.resize(width, height);
                phase = 2;
                break;
            case 2:  // read color depth
                file >> depth;
                x = y = c = 0;
                phase = 3;
                break;
            case 3:  // read color values
                uint32_t v;
                file >> v;
                if constexpr (std::is_floating_point_v<typename T::type>) {
                    image(x, y)[c++] = v / (float)depth;
                } else {
                    image(x, y)[c++] = 255 * v / (float)depth;
                }

                if (c == channels) {  // next column
                    c = 0;
                    x += 1;
                }
                if (x == width) {  // next row
                    x = 0;
                    y += 1;
                }
                if (y == height) phase = -1;  // finished
                break;
        }
    }

    if (file.eof())
        throw detail::CommonBitmapException(
            "PPM Unexpected error: EOF after reading all data?");
    char next;
    do {
        next = file.get();
    } while (next != EOF || std::isspace(next));
    if (next != EOF || !file.eof())
        throw detail::CommonBitmapException(
            "PPM Unexpected error: EOF not reached even after reading all "
            "data?");

    return image;
}

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
    auto saved_image = image.template map<Color<unsigned int, T::size>>(
        [image_max](const T& e) {
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