set(SHOVELER_VIEW_BASE_SRC
	src/component/colliders.c
	src/component/font.c
	src/component/font_atlas.c
	src/component/fonts.c
	src/component/image.c
	src/component/position.c
	src/component/resource.c
	src/component/resources.c
	src/component.c
	src/view.c
	include/shoveler/component/colliders.h
	include/shoveler/component/font.h
	include/shoveler/component/font_atlas.h
	include/shoveler/component/fonts.h
	include/shoveler/component/image.h
	include/shoveler/component/position.h
	include/shoveler/component/resource.h
	include/shoveler/component/resources.h
	include/shoveler/component.h
	include/shoveler/view.h
)

set(SHOVELER_VIEW_BASE_TEST_SRC
	src/component_test.cpp
	src/view_test.cpp
	src/test.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SHOVELER_VIEW_BASE_SRC} ${SHOVELER_VIEW_BASE_TEST_SRC})

add_library(shoveler_view_base ${SHOVELER_VIEW_BASE_SRC})
add_library(shoveler::shoveler_view_base ALIAS shoveler_view_base)
set_property(TARGET shoveler_view_base PROPERTY C_STANDARD 11)

target_include_directories(shoveler_view_base
	PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PUBLIC $<INSTALL_INTERFACE:include>
	PRIVATE src)

target_link_libraries(shoveler_view_base PUBLIC shoveler::shoveler_base Freetype::Freetype PRIVATE PNG::PNG ZLIB::ZLIB)

if(SHOVELER_USE_GLIB)
	target_link_libraries(shoveler_view_base PUBLIC glib::glib)
else()
	target_link_libraries(shoveler_view_base PUBLIC fakeglib::fakeglibstatic)
endif()

if(UNIX)
	target_link_libraries(shoveler_view_base PRIVATE m)
endif()

if(SHOVELER_INSTALL)
	install(TARGETS shoveler_view_base
		EXPORT shoveler-targets
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
		RUNTIME DESTINATION bin
		INCLUDES DESTINATION include)

	install(DIRECTORY include/
		DESTINATION include)
endif()

if(SHOVELER_BUILD_TESTS)
	add_executable(shoveler_view_base_test ${SHOVELER_VIEW_BASE_TEST_SRC})

	target_include_directories(shoveler_view_base_test
		PRIVATE src)

	target_link_libraries(shoveler_view_base_test shoveler::shoveler_view_base GTest::gtest)
	set_property(TARGET shoveler_view_base_test PROPERTY CXX_STANDARD 11)
	add_test(shoveler_view_base shoveler_view_base_test)
endif()
