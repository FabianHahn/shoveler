[![Build Status](https://app.travis-ci.com/FabianHahn/fakeglib.svg?branch=master)](https://app.travis-ci.com/github/FabianHahn/fakeglib)

# fakeglib
A partial implementation of [glib](https://github.com/GNOME/glib)'s [C API](https://docs.gtk.org/glib/) in C++.

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
*fakeglib* currently implements the following parts of [glib](https://github.com/GNOME/glib)'s [C API](https://docs.gtk.org/glib/):
* [Array](https://docs.gtk.org/glib/struct.Array.html) ([partial](lib/include/fakeglib/GArray.h), no sorting and binary search functions)
* Basic Types ([minimal](lib/include/fakeglib/GTypes.h))
* Memory Allocation ([minimal](lib/include/fakeglib/GMemory.h))
* [GDateTime](https://docs.gtk.org/glib/struct.DateTime.html) ([minimal](lib/include/fakeglib//GDateTime.h))
* Date and Time Functions ([minimal](lib/include/fakeglib/GTime.h))
* [Doubly-Linked Lists](https://docs.gtk.org/glib/struct.List.html) ([full](lib/include/fakeglib/GList.h))
* [Double-Ended Queues](https://docs.gtk.org/glib/struct.Queue.html) ([full](lib/include/fakeglib/GQueue.h))
* [Hash Tables](https://docs.gtk.org/glib/struct.HashTable.html) ([full](lib/include/fakeglib/GHashTable.h))
* [Strings](https://docs.gtk.org/glib/struct.String.html) ([partial](lib/include/fakeglib/GString.h), no ASCII/Unichar functions)
* String Utility Functions ([partial](lib/include/fakeglib/GStringUtil.h))
