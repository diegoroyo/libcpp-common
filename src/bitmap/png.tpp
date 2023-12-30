/*
 * png.tpp
 * Diego Royo Meneses - Dec. 2023
 *
 * Portable Network Graphics loader
 */
#include <cstring>
#include <fstream>
#include <vector>

#include "libcpp-common/detail/exception.h"

namespace common {

template <typename T>
struct bitmap_channels;

/// PNG header and chunk metadata ///

static inline const size_t HEADER_SIZE = 8;
static inline const uint8_t HEADER[] = {0x89, 0x50, 0x4E, 0x47,
                                        0x0D, 0x0A, 0x1A, 0x0A};
static inline const size_t CHUNK_TYPE_SIZE = 4;

template <typename T>
bool test_png(std::ifstream& file) {
    char read_header[HEADER_SIZE];
    file.read(read_header, HEADER_SIZE);

    bool header_ok = true;
    for (size_t i = 0; i < HEADER_SIZE; ++i) {
        header_ok = header_ok && HEADER[i] == (uint8_t)read_header[i];
    }

    file.clear();
    file.seekg(0, std::ios::beg);

    return header_ok;
}

/// Chunk CRC computation ///

// Adapted from https://datatracker.ietf.org/doc/html/rfc2083#section-15
static inline bool CRC_TABLE_COMPUTED = false;
static inline uint32_t CRC_TABLE[256];

static inline void make_crc_table() {
    for (uint32_t n = 0; n < 256; ++n) {
        uint32_t c = n;
        for (int k = 0; k < 8; ++k) {
            if (c & 1)
                c = 0xEDB88320 ^ (c >> 1);
            else
                c = c >> 1;
        }
        CRC_TABLE[n] = c;
    }
    CRC_TABLE_COMPUTED = true;
}

static inline uint32_t compute_crc(uint8_t* buf, size_t len) {
    if (!CRC_TABLE_COMPUTED) make_crc_table();
    uint32_t crc = 0xFFFFFFFF;
    for (size_t n = 0; n < len; ++n) {
        crc = CRC_TABLE[(crc ^ buf[n]) & 0xFF] ^ (crc >> 8);
    }
    return crc ^ 0xFFFFFFFF;
}

static inline uint32_t read_big_endian(const uint8_t* read) {
    return read[3] | read[2] << 8 | read[1] << 16 | read[0] << 24;
}

static inline uint32_t read_little_endian(const uint8_t* read) {
    return read[0] | read[1] << 8 | read[2] << 16 | read[3] << 24;
}

/// Read and store chunks (without processing them) ///

struct PNGChunk {
    uint32_t length, crc;
    char type[CHUNK_TYPE_SIZE + 1];
    std::vector<uint8_t> data_raw;
    std::vector<uint8_t>::const_iterator data_begin() const {
        return data_raw.cbegin() + CHUNK_TYPE_SIZE;
    }
    std::vector<uint8_t>::const_iterator data_end() const {
        return data_raw.cend();
    }
};

struct IHDR {
    uint32_t width, height;
    uint8_t bit_depth, color_type, compression_method, filter_method,
        interlace_method;
};

static inline PNGChunk read_png_chunk(std::ifstream& file) {
    uint8_t buffer[4];
    PNGChunk chunk;
    file.read((char*)buffer, 4);
    chunk.length = read_big_endian(buffer);

    chunk.data_raw.resize(chunk.length + CHUNK_TYPE_SIZE);
    file.read((char*)chunk.data_raw.data(), chunk.length + CHUNK_TYPE_SIZE);

    memcpy(chunk.type, chunk.data_raw.data(), CHUNK_TYPE_SIZE);
    chunk.type[CHUNK_TYPE_SIZE] = '\0';

    file.read((char*)buffer, 4);
    chunk.crc = read_big_endian(buffer);
    uint32_t computed_crc =
        compute_crc(chunk.data_raw.data(), chunk.length + CHUNK_TYPE_SIZE);
    if (chunk.crc != computed_crc)
        throw detail::CommonBitmapException(
            "PNG Unexpected error: " + std::string(chunk.type) +
            " chunk's CRC is incorrect.");

    return chunk;
}

/// Apply modifications to resulting image based on chunk type ///

template <typename T>
IHDR apply_ihdr(Grid2D<T>& image, const PNGChunk& chunk,
                const uint8_t channels) {
    IHDR ihdr;
    uint8_t const* data = &(*(chunk.data_begin()));

    ihdr.width = read_big_endian(data);
    ihdr.height = read_big_endian(data + 4);
    data += 8;
    image.resize(ihdr.width, ihdr.height);

    ihdr.bit_depth = *(data++);
    ihdr.color_type = *(data++);
    ihdr.compression_method = *(data++);
    ihdr.filter_method = *(data++);
    ihdr.interlace_method = *(data++);

    if (ihdr.bit_depth != 8)
        throw detail::CommonBitmapException("PNG IHDR: Unsupported bit depth " +
                                            std::to_string(ihdr.bit_depth));

    uint8_t expected_channels;
    switch (ihdr.color_type) {
        case 0:
            // Each pixel is a grayscale sample
            expected_channels = 1;
            break;
        case 2:
            // Each pixel is an R,G,B triple
            expected_channels = 3;
            break;
        case 6:
            // Each pixel is an R,G,B triple, followed by an alpha sample.
            expected_channels = 4;
            break;
        case 3:  // PLTE
        case 4:  // Grayscale + alpha
            throw detail::CommonBitmapException(
                "PNG IHDR: Unsupported color type " +
                std::to_string(ihdr.bit_depth));
    }
    if (channels != expected_channels)
        throw detail::CommonBitmapException(
            "PNG IHDR: You are loading a PNG with " + std::to_string(channels) +
            " channels, but the file actually has " +
            std::to_string(expected_channels) + " channels");

    if (ihdr.compression_method != 0)  // deflate/inflate compression
        throw detail::CommonBitmapException(
            "PNG IHDR: Unsupported compression method " +
            std::to_string(ihdr.compression_method));

    if (ihdr.filter_method != 0)  // adaptive filtering with 5 different types
        throw detail::CommonBitmapException(  //
            "PNG IHDR: Unsupported filter method " +
            std::to_string(ihdr.filter_method));

    if (ihdr.interlace_method == 1)
        throw detail::CommonBitmapException(
            "PNG IHDR: Unsupported Adam7 interlace method");
    if (ihdr.interlace_method != 0 && ihdr.interlace_method != 1)
        throw detail::CommonBitmapException(
            "PNG IHDR: Unsupported interlace method " +
            std::to_string(ihdr.interlace_method));

    return ihdr;
}

static inline const size_t IDAT_ADLER_SIZE = 4;

static inline uint32_t adler32_checksum(const uint8_t* buf, size_t len) {
    const uint32_t BASE = 65521;  // largest prime number smaller than 65536
    int32_t s1 = 1;
    int32_t s2 = 0;
    for (size_t n = 0; n < len; ++n) {
        s1 = (s1 + buf[n]) % BASE;
        s2 = (s2 + s1) % BASE;
    }
    return (s2 << 16) + s1;
}

static inline uint8_t apply_png_filter(const size_t x, const size_t y,
                                       const size_t c, const uint8_t channels,
                                       const uint8_t filter_type,
                                       const uint8_t* previous_row,
                                       const uint8_t* current_row
                                       /* const PLTE* plte = nullptr */) {
    size_t bpp = channels;   // num bytes for a complete pixel
    size_t i = x * bpp + c;  // current byte in row
    uint8_t left = x == 0 ? 0 : current_row[i - bpp];
    uint8_t top = y == 0 ? 0 : previous_row[i];
    uint8_t topleft = (x == 0 || y == 0) ? 0 : previous_row[i - bpp];
    switch (filter_type) {
        default:
            throw detail::CommonBitmapException(
                "PNG IDAT: Unsupported filter algorithm " +
                std::to_string(filter_type));
        case 0:  // none
            return 0;
        case 1:  // sub
            return left;
        case 2:  // up
            return top;
        case 3:  // average
            return (left + top) / 2;
        case 4:  // paeth
            // https://datatracker.ietf.org/doc/html/rfc2083#section-6.6
            uint8_t a = left, b = top, c = topleft;
            uint8_t p = a + b - c;
            uint8_t pa = abs(p - a);
            uint8_t pb = abs(p - b);
            uint8_t pc = abs(p - c);
            // return nearest of a, b, c breaking ties in order a, b, c
            uint8_t paeth = pa <= pb && pa <= pc ? a : (pb <= pc ? b : c);
            return paeth;
    }
}

template <typename T>
void apply_deflate(Grid2D<T>& image, const IHDR& ihdr,
                   std::vector<uint8_t>::iterator deflate,
                   const size_t deflate_size, const uint8_t channels,
                   uint32_t read_adler32
                   /* const PLTE* plte = nullptr */) {
    uint8_t block_format = *(deflate++);
    // first bit: set iif this is the last block of the set
    uint8_t bfinal = block_format & 0b001;
    // next 2 bits: specifies how data are compressed
    uint8_t btype = (block_format & 0b110) >> 1;

    if (btype != 0)  // only support "no compression"
        throw detail::CommonBitmapException(
            "PNG IDAT ZLIB DEFLATE: Unsupported compression type " +
            std::to_string(btype));
    // note: if btype == 0, you must skip the rest 5 bits of block_format
    // otherwise you would have to start reading compressed data from bit 3

    uint32_t len_nlen = read_little_endian(&*deflate);
    deflate += 4;

    uint32_t len = len_nlen & 0xFFFF;
    if (len - (len_nlen >> 16 ^ 0xFFFF) != 0)
        throw detail::CommonBitmapException(
            "PNG IDAT ZLIB DEFLATE: Invalid LEN-NLEN ones-complement pair");
    uint32_t expected_len = image.height() * (1 + image.width() * channels);
    if (len != expected_len)
        throw detail::CommonBitmapException(
            "PNG IDAT ZLIB DEFLATE: Unexpected data length with respect to "
            "other image headers");

    uint32_t computed_adler32 = adler32_checksum(&*deflate, len);
    if (read_adler32 != computed_adler32)
        throw detail::CommonBitmapException(
            "PNG IDAT ZLIB: Incorrect Adler-32 checksum");

    // finally, apply to image
    uint8_t const* previous_row = nullptr;
    uint8_t const* current_row;
    for (size_t y = 0; y < image.height(); ++y) {
        uint8_t filter_type = *(deflate++);
        current_row = &*deflate;
        for (size_t x = 0; x < image.width(); ++x) {
            for (uint8_t c = 0; c < channels; ++c) {
                uint8_t dv = apply_png_filter(x, y, c, channels, filter_type,
                                              previous_row, current_row);
                uint8_t v = *deflate + dv;
                *(deflate++) = v;
                if (channels == 1)
                    image(x, y) = v;
                else
                    image(x, y)[c] = v;
            }
        }
        previous_row = current_row;
    }
}

template <typename T>
void apply_idat(Grid2D<T>& image, const IHDR& ihdr, std::vector<uint8_t>& idat,
                const uint8_t channels
                /* const PLTE* plte = nullptr */) {
    // The concatenation of all IDAT chunks is a ZLIB datastream
    // https://datatracker.ietf.org/doc/html/rfc1950
    // Inside the ZLIB datastream, the header says which compression method
    // it's used. It's most probably DEFLATE compressed data
    // https://www.rfc-editor.org/rfc/rfc1951
    auto data = idat.begin();
    uint8_t cmf = *(data++);  // compression method and flags
    uint8_t flg = *(data++);  // flags

    uint8_t cm = cmf & 0x0F;   // bits 0 to 3: compression method
    uint8_t cinfo = cmf >> 4;  // bits 4 to 7: compression info

    if (cm != 8)
        throw detail::CommonBitmapException(
            "PNG IDAT ZLIB: Unsupported compression method (CM) " +
            std::to_string(cm));
    if (cinfo > 7)
        throw detail::CommonBitmapException(
            "PNG IDAT ZLIB: Unsupported compression info (CINFO) " +
            std::to_string(cinfo));

    // note: no compression is implemented, so this is unused for now
    // size_t lz_window = 1 >> (cinfo + 8);

    // uint8_t fcheck = flg & 0x1F;  // bits 0 to 4: check bits for CMF and FLG
    uint8_t fdict = flg & 0x20;  // bit 5: preset dictionary
    uint8_t flevel = flg >> 6;   // bits 6 to 7: compression level

    uint16_t check = ((uint16_t)cmf << 8) + (uint16_t)flg;
    if (check % 31 != 0)
        throw detail::CommonBitmapException(
            "PNG IDAT ZLIB: Check bits for CMF and FLG (FCHECK) should be a "
            "multiple of 31");

    if (fdict != 0)
        throw detail::CommonBitmapException(
            "PNG IDAT ZLIB: Unsupported DICT dictionary on compressed data");
    // The information in FLEVEL is not needed for decompression; it
    // is there to indicate if recompression might be worthwhile.
    // if (flevel != 0)  // only allow level 0: fastest algorithm
    //     throw detail::CommonBitmapException(
    //         "PNG IDAT: Unsupported compression level " +
    //         std::to_string(flevel));

    size_t zlib_header_size = data - idat.begin();
    auto deflate = data;
    size_t deflate_size = idat.size() - zlib_header_size - IDAT_ADLER_SIZE;

    uint32_t read_adler32 = read_big_endian(&*(data + deflate_size));
    apply_deflate(image, ihdr, deflate, deflate_size, channels, read_adler32);
}

/// Main read function ///

inline bool is_uppercase(const char c) { return c >= 'A' && c <= 'Z'; }

template <typename T>
Grid2D<T> load_png(std::ifstream& file) {
    constexpr uint8_t channels = bitmap_channels<T>::value;
    Grid2D<T> image;

    // Skip magic number header
    file.seekg(8, std::ios::beg);

    // Read all chunks. This should initialize the image.
    PNGChunk chunk;
    IHDR ihdr;
    std::vector<uint8_t> idat;
    do {
        chunk = read_png_chunk(file);
        if (strcmp(chunk.type, "IHDR") == 0) {
            ihdr = apply_ihdr(image, chunk, channels);
        } else if (strcmp(chunk.type, "IDAT") == 0) {
            // Accumulates all IDAT data into a single vector
            idat.insert(idat.end(), chunk.data_begin(), chunk.data_end());
        } else if (strcmp(chunk.type, "IEND") == 0) {
            if (file.eof())
                throw detail::CommonBitmapException(
                    "PNG Unexpected error: EOF while reading IEND chunk?");
            file.get();
            if (!file.eof())
                throw detail::CommonBitmapException(
                    "PNG Unexpected error: PNG file ends without IEND chunk?");
        } else if (is_uppercase(chunk.type[0])) {
            throw detail::CommonBitmapException(
                "PNG Unsupported critical chunk type " +
                std::string(chunk.type));
        }
    } while (file.good());

    // Read the image data into the image
    apply_idat(image, ihdr, idat, channels);

    return image;
}
};  // namespace common