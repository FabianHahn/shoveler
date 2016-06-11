include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${OPENGL_INCLUDE_DIR} ${GLIB_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})

set(SHOVELER_SRC
	input.c
	log.c
	opengl.c
	shader_program.c
	shoveler.c
)

add_executable(shoveler ${SHOVELER_SRC})
set_property(TARGET shoveler PROPERTY C_STANDARD 99)
target_link_libraries(shoveler m ${GLIB_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} glfw)
