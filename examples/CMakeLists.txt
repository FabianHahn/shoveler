set(SHOVELER_EXAMPLE_LIGHTS_SRC
	lights.c
)

if(SHOVELER_BUILD_EXAMPLES)
	add_executable(lights ${SHOVELER_EXAMPLE_LIGHTS_SRC})
	set_property(TARGET lights PROPERTY C_STANDARD 99)
	target_link_libraries(lights shoveler_opengl)
endif()

