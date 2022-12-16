cmake_minimum_required(VERSION 3.17)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIR})

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(${PROJECT_NAME}_tests
	main.cpp
	mock.cpp
	str.cpp
	paragraph.cpp
	caret.cpp
	files.cpp
	clipboard.cpp
	code.cpp
	division.cpp
)

target_link_libraries(${PROJECT_NAME}_tests PRIVATE
	stdc++
    pthread
	gtest
	gtest_main
	gmock
	gmock_main
	m
	dl
	${PROJECT_NAME}
    Qt5::Widgets
)

target_include_directories(${PROJECT_NAME}_tests PRIVATE
    ../src
    ../third_party
)

gtest_discover_tests(${PROJECT_NAME}_tests)
