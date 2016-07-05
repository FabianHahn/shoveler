include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${GLIB_INCLUDE_DIRS} ${GLAD_INCLUDE_DIRS} ${LIBPNG_INCLUDE_DIRS})

set(SHOVELER_SRC
	cameras/perspective.c
	drawables/cube.c
	camera.c
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
target_link_libraries(shoveler ${GLIB_LIBRARIES} glad glfw png_static zlibstatic)

if(UNIX)
	target_link_libraries(shoveler m)
endif()
