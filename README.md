# `libcpp-common`

A collection of C++ files I typically use in my projects

* `geometry.h`: Implementation of `Vec`, `VecList`, and `Mat` types.
* `mesh.h`: 3D model loader. Currently supports:
  * PLY format (only the vertices and the faces).
* `bitmap.h`: Image loader and saver with the `Color` (i.e. RGB), `Bitmap` (i.e. image) and `BitmapList` (i.e. video) types. Currently supports:
  * Loading:
    * PNG format (only 8-bit grayscale/RGB/RGBA non-interlaced and without DEFLATE compression).
    * PPM format (only RGB images i.e. `common::Bitmap3f` or `common::Bitmap3u` up to 32-bit precision)
  * Saving:
    * PPM format (only RGB images i.e. `common::Bitmap3f` or `common::Bitmap3u` with 8-bit precision)
* `test.h`: Simple test framework. See `tests` folder for some examples.
* `log.h`: Simple logging utility.
