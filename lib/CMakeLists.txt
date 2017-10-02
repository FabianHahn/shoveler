set(SHOVELER_LIB_SRC
	src/camera/identity.c
	src/camera/perspective.c
	src/drawable/cube.c
	src/drawable/quad.c
	src/drawable/point.c
	src/filter/depth_texture_gaussian.c
	src/image/png.c
	src/image/ppm.c
	src/light/point.c
	src/light/spot.c
	src/material/color.c
	src/material/depth.c
	src/material/depth_texture_gaussian_filter.c
	src/material/particle.c
	src/material/screenspace_texture.c
	src/material/texture.c
	src/camera.c
	src/file.c
	src/framebuffer.c
	src/game.c
	src/global.c
	src/image.c
	src/input.c
	src/log.c
	src/material.c
	src/model.c
	src/opengl.c
	src/sampler.c
	src/scene.c
	src/shader.c
	src/shader_program.c
	src/texture.c
	src/uniform.c
	src/uniform_attachment.c
	src/uniform_map.c
)

add_library(shoveler ${SHOVELER_LIB_SRC})
set_property(TARGET shoveler PROPERTY C_STANDARD 99)

target_include_directories(shoveler
	PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PUBLIC $<INSTALL_INTERFACE:include>
    PRIVATE src)

target_link_libraries(shoveler glad glfw png_static zlibstatic)

if(shoveler_use_glib)
	target_link_libraries(shoveler glib)
else()
	target_link_libraries(shoveler fakeglibstatic)
endif()

if(UNIX)
	target_link_libraries(shoveler m)
endif()
