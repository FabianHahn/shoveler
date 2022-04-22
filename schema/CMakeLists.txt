set(SHOVELER_SCHEMA_SRC
	src/schema/base.c
	src/schema/components.c
	src/schema/opengl.c
	include/shoveler/schema/base.h
	include/shoveler/schema/components.h
	include/shoveler/schema/opengl.h
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SHOVELER_SCHEMA_SRC})

add_library(shoveler_schema ${SHOVELER_SCHEMA_SRC})
add_library(shoveler::shoveler_schema ALIAS shoveler_schema)
set_property(TARGET shoveler_schema PROPERTY C_STANDARD 11)

target_include_directories(shoveler_schema
	PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PUBLIC $<INSTALL_INTERFACE:include>
	PRIVATE src)

target_link_libraries(shoveler_schema PUBLIC shoveler::shoveler_ecs)

if(SHOVELER_USE_GLIB)
	target_link_libraries(shoveler_schema PUBLIC glib::glib)
else()
	target_link_libraries(shoveler_schema PUBLIC fakeglib::fakeglibstatic)
endif()

if(SHOVELER_INSTALL)
	install(TARGETS shoveler_schema
		EXPORT shoveler-targets
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
		RUNTIME DESTINATION bin
		INCLUDES DESTINATION include)

	install(DIRECTORY include/
		DESTINATION include)
endif()

