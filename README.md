[![Build Status](https://app.travis-ci.com/FabianHahn/shoveler.svg?branch=master)](https://app.travis-ci.com/github/FabianHahn/shoveler)

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

## Architecture

### Entity component system

Shoveler comes with its own entity component system that consists of the following main parts:
 - **Component types** define a number of **component fields** with primitive types such as string, integer or boolean. Each component type has a unique **component type ID**. A **schema** is a collection of such component types, keyed by component type IDs.
 - A **component** is an instance of a specific component type with concrete values set for all of its component fields.
 - A **world** is a collections of **entities** that can each contain any number of components from the world's schema. Entities have an **entity ID** that identifies them and that they are keyed by in the world.
 - A **component system** adds logic to components of a particular component type. A **system** collects various different component system for different component types and makes them available to a world.
 - Components can be **activated** and **deactivated** using the system of the world they belong to. In most component systems, activating a component of that type has the effect of adding some object to the game simulation. For example, activating a `light` or a `model` component in the client system will add a light source or a 3D model to the scene, respectively.
 - Components can specify **dependencies** on other components in the world through a special component field type. A dependency takes the form of an entity ID and a component type ID. A component can only be activated if each of its dependencies has been activated already. Conversely, if a component is deactivated, all of its reverse dependencies will first be recursively deactivated as well. For example, the `model` component declares a dependency on a `drawable` component to specify what 3D geometry should be rendered for the 3D model it adds to the scene.
 - A world's **dependency graph** can be exported in [GraphViz DOT format](https://www.graphviz.org/doc/info/lang.html) and visualized as a graph. Pressing F9 in a game client will write a file called `world_dependencies.dot` into the working directory. This can then be converted in an SVG file using the command `dot -Tsvg -oworld_dependencies.svg world_dependencies.dot`. Open the `world_dependencies.svg` file in an appropriate image viewer such as Google Chrome to see the graph.
 - When a component field of a component is updated, the component itself is deactivated and reactivated with the new field value. Component systems can specify that certain field values may be **live updated** to skip this reactivation. For example, the `model` component's `position` dependency may be live updated. If the `position` component's coordinates change, there is no need to remove and recreate the 3D model in the scene. Instead, when the live update happens, the component system for the `model` component will simply update the 3D model's transform.
 - Components may be **delegated** or **undelegated** to indicate whether the system has **authority** over them. Some component types may only activate in a component system if the component is authoritative. For example, a `client` component indicates that a player is controlling the character represented by the entity containing the component. It will only activate for the `client` component the player has authority over, and not for other players with their own `client` components the player will meet in the game world.

A schema with component type definitions and a **client system** that comes with component systems for most of shoveler's engine concepts is bundled with the engine.
The [client example](examples/client.c) shows how the client system can be used explicitly to create a simple 3D scene.
The separate [shoveler-spatialos](https://github.com/FabianHahn/shoveler-spatialos) project makes use of the client system to add multiplayer support to shoveler using [Improbable's SpatialOS](https://improbable.io/spatialos).
While the existing client system's focus is on driving a game client, the idea is that there will also be a **server system** in the future that is able to execute server side logic using the same shared schema.
