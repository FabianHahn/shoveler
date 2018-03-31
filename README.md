[![Build Status](https://api.travis-ci.org/FabianHahn/fakeglib.svg)](https://travis-ci.org/FabianHahn/fakeglib)

# fakeglib
A partial implementation of [glib](https://github.com/GNOME/glib)'s [C API](https://developer.gnome.org/glib/2.48/) in C++.

## Why?
C is a great programming language, but generally lacks a standard library of common dynamic data structures such as strings, lists, or maps with a native C API.
The [GNOME project](https://www.gnome.org/)'s [glib](https://github.com/GNOME/glib) is a notable exception to this rule, and is pretty fantastic for developing C on Linux.
However, it comes with its own dependencies and uses automake as build tool, making it annoying (though [not impossible](https://github.com/hexchat/gtk-win32)) to build and link on non-UNIX platforms such as Windows.
This projects aims to alleviate this issue by providing a self-contained partial implementation of [glib](https://github.com/GNOME/glib)'s [C API](https://developer.gnome.org/glib/2.48/) implemented in C++ - thus the name *fakeglib*.

The requirements for *fakeglib* are:
* Self-contained: Must not depend on any thirdparty code.
* Cross-platform: Must build on at least Linux and Windows.
* Native development: Uses [CMake](https://cmake.org/) to generate native build files for each supported platform.

## What?
*fakeglib* currently implements the following parts of [glib](https://github.com/GNOME/glib)'s [C API](https://developer.gnome.org/glib/2.48/):
* [Basic Types](https://developer.gnome.org/glib/2.48/glib-Basic-Types.html) ([minimal](lib/include/fakeglib/GTypes.h))
* [Memory Allocation](https://developer.gnome.org/glib/2.48/glib-Memory-Allocation.html) ([minimal](lib/include/fakeglib/GMemory.h))
* [GDateTime](https://developer.gnome.org/glib/2.48/glib-GDateTime.html) ([minimal](lib/include/fakeglib//GDateTime.h))
* [Date and Time Functions](https://developer.gnome.org/glib/2.48/glib-Date-and-Time-Functions.html) ([minimal](lib/include/fakeglib/GTime.h))
* [Doubly-Linked Lists](https://developer.gnome.org/glib/2.48/glib-Doubly-Linked-Lists.html) ([full](lib/include/fakeglib//GList.h))
* [Double-Ended Queues](https://developer.gnome.org/glib/2.48/glib-Double-ended-Queues.html) ([full](lib/include/fakeglib/GQueue.h))
* [Hash Tables](https://developer.gnome.org/glib/2.48/glib-Hash-Tables.html) ([full](lib/include/fakeglib/GHashTable.h))
* [Strings](https://developer.gnome.org/glib/2.48/glib-Strings.html) ([partial](lib/include/fakeglib/GString.h))
