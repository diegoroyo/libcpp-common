#include "libcpp-common/bitmap.h"

#include <iostream>

int main() {
    common::Bitmap4f image =
        common::load_bitmap<Color4f>("/home/diego/cpp/grid.png");

    for (size_t y = 0; y < image.height(); ++y) {
        for (size_t x = 0; x < image.width(); ++x) {
            std::cout << Vec2i(x, y) << ": " << image(x, y) << std::endl;
        }
    }

    std::cout << "---" << std::endl;
    std::cout << Vec2i(-1, 0) << ": " << image(-1, 0) << std::endl;
    std::cout << Vec2i(0, -1) << ": " << image(0, -1) << std::endl;
    std::cout << Vec2i(0, -2) << ": " << image(0, -2) << std::endl;
    std::cout << Vec2i(0, 5) << ": " << image(0, 5) << std::endl;
    std::cout << Vec2i(0, -3) << ": " << image(0, -3) << std::endl;

    common::Bitmap3f rgb = image.map<Color3f>([](const Color4f& in) -> Color3f {
        return Color3f(in.r(), in.g(), in.b());
    });

    std::cout << "---" << std::endl;
    for (size_t y = 0; y < rgb.height(); ++y) {
        for (size_t x = 0; x < rgb.width(); ++x) {
            std::cout << Vec2i(x, y) << ": " << rgb(x, y) << std::endl;
        }
    }

    uint count_nonzero =
        rgb.reduce<uint>(0, [](uint v, const Color3f& e) -> uint {
            return e.max() == 0 ? (v + 1) : v;
        });
    std::cout << "Nonzero elements: " << std::to_string(count_nonzero)
              << std::endl;

    common::save_bitmap("libcommon.npy", image);

    return 0;
}
