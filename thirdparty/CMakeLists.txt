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

if(SHOVELER_VENDOR_ZLIB)
	add_subdirectory(zlib)
	target_include_directories(zlibstatic
		INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/zlib>)
	add_library(ZLIB::ZLIB ALIAS zlibstatic)

	set(ZLIB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/zlib)
	set(ZLIB_LIBRARIES ZLIB::ZLIB)
	set(ZLIB_DEFINITIONS "")
	set(ZLIB_FOUND 1)
	set(CMAKE_DISABLE_FIND_PACKAGE_ZLIB 1)
endif()

if(SHOVELER_VENDOR_LIBPNG)
	add_subdirectory(libpng)
	add_library(PNG::PNG ALIAS png_static)

	set(PNG_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/libpng)
	set(PNG_LIBRARIES PNG::PNG)
	set(PNG_DEFINITIONS "")
	set(PNG_FOUND 1)
	set(CMAKE_DISABLE_FIND_PACKAGE_PNG 1)
endif()

if(SHOVELER_VENDOR_FREETYPE)
	add_subdirectory(freetype)
	add_library(Freetype::Freetype ALIAS freetype)
endif()

