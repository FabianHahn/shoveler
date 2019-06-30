if(SHOVELER_VENDOR_FAKEGLIB)
	add_subdirectory(fakeglib)
endif()

add_subdirectory(glad)

if(SHOVELER_VENDOR_GLFW)
	add_subdirectory(glfw)
endif()

if(SHOVELER_VENDOR_GOOGLETEST)
	add_subdirectory(googletest)
	add_library(GTest::gtest ALIAS gtest)
endif()

if(SHOVELER_VENDOR_LIBPNG)
	set(ZLIB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/zlib)
	add_subdirectory(libpng)
	add_library(PNG::PNG ALIAS png_static)
endif()

if(SHOVELER_VENDOR_ZLIB)
	add_subdirectory(zlib)
	target_include_directories(zlibstatic
		INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/zlib>)
	add_library(ZLIB::ZLIB ALIAS zlibstatic)
endif()

