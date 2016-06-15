# shoveler

A minimalistic OpenGL game engine written in C and built using the following libraries:
 * [glib](https://developer.gnome.org/glib/stable/) - used as "standard library" (on Linux, install `libglib2.0-dev` or similar; on Windows, get a [GTK+ bundle](https://github.com/hexchat/gtk-win32) and add its `bin` directory to your `PATH`)
 * [glfw](http://www.glfw.org/) - used manage the OpenGL context and input handling (provided in `thirdparty/`)
 * [glad](https://github.com/Dav1dde/glad) - used to load OpenGL symbols (provided in `thirdparty/`)
