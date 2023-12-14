/*
 * png.tpp
 * Diego Royo Meneses - Dec. 2023
 *
 * Portable Network Graphics loader
 */

namespace common {

static const size_t HEADER_SIZE = 8;
static const char HEADER[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};

template <typename T>
bool test_png(std::ifstream &file) {
    char read_header[HEADER_SIZE];
    file.read(read_header, HEADER_SIZE);

    bool header_ok = true;
    for (size_t i = 0; i < HEADER_SIZE; ++i) {
        header_ok = header_ok && HEADER[i] == read_header[i];
    }

    file.clear();
    file.seekg(0, std::ios::beg);

    return header_ok;
}

uint32_t read_big_endian(std::istream &line) {
    uint32_t res = 0;
    for (int i = 0; i < 4; ++i) {
        uint8_t byte;
        line.read(reinterpret_cast<char *>(&byte), sizeof(byte));
        res |= byte << (i * 8);
    }
    return res;
}

template <typename T>
Grid2D<T> load_png(std::ifstream &file) {
    constexpr const channels = sizeof(T);

    // TODO skip 8 first bytes

    // TODO read first chunk IHDR (move "read chunk" to a separate function)

    // TODO read other chunks until IEND and ensure that the file ended

    // TODO implement
    Grid2D<T> result(10, 10);
    return result;
}

};  // namespace common