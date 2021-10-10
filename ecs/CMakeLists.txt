set(SHOVELER_ECS_SRC
	src/component_field.c
	include/shoveler/component_field.h
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SHOVELER_ECS_SRC})

add_library(shoveler_ecs ${SHOVELER_ECS_SRC})
add_library(shoveler::shoveler_ecs ALIAS shoveler_ecs)
set_property(TARGET shoveler_ecs PROPERTY C_STANDARD 11)

target_include_directories(shoveler_ecs
	PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PUBLIC $<INSTALL_INTERFACE:include>
	PRIVATE src)

target_link_libraries(shoveler_ecs PUBLIC shoveler::shoveler_base)

if(SHOVELER_USE_GLIB)
	target_link_libraries(shoveler_ecs PUBLIC glib::glib)
else()
	target_link_libraries(shoveler_ecs PUBLIC fakeglib::fakeglibstatic)
endif()

if(SHOVELER_INSTALL)
	install(TARGETS shoveler_ecs
		EXPORT shoveler-targets
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
		RUNTIME DESTINATION bin
		INCLUDES DESTINATION include)

	install(DIRECTORY include/
		DESTINATION include)
endif()

