set(SHOVELER_VIEW_OPENGL_SRC
	src/component/canvas.c
	src/component/client.c
	src/component/controller.c
	src/component/drawable.c
	src/component/font_atlas_texture.c
	src/component/light.c
	src/component/material.c
	src/component/model.c
	src/component/render_state.c
	src/component/sampler.c
	src/component/scene.c
	src/component/shader_cache.c
	src/component/sprite.c
	src/component/text_sprite.c
	src/component/text_texture_renderer.c
	src/component/texture.c
	src/component/texture_sprite.c
	src/component/tile_sprite.c
	src/component/tile_sprite_animation.c
	src/component/tilemap.c
	src/component/tilemap_colliders.c
	src/component/tilemap_sprite.c
	src/component/tilemap_tiles.c
	src/component/tileset.c
	src/game_view.c
	include/shoveler/component/canvas.h
	include/shoveler/component/client.h
	include/shoveler/component/controller.h
	include/shoveler/component/drawable.h
	include/shoveler/component/font_atlas_texture.h
	include/shoveler/component/light.h
	include/shoveler/component/material.h
	include/shoveler/component/model.h
	include/shoveler/component/render_state.h
	include/shoveler/component/sampler.h
	include/shoveler/component/scene.h
	include/shoveler/component/shader_cache.h
	include/shoveler/component/sprite.h
	include/shoveler/component/text_sprite.h
	include/shoveler/component/text_texture_renderer.h
	include/shoveler/component/texture.h
	include/shoveler/component/texture_sprite.h
	include/shoveler/component/tile_sprite.h
	include/shoveler/component/tile_sprite_animation.h
	include/shoveler/component/tilemap.h
	include/shoveler/component/tilemap_colliders.h
	include/shoveler/component/tilemap_sprite.h
	include/shoveler/component/tilemap_tiles.h
	include/shoveler/component/tileset.h
	include/shoveler/game_view.h
)

set(SHOVELER_VIEW_OPENGL_TEST_SRC
	src/test.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SHOVELER_VIEW_OPENGL_SRC} ${SHOVELER_VIEW_OPENGL_TEST_SRC})

add_library(shoveler_view_opengl ${SHOVELER_VIEW_OPENGL_SRC})
add_library(shoveler::shoveler_view_opengl ALIAS shoveler_view_opengl)
set_property(TARGET shoveler_view_opengl PROPERTY C_STANDARD 11)

target_include_directories(shoveler_view_opengl
	PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PUBLIC $<INSTALL_INTERFACE:include>
	PRIVATE src)

target_link_libraries(shoveler_view_opengl PUBLIC shoveler_opengl shoveler_view_base shoveler_base glad::glad glfw)

if(SHOVELER_INSTALL)
	install(TARGETS shoveler_view_opengl
		EXPORT shoveler-targets
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
		RUNTIME DESTINATION bin
		INCLUDES DESTINATION include)

	install(DIRECTORY include/
		DESTINATION include)
endif()

if(SHOVELER_BUILD_TESTS)
	add_executable(shoveler_view_opengl_test ${SHOVELER_VIEW_OPENGL_TEST_SRC})

	target_include_directories(shoveler_view_opengl_test
		PRIVATE src)

	target_link_libraries(shoveler_view_opengl_test shoveler::shoveler_view_opengl GTest::gtest)
	set_property(TARGET shoveler_view_opengl_test PROPERTY CXX_STANDARD 11)
	add_test(shoveler_view_opengl shoveler_view_opengl_test)
endif()
