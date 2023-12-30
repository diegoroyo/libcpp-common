/*
 * bitmap.tpp
 * Diego Royo Meneses - Dec. 2023
 *
 * Image data structure with read/write operations
 */

#include <filesystem>
#include <string_view>

namespace common {

template <typename T>
struct bitmap_channels;

template <typename T>
Grid2D<T> bitmap_load(const std::string& filename) {
    constexpr uint8_t channels = bitmap_channels<T>::value;
    static_assert(channels > 0,
                  "Invalid bitmap type to load. It must be one of: float, "
                  "unsigned int, or a Color.");
    std::ifstream file(filename);

    if (!file.is_open())
        throw detail::CommonBitmapException("Could not open file " +
                                            std::string(filename));

    // Write here all the loaders
    if (test_png<T>(file)) return load_png<T>(file, channels);

    throw detail::CommonBitmapException("No image loader found for file " +
                                        std::string(filename));
}

template <typename T>
void bitmap_save(const std::string& filename, const Grid2D<T>& image) {
    constexpr uint8_t channels = bitmap_channels<T>::value;
    static_assert(channels > 0,
                  "Invalid bitmap type to load. It must be one of: float, "
                  "unsigned int, or a Color.");
    std::ofstream file(filename, std::ios::binary);

    try {
        std::string view(filename);
#define COMMON_ends_with(t)     \
    view.size() >= strlen(t) && \
        view.compare(view.size() - strlen(t), strlen(t), t) == 0
        if (COMMON_ends_with(".ppm")) return save_ppm(file, image);
#undef COMMON_ends_with

        throw detail::CommonBitmapException("No image saver found for file " +
                                            std::string(filename));
    } catch (const detail::CommonBitmapException& exception) {
        std::filesystem::remove(filename);
        throw;
    }
}

};  // namespace common