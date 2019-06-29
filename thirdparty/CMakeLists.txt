if(SHOVELER_VENDOR_FAKEGLIB)
	add_subdirectory(fakeglib)
endif()

add_subdirectory(glad)
add_subdirectory(glfw)

if(SHOVELER_VENDOR_GOOGLETEST)
	add_subdirectory(googletest)
	add_library(GTest::gtest ALIAS gtest)
endif()

add_subdirectory(zlib)
add_subdirectory(libpng)
