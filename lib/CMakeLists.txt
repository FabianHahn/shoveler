set(SHOVELER_LIB_SRC
	camera/identity.c
	camera/perspective.c
	drawable/cube.c
	drawable/quad.c
	drawable/point.c
	filter/depth_texture_gaussian.c
	image/png.c
	image/ppm.c
	light/point.c
	light/spot.c
	material/color.c
	material/depth.c
	material/depth_texture_gaussian_filter.c
	material/particle.c
	material/screenspace_texture.c
	material/texture.c
	camera.c
	file.c
	framebuffer.c
	game.c
	global.c
	image.c
	input.c
	log.c
	material.c
	model.c
	opengl.c
	sampler.c
	scene.c
	shader.c
	shader_program.c
	texture.c
	uniform.c
	uniform_attachment.c
	uniform_map.c
)

add_library(shoveler ${SHOVELER_LIB_SRC})
set_property(TARGET shoveler PROPERTY C_STANDARD 99)
target_link_libraries(shoveler glad glfw png_static zlibstatic)
target_include_directories(shoveler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${GLIB_INCLUDE_DIRS} ${GLAD_INCLUDE_DIRS})

if(shoveler_use_glib)
	target_link_libraries(shoveler glib)
else()
	target_link_libraries(shoveler fakeglibstatic)
endif()

if(UNIX)
	target_link_libraries(shoveler m)
endif()
