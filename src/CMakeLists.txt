include(GenerateExportHeader)

set(FAKEGLIB_SRC
	GDateTime.cpp
	GHashTable.cpp
	GList.cpp
	GMemory.cpp
	GQueue.cpp
	GString.cpp
)

add_library(fakeglib SHARED ${FAKEGLIB_SRC})
generate_export_header(fakeglib
	EXPORT_MACRO_NAME FAKEGLIB_API
	EXPORT_FILE_NAME api.h
	DEPRECATED_MACRO_NAME FAKEGLIB_DEPRECATED
	NO_EXPORT_MACRO_NAME FAKEGLIB_NO_EXPORT
	STATIC_DEFINE FAKEGLIB_STATIC
	NO_DEPRECATED_MACRO_NAME FAKEGLIB_NO_DEPRECATED
)
target_include_directories(fakeglib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})  
set_property(TARGET fakeglib PROPERTY CXX_STANDARD 11)

add_library(fakeglibstatic STATIC ${FAKEGLIB_SRC})
target_include_directories(fakeglibstatic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})  
target_compile_definitions(fakeglibstatic PUBLIC FAKEGLIB_STATIC)
set_property(TARGET fakeglibstatic PROPERTY CXX_STANDARD 11)