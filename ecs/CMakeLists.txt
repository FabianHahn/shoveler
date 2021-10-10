set(SHOVELER_ECS_SRC
	src/component.c
	src/component_field.c
	src/component_type.c
	include/shoveler/component.h
	include/shoveler/component_field.h
	include/shoveler/component_type.h
	include/shoveler/entity_component_id.h
)

set(SHOVELER_ECS_TEST_SRC
	src/component_test.cpp
	src/test.cpp
	src/test_component_types.h
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

if(SHOVELER_BUILD_TESTS)
	add_executable(shoveler_ecs_test ${SHOVELER_ECS_TEST_SRC})

	target_include_directories(shoveler_ecs_test
		PRIVATE src)

	target_link_libraries(shoveler_ecs_test shoveler::shoveler_ecs GTest::gtest GTest::gmock)
	set_property(TARGET shoveler_ecs_test PROPERTY CXX_STANDARD 11)
	add_test(shoveler_ecs shoveler_ecs_test)
endif()
