cmake_minimum_required(VERSION 3.17)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
	set(CMAKE_PREFIX_PATH "${INSTALL_PATH}")
else()
	set(GTest_DIR "/usr/lib/x86_64-linux-gnu/cmake/GTest")
endif()

enable_testing()
find_package(GTest REQUIRED PATHS ${INSTALL_PATH}/share/gtest)

if(WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8 /bigobj")
	find_package(PkgConfig)
	pkg_check_modules(mpfr REQUIRED IMPORTED_TARGET mpfr)
	find_package(ZLIB REQUIRED)
	find_package(yutovo_calculator REQUIRED PATHS ${INSTALL_PATH}/lib/cmake/)
	find_package(Boost 1.81.0 REQUIRED PATHS ${INSTALL_PATH}/lib/cmake/)
	find_package(RapidJSON CONFIG REQUIRED)
	find_package(Boost REQUIRED COMPONENTS iostreams)
endif()

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

find_package(ZLIB REQUIRED)

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
	brackets.cpp
	formulas.cpp
	solver_auto.cpp
	solver_real.cpp
	solver_integer.cpp
	solver_rational.cpp
	solver_complex.cpp
	solver_array_real.cpp
	assignment.cpp
	unit.cpp
	functions.cpp
	subscript.cpp
	variables.cpp
	image.cpp
	sum.cpp
	product.cpp
)

if(WIN32)
	target_link_libraries(${PROJECT_NAME}_tests PUBLIC
		${PROJECT_NAME}
	    Qt5::Widgets
		yutovo_solver
	    yutovo_logger
	)
	target_link_libraries(${PROJECT_NAME}_tests PRIVATE yutovo_calculator)
	target_link_libraries(${PROJECT_NAME} PRIVATE Boost::iostreams)
	target_link_libraries(${PROJECT_NAME}_tests PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
    target_include_directories(${PROJECT_NAME}_tests PRIVATE
        ../src
    	${INSTALL_PATH}/include
	)
else()
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
		libboost_iostreams.a
		z
	)
    target_include_directories(${PROJECT_NAME}_tests PRIVATE
        ../src
        ${THIRD_PARTY_PATH}
	)
endif()

if(NOT WIN32)
	include(GoogleTest)
	gtest_discover_tests(${PROJECT_NAME}_tests)
endif()

install(
    TARGETS ${PROJECT_NAME}_tests
    EXPORT ${PROJECT_NAME}
    DESTINATION ${INSTALL_PATH}/bin
    COMPONENT ${PROJECT_NAME}
)
