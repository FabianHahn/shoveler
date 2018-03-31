include(GenerateExportHeader)

set(FAKEGLIB_LIB_SRC
	src/GDateTime.cpp
	src/GHashTable.cpp
	src/GList.cpp
	src/GMemory.cpp
	src/GQueue.cpp
	src/GString.cpp
	src/GTime.cpp
	include/fakeglib/GDateTime.h
	include/fakeglib/GHashTable.h
	include/fakeglib/GList.h
	include/fakeglib/GMemory.h
	include/fakeglib/GQueue.h
	include/fakeglib/GString.h
	include/fakeglib/GTime.h
	include/glib.h
)

if(FAKEGLIB_BUILD_SHARED)
	add_library(fakeglib SHARED ${FAKEGLIB_LIB_SRC})
	set_property(TARGET fakeglib PROPERTY CXX_STANDARD 11)
	
	generate_export_header(fakeglib
		EXPORT_MACRO_NAME FAKEGLIB_API
		EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/include/fakeglib/api.h
		DEPRECATED_MACRO_NAME FAKEGLIB_DEPRECATED
		NO_EXPORT_MACRO_NAME FAKEGLIB_NO_EXPORT
		STATIC_DEFINE FAKEGLIB_STATIC
		NO_DEPRECATED_MACRO_NAME FAKEGLIB_NO_DEPRECATED
	)
	
	target_include_directories(fakeglib
		PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
	    PUBLIC $<INSTALL_INTERFACE:include>
	    PRIVATE src)
endif()

if(FAKEGLIB_BUILD_STATIC)
	add_library(fakeglibstatic STATIC ${FAKEGLIB_LIB_SRC})
	set_property(TARGET fakeglibstatic PROPERTY CXX_STANDARD 11)
	target_compile_definitions(fakeglibstatic PUBLIC FAKEGLIB_STATIC)
	
	generate_export_header(fakeglibstatic
		EXPORT_MACRO_NAME FAKEGLIB_API
		EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/include/fakeglib/api.h
		DEPRECATED_MACRO_NAME FAKEGLIB_DEPRECATED
		NO_EXPORT_MACRO_NAME FAKEGLIB_NO_EXPORT
		STATIC_DEFINE FAKEGLIB_STATIC
		NO_DEPRECATED_MACRO_NAME FAKEGLIB_NO_DEPRECATED
	)
	
	target_include_directories(fakeglibstatic
		PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
	    PUBLIC $<INSTALL_INTERFACE:include>
	    PRIVATE src)
endif()
