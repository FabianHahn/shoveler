cmake_minimum_required(VERSION 3.1)
project(shoveler VERSION 0.2 LANGUAGES C CXX)

enable_testing()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

option(SHOVELER_BUILD_TESTS "Build the shoveler tests" ON)
option(SHOVELER_BUILD_EXAMPLES "Build example binaries using shoveler." ON)
option(SHOVELER_USE_GLIB "Link against system glib instead of bundled fakeglib." OFF)
option(SHOVELER_VENDOR_FAKEGLIB "Vendor the fakeglib thirdparty library." ON)
option(SHOVELER_VENDOR_GLFW "Vendor the glfw thirdparty library." ON)
option(SHOVELER_VENDOR_GOOGLETEST "Vendor the googletest thirdparty library" ON)
option(SHOVELER_VENDOR_LIBPNG "Vendor the libpng thirdparty library." ON)
option(SHOVELER_VENDOR_ZLIB "Vendor the zlib thirdparty library." ON)
option(SHOVELER_INSTALL "Enable shoveler installation" OFF)

if(SHOVELER_VENDOR_GLFW)
	message(STATUS "Using vendored glfw.")
else()
	find_package(glfw3 3.3 REQUIRED)
endif()

if(SHOVELER_USE_GLIB)
	find_package(glib 2.48 REQUIRED)
endif()

if(SHOVELER_VENDOR_FAKEGLIB)
	message(STATUS "Using vendored fakeglib.")
else()
	find_package(fakeglib REQUIRED)
endif()

if(SHOVELER_VENDOR_LIBPNG)
	message(STATUS "Using vendored libpng.")
else()
	find_package(PNG 1.6.24 REQUIRED)
endif()

if(SHOVELER_VENDOR_ZLIB)
	message(STATUS "Using vendored zlib.")
else()
	find_package(ZLIB 1.2.8 REQUIRED)
endif()

set(FAKEGLIB_BUILD_TESTS OFF CACHE BOOL "Disable building fakeglib tests")
set(FAKEGLIB_BUILD_SHARED OFF CACHE BOOL "Disable building shared fakeglib")
set(FAKEGLIB_VENDOR_GOOGLETEST OFF CACHE BOOL "Don't vendor the googletest thirdparty library")
set(FAKEGLIB_INSTALL OFF CACHE BOOL "Disable fakeglib installation")
set(INSTALL_GTEST OFF CACHE BOOL "Disable gtest installation")
set(INSTALL_GMOCK OFF CACHE BOOL "Disable gmock installation")
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Disable building glfw examples")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Disable building glfw tests")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Disable building glfw documentation")
set(PNG_SHARED OFF CACHE BOOL "Disable building shared libpng library")
set(PNG_TESTS OFF CACHE BOOL "Disable building libpng tests")
set(SKIP_INSTALL_ALL ON CACHE BOOL "Skip libpng installation steps")

if(SHOVELER_INSTALL)
	if(SHOVELER_VENDOR_FAKEGLIB OR SHOVELER_VENDOR_GLFW OR SHOVELER_VENDOR_LIBPNG OR SHOVELER_VENDOR_ZLIB)
		message(FATAL_ERROR "Cannot install (SHOVELER_INSTALL) when fakeglib, glfw, libpng or zlib are vendored.")
	endif()

	set(INSTALL_CONFIGDIR lib/cmake/shoveler)

	install(EXPORT shoveler-targets
		FILE shovelerTargets.cmake
		NAMESPACE shoveler::
		DESTINATION ${INSTALL_CONFIGDIR}
	)

	include(CMakePackageConfigHelpers)
	write_basic_package_version_file(
		${CMAKE_CURRENT_BINARY_DIR}/shovelerConfigVersion.cmake
		VERSION ${PROJECT_VERSION}
		COMPATIBILITY AnyNewerVersion
	)

	configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/shovelerConfig.cmake.in
		${CMAKE_CURRENT_BINARY_DIR}/shovelerConfig.cmake
		INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
	)

	install(FILES
		${CMAKE_CURRENT_LIST_DIR}/Findglib.cmake
		${CMAKE_CURRENT_BINARY_DIR}/shovelerConfig.cmake
		${CMAKE_CURRENT_BINARY_DIR}/shovelerConfigVersion.cmake
		DESTINATION ${INSTALL_CONFIGDIR}
	)

	configure_file(${CMAKE_CURRENT_LIST_DIR}/Findglib.cmake
		${CMAKE_CURRENT_BINARY_DIR}/Findglib.cmake
		COPYONLY
	)

	export(EXPORT shoveler-targets
		FILE ${CMAKE_CURRENT_BINARY_DIR}/shovelerTargets.cmake
		NAMESPACE shoveler::
	)

	export(PACKAGE shoveler)
endif()

add_subdirectory(base)
add_subdirectory(examples)
add_subdirectory(opengl)
add_subdirectory(thirdparty)
