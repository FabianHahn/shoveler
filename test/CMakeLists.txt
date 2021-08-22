set(TEST_SRC
	GArrayTest.cpp
	GDateTimeTest.cpp
	GHashTableTest.cpp
	GListTest.cpp
	GQueueTest.cpp
	GStringTest.cpp
	GStringUtilTest.cpp
	GTime.cpp
	main.cpp
)

if(FAKEGLIB_BUILD_STATIC)
	add_executable(fakeglibtest ${TEST_SRC})
	target_link_libraries(fakeglibtest fakeglib::fakeglibstatic GTest::gtest GTest::gmock)
	set_property(TARGET fakeglibtest PROPERTY CXX_STANDARD 11)

	add_test(fakeglibtest fakeglibtest)
endif()

if(GLIB_FOUND)
	add_executable(glibtest ${TEST_SRC})
	target_link_libraries(glibtest glib::glib GTest::gtest GTest::gmock)
	set_property(TARGET glibtest PROPERTY CXX_STANDARD 11)

	add_test(glibtest glibtest)
endif()
