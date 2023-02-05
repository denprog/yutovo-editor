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
	plus.cpp
	minus.cpp
	multiply.cpp
	division.cpp
	power.cpp
	nth_root.cpp
	square_root.cpp
	fences.cpp
	formulas.cpp
	solver_auto.cpp
	solver_real.cpp
	solver_integer.cpp
	solver_rational.cpp
	assignment.cpp
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

install(
    TARGETS ${PROJECT_NAME}_tests
    EXPORT ${PROJECT_NAME}
    DESTINATION ${INSTALL_PATH}/bin
    COMPONENT ${PROJECT_NAME}
)
