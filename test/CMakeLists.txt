cmake_minimum_required(VERSION 3.17)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GTest_DIR "/usr/lib/x86_64-linux-gnu/cmake/GTest")

enable_testing()
find_package(GTest REQUIRED)

if(EMSCRIPTEN)
    set(CMAKE_FIND_ROOT_PATH ${INSTALL_PATH}/wasm/cmake/)
else()
    set(CMAKE_FIND_ROOT_PATH ${INSTALL_PATH}/lib/cmake/)
endif()

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(yutovo_logger REQUIRED PATHS ${INSTALL_PATH}/lib/cmake/)

if(NOT DEFINED REMOTE_SOLVER)
    find_package(yutovo_solver REQUIRED)
endif()

add_executable(${PROJECT_NAME}_tests
	main.cpp
	mock.cpp
	str.cpp
	link.cpp
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
	solver_complex.cpp
	assignment.cpp
	unit.cpp
	functions.cpp
	subscript.cpp
	variables.cpp
	image.cpp
	sum.cpp
	product.cpp
)

target_link_libraries(${PROJECT_NAME}_tests PUBLIC
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
    yutovo_logger
)

target_include_directories(${PROJECT_NAME}_tests PRIVATE
    ../src
    ${THIRD_PARTY_PATH}
)

gtest_discover_tests(${PROJECT_NAME}_tests)

install(
    TARGETS ${PROJECT_NAME}_tests
    EXPORT ${PROJECT_NAME}
    DESTINATION ${INSTALL_PATH}/bin
    COMPONENT ${PROJECT_NAME}
)
