set(TEST_SRC
	GDateTimeTest.cpp
	GHashTableTest.cpp
	GListTest.cpp
	GQueueTest.cpp
	GStringTest.cpp
	main.cpp
)

if(FAKEGLIB_BUILD_STATIC)
	add_executable(fakeglibtest ${TEST_SRC})
	target_link_libraries(fakeglibtest fakeglibstatic gtest)
	target_include_directories(fakeglibtest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
	set_property(TARGET fakeglibtest PROPERTY CXX_STANDARD 11)

	add_test(fakeglibtest fakeglibtest)
endif()

if(GLIB_FOUND)
	add_executable(glibtest ${TEST_SRC})
	target_link_libraries(glibtest glib gtest)
	target_include_directories(glibtest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
	set_property(TARGET glibtest PROPERTY CXX_STANDARD 11)

	add_test(glibtest glibtest)
endif()
