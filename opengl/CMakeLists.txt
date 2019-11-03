set(SHOVELER_OPENGL_SRC
	src/camera/identity.c
	src/camera/perspective.c
	src/collider/chunk.c
	src/component/canvas.c
	src/component/chunk.c
	src/component/chunk_layer.c
	src/component/client.c
	src/component/controller.c
	src/component/drawable.c
	src/component/light.c
	src/component/material.c
	src/component/model.c
	src/component/sampler.c
	src/component/scene.c
	src/component/shader_cache.c
	src/component/texture.c
	src/component/tile_sprite.c
	src/component/tile_sprite_animation.c
	src/component/tilemap.c
	src/component/tilemap_colliders.c
	src/component/tilemap_tiles.c
	src/component/tileset.c
	src/drawable/cube.c
	src/drawable/quad.c
	src/drawable/point.c
	src/drawable/tiles.c
	src/filter/depth_texture_gaussian.c
	src/light/point.c
	src/light/spot.c
	src/material/canvas.c
	src/material/chunk.c
	src/material/color.c
	src/material/depth.c
	src/material/depth_texture_gaussian_filter.c
	src/material/particle.c
	src/material/screenspace_texture.c
	src/material/text.c
	src/material/texture.c
	src/material/tile_sprite.c
	src/material/tilemap.c
	src/material/variation.c
	src/shader_program/model_vertex_projected.c
	src/shader_program/model_vertex_screenspace.c
	src/view/canvas.c
	src/view/chunk.c
	src/view/client.c
	src/view/drawable.c
	src/view/light.c
	src/view/material.c
	src/view/model.c
	src/view/texture.c
	src/view/tile_sprite.c
	src/view/tile_sprite_animation.c
	src/view/tilemap.c
	src/view/tilemap_tiles.c
	src/view/tileset.c
	src/camera.c
	src/canvas.c
	src/chunk.c
	src/controller.c
	src/font_atlas_texture.c
	src/framebuffer.c
	src/game.c
	src/global.c
	src/input.c
	src/material.c
	src/model.c
	src/opengl.c
	src/render_state.c
	src/sampler.c
	src/scene.c
	src/shader.c
	src/shader_cache.c
	src/shader_program.c
	src/text_texture_renderer.c
	src/texture.c
	src/tile_sprite_animation.c
	src/tilemap.c
	src/tileset.c
	src/uniform.c
	src/uniform_attachment.c
	src/uniform_map.c
	include/shoveler/camera/identity.h
	include/shoveler/camera/perspective.h
	include/shoveler/collider/chunk.h
	include/shoveler/component/canvas.h
	include/shoveler/component/chunk.h
	include/shoveler/component/chunk_layer.h
	include/shoveler/component/client.h
	include/shoveler/component/controller.h
	include/shoveler/component/drawable.h
	include/shoveler/component/light.h
	include/shoveler/component/material.h
	include/shoveler/component/model.h
	include/shoveler/component/sampler.h
	include/shoveler/component/scene.h
	include/shoveler/component/shader_cache.h
	include/shoveler/component/texture.h
	include/shoveler/component/tile_sprite.h
	include/shoveler/component/tile_sprite_animation.h
	include/shoveler/component/tilemap.h
	include/shoveler/component/tilemap_colliders.h
	include/shoveler/component/tilemap_tiles.h
	include/shoveler/component/tileset.h
	include/shoveler/drawable/cube.h
	include/shoveler/drawable/point.h
	include/shoveler/drawable/quad.h
	include/shoveler/drawable/tiles.h
	include/shoveler/filter/depth_texture_gaussian.h
	include/shoveler/light/point.h
	include/shoveler/light/spot.h
	include/shoveler/material/canvas.h
	include/shoveler/material/chunk.h
	include/shoveler/material/color.h
	include/shoveler/material/depth.h
	include/shoveler/material/depth_texture_gaussian_filter.h
	include/shoveler/material/particle.h
	include/shoveler/material/screenspace_texture.h
	include/shoveler/material/text.h
	include/shoveler/material/texture.h
	include/shoveler/material/tile_sprite.h
	include/shoveler/material/tilemap.h
	include/shoveler/material/variation.h
	include/shoveler/shader_program/model_vertex.h
	include/shoveler/shader_program/model_vertex_projected.h
	include/shoveler/shader_program/model_vertex_screenspace.h
	include/shoveler/view/canvas.h
	include/shoveler/view/chunk.h
	include/shoveler/view/client.h
	include/shoveler/view/controller.h
	include/shoveler/view/drawable.h
	include/shoveler/view/light.h
	include/shoveler/view/material.h
	include/shoveler/view/model.h
	include/shoveler/view/scene.h
	include/shoveler/view/shader_cache.h
	include/shoveler/view/texture.h
	include/shoveler/view/tile_sprite.h
	include/shoveler/view/tile_sprite_animation.h
	include/shoveler/view/tilemap.h
	include/shoveler/view/tilemap_tiles.h
	include/shoveler/view/tileset.h
	include/shoveler/camera.h
	include/shoveler/canvas.h
	include/shoveler/chunk.h
	include/shoveler/controller.h
	include/shoveler/drawable.h
	include/shoveler/filter.h
	include/shoveler/font_atlas_texture.h
	include/shoveler/framebuffer.h
	include/shoveler/game.h
	include/shoveler/global.h
	include/shoveler/input.h
	include/shoveler/light.h
	include/shoveler/material.h
	include/shoveler/model.h
	include/shoveler/opengl.h
	include/shoveler/render_state.h
	include/shoveler/sampler.h
	include/shoveler/scene.h
	include/shoveler/shader.h
	include/shoveler/shader_cache.h
	include/shoveler/shader_program.h
	include/shoveler/text_texture_renderer.h
	include/shoveler/texture.h
	include/shoveler/tile_sprite_animation.h
	include/shoveler/tilemap.h
	include/shoveler/tileset.h
	include/shoveler/uniform_attachment.h
	include/shoveler/uniform.h
	include/shoveler/uniform_map.h
)

set(SHOVELER_OPENGL_TEST_SRC
	src/chunk_test.cpp
	src/shader_cache_test.cpp
	src/test.cpp
)

add_library(shoveler_opengl ${SHOVELER_OPENGL_SRC})
add_library(shoveler::shoveler_opengl ALIAS shoveler_opengl)
set_property(TARGET shoveler_opengl PROPERTY C_STANDARD 99)

target_include_directories(shoveler_opengl
	PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PUBLIC $<INSTALL_INTERFACE:include>
	PRIVATE src)

target_link_libraries(shoveler_opengl PUBLIC shoveler_base glad::glad glfw)

if(SHOVELER_INSTALL)
	install(TARGETS shoveler_opengl
		EXPORT shoveler-targets
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
		RUNTIME DESTINATION bin
		INCLUDES DESTINATION include)

	install(DIRECTORY include/
		DESTINATION include)
endif()

if(SHOVELER_BUILD_TESTS)
	add_executable(shoveler_opengl_test ${SHOVELER_OPENGL_TEST_SRC})

	target_include_directories(shoveler_opengl_test
		PRIVATE src)

	target_link_libraries(shoveler_opengl_test shoveler::shoveler_opengl GTest::gtest)
	set_property(TARGET shoveler_opengl_test PROPERTY CXX_STANDARD 11)
	add_test(shoveler_opengl shoveler_opengl_test)
endif()
