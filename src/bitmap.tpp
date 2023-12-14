/*
 * bitmap.tpp
 * Diego Royo Meneses - Dec. 2023
 *
 * Image data structure with read/write operations
 */

namespace common {

template <typename T>
Grid2D<T> bitmap_load(const char* filename) {
    static_assert(std::is_same<T, float>::value ||  //
                      std::is_same<T, Color3f>::value ||
                      std::is_same<T, Color4f>::value ||
                      std::is_same<T, unsigned int>::value ||
                      std::is_same<T, Color3u>::value ||
                      std::is_same<T, Color4u>::value,
                  "Invalid bitmap type to load. It must be one of: float, "
                  "unsigned int, or a Color.");
    std::ifstream file(filename);

    if (!file.is_open())
        throw CommonBitmapException("Could not open file " +
                                    std::string(filename));

    // Write here all the loaders
    if (test_png<T>(file)) return load_png<T>(file);

    throw CommonBitmapException("No image loader found for file " +
                                std::string(filename));
}

};  // namespace common