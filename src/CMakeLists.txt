include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${GLIB_INCLUDE_DIRS})

set(SHOVELER_SRC
	log.c
	shoveler.c
)

add_executable(shoveler ${SHOVELER_SRC})
set_property(TARGET shoveler PROPERTY C_STANDARD 99)
target_link_libraries(shoveler ${GLIB_LIBRARIES})
target_link_libraries(shoveler glfw)
