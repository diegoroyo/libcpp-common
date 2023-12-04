/*
 * ply.cpp
 * Diego Royo Meneses - Dec. 2023
 *
 * Polygon File Format loader
 */

#include <algorithm>
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <tuple>

#include "libcpp-common/mesh.h"

namespace common {

bool test_ply(std::ifstream& file) {
    std::string ply;
    std::getline(file, ply);

    bool is_ply = ply == "ply";
    file.clear();
    file.seekg(0, std::ios::beg);

    return is_ply;
}

template <typename T>
uint64_t read_ascii(std::istream& line) {
    T res;
    line >> res;
    return *((uint64_t*)&res);
}

template <typename T>
uint64_t read_little_endian(std::istream& line) {
    size_t n = sizeof(T);
    uint64_t res = 0;
    for (int i = 0; i < n; ++i) {
        uint8_t byte;
        line.read(reinterpret_cast<char*>(&byte), sizeof(byte));
        res |= byte << (i * 8);
    }
    return res;
}

template <typename T>
uint64_t read_big_endian(std::istream& line) {
    size_t n = sizeof(T);
    uint64_t res = 0;
    for (int i = 0; i < n; ++i) {
        uint8_t byte;
        line.read(reinterpret_cast<char*>(&byte), sizeof(byte));
        res |= byte << ((n - i - 1) * 8);
    }
    return res;
}

class Property {
   public:
    Property(const std::string& type) : m_type(type) {}
    virtual ~Property() = default;
    virtual void write(Mesh& mesh, const size_t idx, uint64_t value) const {};
    std::string type() const { return m_type; }
    virtual bool is_list() const { return false; }

   private:
    std::string m_type;
};

class VertexXProperty : public Property {
   public:
    VertexXProperty(const std::string& type) : Property(type) {}
    void write(Mesh& mesh, const size_t idx, uint64_t value) const override {
        mesh.vertices[idx].x() = *((float*)&value);
    }
};

class VertexYProperty : public Property {
   public:
    VertexYProperty(const std::string& type) : Property(type) {}
    void write(Mesh& mesh, const size_t idx, uint64_t value) const override {
        mesh.vertices[idx].y() = *((float*)&value);
    }
};

class VertexZProperty : public Property {
   public:
    VertexZProperty(const std::string& type) : Property(type) {}
    void write(Mesh& mesh, const size_t idx, uint64_t value) const override {
        mesh.vertices[idx].z() = *((float*)&value);
    }
};

class IgnoreProperty : public Property {
   public:
    IgnoreProperty(const std::string& type) : Property(type) {}
};

class ListProperty : public Property {
   public:
    ListProperty(const std::string& length_type, const std::string& type)
        : m_length_type(length_type), Property(type) {}
    std::string length_type() const { return m_length_type; }
    bool is_list() const override { return true; }
    void write(Mesh& mesh, const size_t idx, uint64_t value) const override{};

   protected:
    std::string m_length_type;
};

class FaceTriangleProperty : public ListProperty {
   public:
    FaceTriangleProperty(const std::string& length_type,
                         const std::string& type)
        : ListProperty(length_type, type) {}

