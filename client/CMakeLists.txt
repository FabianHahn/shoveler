set(SHOVELER_CLIENT_SRC
	include/shoveler/client_system.h
	include/shoveler/component/canvas.h
	include/shoveler/component/drawable.h
	include/shoveler/component/font.h
	include/shoveler/component/font_atlas.h
	include/shoveler/component/font_atlas_texture.h
	include/shoveler/component/image.h
	include/shoveler/component/light.h
	include/shoveler/component/position.h
	include/shoveler/component/resource.h
	include/shoveler/component/sampler.h
	include/shoveler/component/text_texture_renderer.h
	include/shoveler/component/texture.h
	include/shoveler/component/tilemap.h
	include/shoveler/component/tilemap_colliders.h
	include/shoveler/component/tilemap_tiles.h
	include/shoveler/component/tileset.h
	src/client_system.c
	src/component/canvas.c
	src/component/drawable.c
	src/component/font.c
	src/component/font_atlas.c
	src/component/font_atlas_texture.c
	src/component/image.c
	src/component/light.c
	src/component/position.c
	src/component/sampler.c
	src/component/text_texture_renderer.c
	src/component/texture.c
	src/component/tilemap.c
	src/component/tilemap_colliders.c
	src/component/tilemap_tiles.c
	src/component/tileset.c
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SHOVELER_CLIENT_SRC})

add_library(shoveler_client ${SHOVELER_CLIENT_SRC})
add_library(shoveler::shoveler_client ALIAS shoveler_client)
set_property(TARGET shoveler_client PROPERTY C_STANDARD 11)

target_include_directories(shoveler_client
	PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PUBLIC $<INSTALL_INTERFACE:include>
	PRIVATE src)

target_link_libraries(shoveler_client PUBLIC shoveler::shoveler_opengl shoveler::shoveler_schema)

if(SHOVELER_USE_GLIB)
	target_link_libraries(shoveler_client PUBLIC glib::glib)
else()
	target_link_libraries(shoveler_client PUBLIC fakeglib::fakeglibstatic)
endif()

if(SHOVELER_INSTALL)
	install(TARGETS shoveler_client
		EXPORT shoveler-targets
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
		RUNTIME DESTINATION bin
		INCLUDES DESTINATION include)

	install(DIRECTORY include/
		DESTINATION include)
endif()

