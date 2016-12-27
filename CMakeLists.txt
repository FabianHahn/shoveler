cmake_minimum_required(VERSION 3.1)
project(fakeglib)

enable_testing()

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})

option(FAKEGLIB_BUILD_TESTS "Build the fakeglib tests" ON)
option(FAKEGLIB_BUILD_STATIC "Build the fakeglib static library" ON)
option(FAKEGLIB_BUILD_SHARED "Build the fakeglib shared library" ON)

find_package(glib 2.48)
if(GLIB_FOUND)
	add_library(glib UNKNOWN IMPORTED)
	set_property(TARGET glib PROPERTY IMPORTED_LOCATION ${GLIB_LIBRARIES})
	set_property(TARGET glib PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${GLIB_INCLUDE_DIRS})
endif()

set(gtest_force_shared_crt ON CACHE BOOL "Disable gtest use of static runtime")
set(gtest_disable_pthreads ON CACHE BOOL "Disable gtest use of pthreads")

add_subdirectory(src)

if(FAKEGLIB_BUILD_TESTS)
	add_subdirectory(test)
	add_subdirectory(thirdparty)
endif()
