cmake_minimum_required(VERSION 3.1)
project(shoveler)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})

find_package(glfw3 3.2 REQUIRED)
find_package(glib 2.48 REQUIRED)

add_subdirectory(src)