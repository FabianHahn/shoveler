[![ci](https://github.com/FabianHahn/shoveler/actions/workflows/ci.yml/badge.svg)](https://github.com/FabianHahn/shoveler/actions/workflows/ci.yml)

# shoveler

![lights demo screenshot](https://github.com/FabianHahn/shoveler-assets/raw/master/examples/lights.png)

## Introduction
**shoveler** is a _minimalistic_ OpenGL game engine.
It is minimalistic in the following sense:
 * **No magic**: It is completely written in C and thus only uses the most basic of programming language concepts, making the effect of every line of code as obvious as possible without any implicit "magic" happening in the background.
 * **Written from scratch, with batteries included**: It uses minimal dependencies that are all included as source, enabling a game to compile into a single statically linked executable that can be easily distributed and executed anywhere.
 * **Pop off the hood anytime**: All high-level abstractions are built on top of lower-level ones that are also made accessible and can be used directly if desired.
 * **Cross compilation**: **shoveler** is not tied to a specific platform, and cross compiles easily between its supported platforms Linux and Windows.

## Build system
**shoveler** uses [Bazel](https://bazel.build/) to handle all dependencies and manage the build.
Bazel will fetch and set up all dependencies automatically, which includes the compiler toolchain.
**shoveler** uses the [bazel-zig-cc](https://sr.ht/~motiejus/bazel-zig-cc/) toolchain, which enables the use of the [zig compiler](https://ziglang.org/) for [C/C++ cross compilation via `zig cc`](https://andrewkelley.me/post/zig-cc-powerful-drop-in-replacement-gcc-clang.html) that uses a modern build of [Clang](https://clang.llvm.org/) internally.

### Dependencies
**shoveler** links against the following third-party dependencies:
 * [fakeglib](https://github.com/FabianHahn/fakeglib) - used as "standard library" for data structures
 * [freetype](https://github.com/freetype/freetype) - used to load fonts and render text
 * [glad](https://github.com/Dav1dde/glad) - used to load OpenGL symbols
 * [glfw](http://www.glfw.org/) - used manage the OpenGL context and input handling
 * [libpng](https://github.com/glennrp/libpng) - used to load and write texture image files 
 * [zlib](https://github.com/madler/zlib) - used to compress and uncompress data

Further, Linux builds dynamically link against the X11/Xorg client libraries to enable glfw window creation.
These link targets are provided by the excellent [hexops/sdk-linux-x86_64](https://github.com/hexops/sdk-linux-x86_64) project originally created for the [Mach game engine](https://github.com/hexops/mach).

## Usage

**shoveler** currently supports Linux and Windows platforms as both host and target platforms.

### Prerequisites
The only prerequisite to build shoveler is [bazel](https://bazel.build/).
You don't need to install anything else to build shoveler, not even a compiler.
Bazel will fetch everything needed and set it up automatically.
The recommended way to set up Bazel is through [bazelisk](https://github.com/bazelbuild/bazelisk), which will automatically download the appropriate version of Bazel as specified in the [`.bazelversion`](.bazelversion) file.
Alternatively, you can also install the required version of bazel [manually](https://bazel.build/start).

### Build
You can specify the target platform to build for using the `--config` flag and select either `linux` or `windows`.
This will work whether your host platform (i.e. the platform you are working on) is Linux or Windows.

To build shoveler for Linux, run:
```
bazel build ... --config linux
```

To build shoveler for Windows, instead run:
```
bazel build ... --config windows
```

_Note:_ Windows builds currently won't result in `.exe` extensions. However, you can run them just fine using `bazel run` or by renaming the resulting binaries to have an `.exe` extension yourself.

### Tests

To run unit tests on Linux, run:
```
bazel test ... --config linux
```

To run them on Windows, run:
```
bazel test ... --config windows
```

### Examples
The included examples can be found in the [examples](examples/) directory.
After running `bazel build`, the built binaries can be found in `bazel-bin/examples`.

To run them directly via bazel on Linux, run e.g.:
```
bazel run //examples:client --config linux
```

On Windows, run e.g.:
```
bazel run //examples:client --config windows
```

You should be able to run the Windows binaries on Linux by installing [Wine](https://www.winehq.org/) and the wine-binfmt package.

The following example binaries are included that demonstrate different shoveler features:
 * [canvas](examples/canvas.c): 2D capabilities including character animation and collision detection.
 * [canvas_layers](examples/canvas_layers.c): drawing 2D canvasses with multiple layers and transparency.
 * [client](examples/client.c): client side entity component system and its support for both 2D and 3D components.
 * [client_text](examples/client_text.c): rendering text within the client side entity component system.
 * [font](examples/font.c): reading a font file and create a font atlas.
 * [lights](examples/lights.c): rendering an animated 3D scene with multiple light sources and shadows.
 * [text](examples/text.c): rendering dynamic text within a 3D scene.
 * [tiles](examples/tiles.c): 2D tilemapping.

#### Example screenshots
![text demo screenshot](https://github.com/FabianHahn/shoveler-assets/raw/master/examples/text.png)
![text demo screenshot](https://github.com/FabianHahn/shoveler-assets/raw/master/examples/client.png)

### System toolchain

Alternatively, bazel can also use your system compiler such as gcc on Linux or MSVC on Windows, but this requires their prior installation and configuration and doesn't support cross compilation.

If `--config` is not specified, bazel will [autodetect the first C/C++ toolchain](https://bazel.build/docs/cc-toolchain-config-reference) it can find and attempt to use that.
**shoveler** was successfully tested to compile on Linux with _gcc_ and _clang_ and on Windows with _Visual Studio 2019_.

## Architecture

### Entity component system

**shoveler** comes with its own entity component system that consists of the following main parts:
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
