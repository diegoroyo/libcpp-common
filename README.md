# `libcpp-common`

A collection of C++ files I typically use in my projects

* `geometry.h`: Implementation of `Vec`, `VecList`, and `Mat` types.
* `mesh.h`: 3D model loader. Currently supports:
  * PLY format (only the vertices and the faces).
* `bitmap.h`: Image loader and saver (using `Color` and `Bitmap` types). Currently supports:
  * Loading:
    * PNG format (only 8-bit grayscale/RGB/RGBA non-interlaced and without DEFLATE compression).
  * Saving:
    * PPM format (only RGB images i.e. `common::Bitmap3f` or `common::Bitmap3u`)
