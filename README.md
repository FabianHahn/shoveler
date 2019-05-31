[![Build Status](https://api.travis-ci.org/FabianHahn/shoveler.svg)](https://travis-ci.org/FabianHahn/shoveler)

# shoveler

## Introduction
**shoveler** is a a _minimalistic_ OpenGL game engine. It is minimalistic in that it is completely written in C and thus only uses the most basic of programming language concepts, making the effect of every line of code as obvious as possible without any implicit "magic" happening in the background. It is minimalistic in the sense that I wrote it from scratch with minimal dependencies, all of which are included as source, which means that a game can compile into a single statically linked executable that you can distribute and execute anywhere. It is minimalistic in the sense that while it does offer high-level abstractions, they are all built on lower-level ones that you might as well use directly if you so desire.

## Dependencies
**shoveler** requires [CMake](https://cmake.org/) to build and bundles the following [third-party dependencies](thirdparty) as source:
 * [fakeglib](https://github.com/FabianHahn/fakeglib) - used as "standard library" for data structures
 * [glad](https://github.com/Dav1dde/glad) - used to load OpenGL symbols
 * [glfw](http://www.glfw.org/) - used manage the OpenGL context and input handling
 * [libpng](https://github.com/glennrp/libpng) - used to load and write texture image files 
 * [zlib](https://github.com/madler/zlib) - used to compress and uncompress data

### Optional Dependencies
 * [glib](https://developer.gnome.org/glib/stable/) can be used instead of *fakeglib* by setting the `shoveler_use_glib` CMake option to *true*. Requires *glib* to be available on your system (on Linux, install `libglib2.0-dev` or similar; on Windows, get a [GTK+ bundle](https://github.com/hexchat/gtk-win32) and add its `bin` directory to your `PATH`).

## Usage

**shoveler** was tested to compile out of the box on Linux (tested with _gcc_ and _clang_) and Windows (tested with _Visual Studio 2015_). It might work on other platforms, but since I do not have access to them I am unable to support them and guarantee that they work.

### Linux

Make sure you have headers for the [X Window System](http://www.opengroup.org/tech/desktop/x-window-system/) installed that will be used by [glfw](http://www.glfw.org/) to create a window to render to. If you are using a Linux distribution based on the [APT](https://wiki.debian.org/Apt) package manager such as _Debian_ or _Ubuntu_, you can install them with the following command:
```
sudo apt-get install xorg-dev libxi-dev
```

Clone the repository and switch into it:
```
git clone https://github.com/FabianHahn/shoveler
cd shoveler
```

Create a build directory and run CMake:
```
mkdir build
cd build
cmake ..
```

Build shoveler and its dependencies:
```
make
```

Congratulations! You should now be able to run the example executables provided:
```
cd examples
./lights
```

### Windows

Open a command prompt by pressing `Windows + R` and running `cmd.exe`. Clone the repository and switch into it:
```
git clone https://github.com/FabianHahn/shoveler
cd shoveler
```

Create a build directory and run CMake with the correct flag for the compiler you would like to use. For _Visual Studio 2015_, use the following:
```
mkdir build
cd build
cmake -G "Visual Studio 14 2015 Win64" ..
```

This will create a `shoveler.sln` solution file you can open in _Visual Studio_ and build by pressing F7. Alternatively, you can also compile it from the command line:
```
cmake --build . --config release
```

Congratulations! You should now be able to run the example executables provided:
```
cd examples
cd Release
lights.exe
```
