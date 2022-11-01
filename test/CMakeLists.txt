cmake_minimum_required(VERSION 3.17)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIR})

add_executable(${PROJECT_NAME}_tests
	main.cpp
	strings.cpp
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
)

target_include_directories(${PROJECT_NAME}_tests PRIVATE
    ../src
    ../third_party
)

gtest_discover_tests(${PROJECT_NAME}_tests)
