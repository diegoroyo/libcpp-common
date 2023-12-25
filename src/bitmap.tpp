/*
 * bitmap.tpp
 * Diego Royo Meneses - Dec. 2023
 *
 * Image data structure with read/write operations
 */

namespace common {

template <typename T>
Grid2D<T> bitmap_load(const char* filename) {
    static_assert(std::is_same_v<T, float> ||             //
                      std::is_same_v<T, Color3f> ||       //
                      std::is_same_v<T, Color4f> ||       //
                      std::is_same_v<T, unsigned int> ||  //
                      std::is_same_v<T, Color3u> ||       //
                      std::is_same_v<T, Color4u>,         //
                  "Invalid bitmap type to load. It must be one of: float, "
                  "unsigned int, or a Color.");
    uint8_t channels;
    if constexpr (std::is_same_v<T, float> || std::is_same_v<T, unsigned int>) {
        channels = 1;
    } else if constexpr (std::is_same_v<T, Color3f> ||
                         std::is_same_v<T, Color3u>) {
        channels = 3;
    } else if constexpr (std::is_same_v<T, Color4f> ||
                         std::is_same_v<T, Color4u>) {
        channels = 4;
    }
    std::ifstream file(filename);

    if (!file.is_open())
        throw detail::CommonBitmapException("Could not open file " +
                                            std::string(filename));

    // Write here all the loaders
    if (test_png<T>(file)) return load_png<T>(file, channels);

    throw detail::CommonBitmapException("No image loader found for file " +
                                        std::string(filename));
}

};  // namespace common