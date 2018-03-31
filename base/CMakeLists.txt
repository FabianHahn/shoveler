set(SHOVELER_BASE_SRC
	src/file.c
	src/log.c
	include/shoveler/constants.h
	include/shoveler/file.h
	include/shoveler/log.h
	include/shoveler/types.h
)

add_library(shoveler_base ${SHOVELER_BASE_SRC})
set_property(TARGET shoveler_base PROPERTY C_STANDARD 99)

target_include_directories(shoveler_base
	PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PUBLIC $<INSTALL_INTERFACE:include>
    PRIVATE src)

target_link_libraries(shoveler_base)

if(shoveler_use_glib)
	target_link_libraries(shoveler_base glib)
else()
	target_link_libraries(shoveler_base fakeglibstatic)
endif()

if(UNIX)
	target_link_libraries(shoveler_base m)
endif()

