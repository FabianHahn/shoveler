include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${GLIB_INCLUDE_DIRS} ${GLAD_INCLUDE_DIRS})

set(SHOVELER_SAMPLE_SRC
	sample.c
	shoveler.c
)

if(SHOVELER_BUILD_SAMPLE)
	add_executable(sample ${SHOVELER_SAMPLE_SRC})
	set_property(TARGET sample PROPERTY C_STANDARD 99)
	target_link_libraries(sample shoveler_opengl)
endif()

