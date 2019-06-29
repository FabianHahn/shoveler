if(FAKEGLIB_VENDOR_GOOGLETEST)
	add_subdirectory(googletest)
	add_library(GTest::gtest ALIAS gtest)
endif()
