set(SHOVELER_BASE_SRC
	src/image/png.c
	src/image/ppm.c
	src/resources/image_png.c
	src/view/position.c
	src/view/resources.c
	src/color.c
	src/compression.c
	src/executor.c
	src/file.c
	src/image.c
	src/log.c
	src/resources.c
	src/view.c
	include/shoveler/image/png.h
	include/shoveler/image/ppm.h
	include/shoveler/resources/image_png.h
	include/shoveler/view/position.h
	include/shoveler/view/resources.h
	include/shoveler/color.h
	include/shoveler/compression.h
	include/shoveler/constants.h
	include/shoveler/executor.h
	include/shoveler/file.h
	include/shoveler/hash.h
	include/shoveler/image.h
	include/shoveler/log.h
	include/shoveler/resources.h
	include/shoveler/types.h
	include/shoveler/view.h
)

set(SHOVELER_BASE_TEST_SRC
	src/image/png_test.cpp
	src/resources/image_png_test.cpp
	src/view/resources_test.cpp
	src/color_test.cpp
	src/compression_test.cpp
	src/executor_test.cpp
	src/resources_test.cpp
	src/test.cpp
	src/types_test.cpp
)

include_directories(${ZLIB_INCLUDE_DIR})
add_library(shoveler_base ${SHOVELER_BASE_SRC})
set_property(TARGET shoveler_base PROPERTY C_STANDARD 99)

target_include_directories(shoveler_base
	PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PUBLIC $<INSTALL_INTERFACE:include>
    PRIVATE src)

target_link_libraries(shoveler_base png_static zlibstatic)

if(SHOVELER_USE_GLIB)
	target_link_libraries(shoveler_base glib)
else()
	target_link_libraries(shoveler_base fakeglibstatic)
endif()

if(UNIX)
	target_link_libraries(shoveler_base m)
endif()

if(SHOVELER_BUILD_TESTS)
	add_executable(shoveler_base_test ${SHOVELER_BASE_TEST_SRC})
	target_link_libraries(shoveler_base_test shoveler_base gtest)
	set_property(TARGET shoveler_base_test PROPERTY CXX_STANDARD 11)
	add_test(shoveler_base shoveler_base_test)
endif()
