#include "libcpp-common/bitmap.h"

int main() {
    common::Bitmap3f a =
        common::bitmap_load<Color3f>("/home/diego/Desktop/test.png");
    return 0;
}
