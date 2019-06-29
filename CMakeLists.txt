cmake_minimum_required(VERSION 3.1)
project(fakeglib VERSION 0.1 LANGUAGES CXX)

enable_testing()

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})

option(FAKEGLIB_BUILD_TESTS "Build the fakeglib tests" ON)
option(FAKEGLIB_BUILD_STATIC "Build the fakeglib static library" ON)
option(FAKEGLIB_BUILD_SHARED "Build the fakeglib shared library" ON)
option(FAKEGLIB_VENDOR_GOOGLETEST "Vendor the googletest thirdparty library" ON)
option(FAKEGLIB_INSTALL "Enable fakeglib installation" ON)

find_package(glib 2.48)

set(gtest_force_shared_crt ON CACHE BOOL "Disable gtest use of static runtime")
set(gtest_disable_pthreads ON CACHE BOOL "Disable gtest use of pthreads")
set(INSTALL_GTEST OFF CACHE BOOL "Disable gtest installation")

if(FAKEGLIB_INSTALL)
	set(INSTALL_CONFIGDIR lib/cmake/fakeglib)

	install(EXPORT fakeglib-targets
		FILE fakeglibTargets.cmake
		NAMESPACE fakeglib::
		DESTINATION ${INSTALL_CONFIGDIR}
	)

	include(CMakePackageConfigHelpers)
	write_basic_package_version_file(
		${CMAKE_CURRENT_BINARY_DIR}/fakeglibConfigVersion.cmake
		VERSION ${PROJECT_VERSION}
		COMPATIBILITY AnyNewerVersion
	)

	configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/fakeglibConfig.cmake.in
		${CMAKE_CURRENT_BINARY_DIR}/fakeglibConfig.cmake
		INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
	)

	install(FILES
		${CMAKE_CURRENT_LIST_DIR}/Findglib.cmake
		${CMAKE_CURRENT_BINARY_DIR}/fakeglibConfig.cmake
		${CMAKE_CURRENT_BINARY_DIR}/fakeglibConfigVersion.cmake
		DESTINATION ${INSTALL_CONFIGDIR}
	)

	configure_file(${CMAKE_CURRENT_LIST_DIR}/Findglib.cmake
		${CMAKE_CURRENT_BINARY_DIR}/Findglib.cmake
		COPYONLY
	)

	export(PACKAGE fakeglib)
endif()

add_subdirectory(lib)

if(FAKEGLIB_BUILD_TESTS)
	add_subdirectory(test)
	add_subdirectory(thirdparty)
endif()
