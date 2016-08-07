cmake_minimum_required(VERSION 3.1)
project(shoveler)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})

option(shoveler_use_glib "Link against system glib instead of bundled fakeglib." OFF)

if(shoveler_use_glib)
	find_package(glib 2.48 REQUIRED)
	add_library(glib UNKNOWN IMPORTED)
	set_property(TARGET glib PROPERTY IMPORTED_LOCATION ${GLIB_LIBRARIES})
	set_property(TARGET glib PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${GLIB_INCLUDE_DIRS})
endif()

set(GLAD_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/thirdparty/glad/include)
set(PNG_SHARED OFF CACHE BOOL "Disable building shared libpng library")
set(PNG_TESTS OFF CACHE BOOL "Disable building libpng tests")
set(SKIP_INSTALL_ALL ON CACHE BOOL "Skip libpng installation steps")
set(ZLIB_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/zlib)

add_subdirectory(src)
add_subdirectory(thirdparty)
