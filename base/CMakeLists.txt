set(SHOVELER_BASE_SRC
	src/component/colliders.c
	src/component/font.c
	src/component/font_atlas.c
	src/component/fonts.c
	src/component/image.c
	src/component/position.c
	src/component/resource.c
	src/component/resources.c
	src/image/png.c
	src/image/ppm.c
	src/resources/image_png.c
	src/collider.c
	src/colliders.c
	src/color.c
	src/component.c
	src/compression.c
	src/executor.c
	src/file.c
	src/font.c
	src/font_atlas.c
	src/frustum.c
	src/image.c
	src/input_stream.c
	src/log.c
	src/projection.c
	src/resources.c
	src/view.c
	include/shoveler/collider/box.h
	include/shoveler/component/colliders.h
	include/shoveler/component/font.h
	include/shoveler/component/font_atlas.h
	include/shoveler/component/fonts.h
	include/shoveler/component/image.h
	include/shoveler/component/position.h
	include/shoveler/component/resource.h
	include/shoveler/component/resources.h
	include/shoveler/image/png.h
	include/shoveler/image/ppm.h
	include/shoveler/resources/image_png.h
	include/shoveler/collider.h
	include/shoveler/colliders.h
	include/shoveler/color.h
	include/shoveler/component.h
	include/shoveler/compression.h
	include/shoveler/constants.h
	include/shoveler/executor.h
	include/shoveler/file.h
	include/shoveler/font.h
	include/shoveler/font_atlas.h
	include/shoveler/frustum.h
	include/shoveler/hash.h
	include/shoveler/image.h
	include/shoveler/input_stream.h
	include/shoveler/log.h
	include/shoveler/projection.h
	include/shoveler/resources.h
	include/shoveler/types.h
	include/shoveler/view.h
)

set(SHOVELER_BASE_TEST_SRC
	src/image/png_test.cpp
	src/image/ppm_test.cpp
	src/resources/image_png_test.cpp
	src/colliders_test.cpp
	src/color_test.cpp
	src/component_test.cpp
	src/compression_test.cpp
	src/executor_test.cpp
	src/frustum_test.cpp
	src/image_testing.cpp
	src/image_testing.h
	src/resources_test.cpp
	src/test.cpp
	src/types_test.cpp
	src/view_test.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SHOVELER_BASE_SRC} ${SHOVELER_BASE_TEST_SRC})

add_library(shoveler_base ${SHOVELER_BASE_SRC})
add_library(shoveler::shoveler_base ALIAS shoveler_base)
set_property(TARGET shoveler_base PROPERTY C_STANDARD 11)

target_include_directories(shoveler_base
	PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PUBLIC $<INSTALL_INTERFACE:include>
	PRIVATE src)

target_link_libraries(shoveler_base PUBLIC Freetype::Freetype PRIVATE PNG::PNG ZLIB::ZLIB)

if(SHOVELER_USE_GLIB)
	target_link_libraries(shoveler_base PUBLIC glib::glib)
else()
	target_link_libraries(shoveler_base PUBLIC fakeglib::fakeglibstatic)
endif()

if(UNIX)
	target_link_libraries(shoveler_base PRIVATE m)
endif()

if(SHOVELER_INSTALL)
	install(TARGETS shoveler_base
		EXPORT shoveler-targets
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
		RUNTIME DESTINATION bin
		INCLUDES DESTINATION include)

	install(DIRECTORY include/
		DESTINATION include)
endif()

if(SHOVELER_BUILD_TESTS)
	add_executable(shoveler_base_test ${SHOVELER_BASE_TEST_SRC})

	target_include_directories(shoveler_base_test
		PRIVATE src)

	target_link_libraries(shoveler_base_test shoveler::shoveler_base GTest::gtest)
	set_property(TARGET shoveler_base_test PROPERTY CXX_STANDARD 11)
	add_test(shoveler_base shoveler_base_test)
endif()
