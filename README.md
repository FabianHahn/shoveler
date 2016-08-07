# shoveler

A minimalistic OpenGL game engine written in C.

*shoveler* builds with [CMake](https://cmake.org/), and compiles out of the box on Linux (tested with *gcc*) and Windows (tested with *Visual Studio 2015*). In order to be completely self-contained, it comes with the following [third-party dependencies](thirdparty) bundled as source:
 * [fakeglib](https://github.com/FabianHahn/fakeglib) - used as "standard library" for data structures 
 * [glad](https://github.com/Dav1dde/glad) - used to load OpenGL symbols
 * [glfw](http://www.glfw.org/) - used manage the OpenGL context and input handling
 * [libpng](https://github.com/glennrp/libpng) - used to load and write texture image files 
 * [zlib](https://github.com/madler/zlib) - used to compress and uncompress images for *libpng*

## Optional Dependencies
 * [glib](https://developer.gnome.org/glib/stable/) can be used instead of *fakeglib* by setting the `shoveler_use_glib` CMake option to *true*. Requires *glib* to be available on your system (on Linux, install `libglib2.0-dev` or similar; on Windows, get a [GTK+ bundle](https://github.com/hexchat/gtk-win32) and add its `bin` directory to your `PATH`).
