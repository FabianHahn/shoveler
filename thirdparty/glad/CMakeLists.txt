project(glad VERSION 0.1 LANGUAGES C)

add_library(glad
	src/glad.c
	include/glad/glad.h
	include/KHR/khrplatform.h)
add_library(glad::glad ALIAS glad)

target_include_directories(glad
	PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PUBLIC $<INSTALL_INTERFACE:include>
	PRIVATE src)

if(SHOVELER_INSTALL)
	install(TARGETS glad
		EXPORT shoveler-targets
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
		RUNTIME DESTINATION bin
		INCLUDES DESTINATION include)

	install(DIRECTORY include/
		DESTINATION include)
endif()
