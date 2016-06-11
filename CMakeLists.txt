cmake_minimum_required(VERSION 3.1)
project(shoveler)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})

find_package(OpenGL REQUIRED)
find_package(glib 2.48 REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 3.2 REQUIRED)

add_subdirectory(src)