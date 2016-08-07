include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${GLIB_INCLUDE_DIRS} ${GLAD_INCLUDE_DIRS})

set(SHOVELER_SRC
	cameras/perspective.c
	drawables/cube.c
	image/png.c
	image/ppm.c
	camera.c
	file.c
	image.c
	input.c
	log.c
	material.c
	model.c
	opengl.c
	sample.c
	scene.c
	shader.c
	shader_program.c
	shoveler.c
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
