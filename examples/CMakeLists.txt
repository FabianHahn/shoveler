set(SHOVELER_EXAMPLE_CANVAS_SRC
	canvas.c
)

set(SHOVELER_EXAMPLE_CHUNK_SRC
	chunk.c
)

set(SHOVELER_EXAMPLE_LIGHTS_SRC
	lights.c
)

set(SHOVELER_EXAMPLE_TILES_SRC
	tiles.c
)

set(SHOVELER_EXAMPLE_VIEW_SRC
	view.c
)

if(SHOVELER_BUILD_EXAMPLES)
	add_executable(shoveler_example_canvas ${SHOVELER_EXAMPLE_CANVAS_SRC})
	set_property(TARGET shoveler_example_canvas PROPERTY C_STANDARD 99)
	target_link_libraries(shoveler_example_canvas shoveler::shoveler_opengl)

	add_executable(shoveler_example_chunk ${SHOVELER_EXAMPLE_CHUNK_SRC})
	set_property(TARGET shoveler_example_chunk PROPERTY C_STANDARD 99)
	target_link_libraries(shoveler_example_chunk shoveler::shoveler_opengl)

	add_executable(shoveler_example_lights ${SHOVELER_EXAMPLE_LIGHTS_SRC})
	set_property(TARGET shoveler_example_lights PROPERTY C_STANDARD 99)
	target_link_libraries(shoveler_example_lights shoveler::shoveler_opengl)

	add_executable(shoveler_example_tiles ${SHOVELER_EXAMPLE_TILES_SRC})
	set_property(TARGET shoveler_example_tiles PROPERTY C_STANDARD 99)
	target_link_libraries(shoveler_example_tiles shoveler::shoveler_opengl)

	add_executable(shoveler_example_view ${SHOVELER_EXAMPLE_VIEW_SRC})
	set_property(TARGET shoveler_example_view PROPERTY C_STANDARD 99)
	target_link_libraries(shoveler_example_view shoveler::shoveler_opengl)
endif()

