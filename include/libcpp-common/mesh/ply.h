/*
 * ply.h
 * Diego Royo Meneses - Dec. 2023
 *
 * Polygon File Format loader
 */

namespace common {

struct Mesh;
bool test_ply(std::ifstream& file);
Mesh load_ply(std::ifstream& file);

};