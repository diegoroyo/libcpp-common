#include "libcpp-common/bitmap.h"

int main() {
    common::Bitmap4f image =
        common::bitmap_load<Color4f>("/home/diego/desarrollo/cpp/grid.png");

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
    return 0;
}
