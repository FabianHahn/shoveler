cmake_minimum_required(VERSION 3.1)
project(shoveler)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})

find_package(glib 2.48 REQUIRED)

add_subdirectory(thirdparty)
set(GLAD_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/thirdparty/glad/include)

add_subdirectory(src)