    void write_face(Mesh& mesh, const size_t idx, Vec3u face) const {
        mesh.faces[idx] = face;
    }
};

Mesh load_ply(std::ifstream& file) {
    std::string ply, format;
    std::getline(file, ply);
    std::getline(file, format);

    bool is_ascii = false;
    bool is_binary_little_endian = false;
    bool is_binary_big_endian = false;
    if (ply != "ply")
        throw CommonMeshException("Unexpected error: file is not a ply file?");
    if (format == "format ascii 1.0") {
        is_ascii = true;
    } else if (format == "format binary_little_endian 1.0") {
        is_binary_little_endian = true;
    } else if (format == "format binary_big_endian 1.0") {
        is_binary_big_endian = true;
    } else {
        throw CommonMeshException("Unsupported ply format: " + format);
    }

    Mesh mesh;
    bool end_header = false;
    std::vector<std::tuple<size_t, std::vector<std::shared_ptr<Property>>>>
        all_properties;

    std::string line;
    std::getline(file, line);
    while (!file.eof()) {
        if (line == "end_header") {
            end_header = true;
            break;
        } else if (line.substr(0, 7) == "comment") {
            // Ignore the line and read next one
            std::getline(file, line);
        } else if (line.substr(0, 7) == "element") {
            size_t num_elements;
            if (line.substr(8, 6) == "vertex") {
                num_elements = std::stoi(line.substr(15));
                mesh.vertices.resize(num_elements);
                std::fill(mesh.vertices.begin(), mesh.vertices.end(),
                          Vec4f(0, 0, 0, 1));
            } else if (line.substr(8, 4) == "face") {
                num_elements = std::stoi(line.substr(13));
                mesh.faces.resize(num_elements);
                std::fill(mesh.faces.begin(), mesh.faces.end(), Vec3u(0, 0, 0));
            }

            std::vector<std::shared_ptr<Property>> properties;

            std::getline(file, line);
            while (!file.eof() && line.substr(0, 8) == "property") {
                std::string type, name;
                std::stringstream ss(line.substr(9));
                ss >> type >> name;
                if (type == "float") {
                    if (name == "x")
                        properties.push_back(
                            std::make_unique<VertexXProperty>(type));
                    else if (name == "y")
                        properties.push_back(
                            std::make_unique<VertexYProperty>(type));
                    else if (name == "z")
                        properties.push_back(
                            std::make_unique<VertexZProperty>(type));
                    else
                        properties.push_back(
                            std::make_unique<IgnoreProperty>(type));
                } else if (type == "list") {
                    std::string length_type, list_type;
                    length_type = name;
                    ss >> list_type >> name;
                    if (length_type == "uchar" &&
                        (list_type == "int" or list_type == "uint") &&
                        name == "vertex_indices") {
                        properties.push_back(
                            std::make_unique<FaceTriangleProperty>(length_type,
                                                                   list_type));
                    }
                }
                std::getline(file, line);
            }
            all_properties.push_back(std::make_tuple(num_elements, properties));
        }
    }

    auto read_value_ascii = [](const std::string& type,
                               std::istream& is) -> uint64_t {
        if (type == "float") {
            return read_ascii<float>(is);
        } else if (type == "uint" or type == "int" or type == "uchar" or
                   type == "char") {
            return read_ascii<int>(is);
        }
        throw CommonMeshException("Unsupported type: " + type);
    };

    auto read_value_binary = [&is_binary_little_endian, &is_binary_big_endian](
                                 const std::string& type,
                                 std::istream& is) -> uint64_t {
        if (type == "float" or type == "uint" or type == "int") {
            if (is_binary_little_endian) {
                return read_little_endian<uint32_t>(is);
            } else if (is_binary_big_endian) {
                return read_big_endian<uint32_t>(is);
            }
        } else if (type == "uchar" or type == "char") {
            if (is_binary_little_endian) {
                return read_little_endian<uint8_t>(is);
            } else if (is_binary_big_endian) {
                return read_big_endian<uint8_t>(is);
            }
        }
        throw CommonMeshException("Unsupported type: " + type);
    };

    auto read_value = [&is_ascii, &read_value_ascii, &read_value_binary](
                          const std::string& type, std::ifstream& file,
                          std::stringstream& ss) -> uint64_t {
        if (is_ascii) {
            return read_value_ascii(type, ss);
        } else {
            return read_value_binary(type, file);
        }
    };

    std::stringstream ss;
    for (const auto& element : all_properties) {
        const size_t num_elements = std::get<0>(element);
        const std::vector<std::shared_ptr<Property>>& properties =
            std::get<1>(element);
        size_t idx = 0;
        while (idx < num_elements) {
            if (is_ascii) {
                std::getline(file, line);
                ss = std::stringstream(line);
            }
            for (const auto& property : properties) {
                if (!property->is_list()) {
                    uint64_t value = read_value(property->type(), file, ss);
                    property->write(mesh, idx, value);
                    continue;
                }

                const auto fproperty =
                    std::dynamic_pointer_cast<FaceTriangleProperty>(property);
                if (fproperty) {
                    uint8_t length =
                        read_value(fproperty->length_type(), file, ss);
                    if (length != 3)
                        throw CommonMeshException("Unexpected face length: " +
                                                  std::to_string(length));

                    Vec3u face;
                    for (size_t i = 0; i < length; ++i) {
                        face[i] = read_value(fproperty->type(), file, ss);
                    }
                    fproperty->write_face(mesh, idx, face);
                }
            }
            ++idx;
        }
    }

    return mesh;
}
};  // namespace common
