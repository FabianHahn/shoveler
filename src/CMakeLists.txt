include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${GLIB_INCLUDE_DIRS} ${GLAD_INCLUDE_DIRS})

set(SHOVELER_SRC
	cameras/perspective.c
	drawable/cube.c
	drawable/quad.c
	image/png.c
	image/ppm.c
	material/color.c
	material/depth.c
	material/texture.c
	camera.c
	file.c
	framebuffer.c
	image.c
	input.c
	light.c
	log.c
	material.c
	model.c
	opengl.c
	sample.c
	sampler.c
	scene.c
	shader.c
	shader_program.c
	shoveler.c
	texture.c
	uniform.c
	uniform_attachment.c
	uniform_map.c
)

add_executable(shoveler ${SHOVELER_SRC})
set_property(TARGET shoveler PROPERTY C_STANDARD 99)
target_link_libraries(shoveler glad glfw png_static zlibstatic)

if(shoveler_use_glib)
	target_link_libraries(shoveler glib)
else()
	target_link_libraries(shoveler fakeglibstatic)
endif()

if(UNIX)
	target_link_libraries(shoveler m)
endif()
