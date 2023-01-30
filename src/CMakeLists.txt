cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(yutovo_service REQUIRED PATHS ${INSTALL_PATH}/cmake/)

add_library(${PROJECT_NAME}
    caret.cpp
    caret_state.cpp
    selection.cpp
    editor_state.cpp
    task.cpp
    document.cpp
    element.cpp
    block.cpp
    page.cpp
    paragraph.cpp
    row.cpp
    str.cpp
    style.cpp
    text.cpp
    window.cpp
    util.cpp
    logger.cpp
    result_codes.cpp
    solver.cpp
    solver_task.cpp
    config.cpp
    formulas/formula.cpp
    formulas/code_block.cpp
    formulas/code_paragraph.cpp
    formulas/code_string.cpp
    formulas/code_row.cpp
    formulas/shape.cpp
    formulas/only_shape_formula.cpp
    formulas/middle_shape_formula.cpp
    formulas/plus.cpp
    formulas/minus.cpp
    formulas/multiply.cpp
    formulas/division.cpp
    formulas/power.cpp
    formulas/nth_root.cpp
    formulas/square_root.cpp
    formulas/equation.cpp
    formulas/result.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
	POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    stdc++
    stdc++fs
    pthread
    boost_serialization
    zmq
)

target_include_directories(${PROJECT_NAME} PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    ${INSTALL_PATH}/include
)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<INSTALL_INTERFACE:${INSTALL_PATH}/include>
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ../third_party
)

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}
    DESTINATION ${INSTALL_PATH}/lib
    COMPONENT ${PROJECT_NAME}
)

install(
    FILES 
        document.h
        caret_state.h
        editor_state.h
        selection.h
        result_codes.h
        caret.h
        element.h
        text.h
        logger.h
        solver.h
        solver_task.h
        task.h
        window.h
        style.h
        util.h
        config.h
    DESTINATION ${INSTALL_PATH}/include/${PROJECT_NAME}
    COMPONENT ${PROJECT_NAME}
)

install(
    EXPORT ${PROJECT_NAME}
    DESTINATION ${INSTALL_PATH}/cmake/${PROJECT_NAME}
    COMPONENT ${PROJECT_NAME}
    FILE ${PROJECT_NAME}.cmake
)

install(
    FILES ${PROJECT_NAME}-config.cmake
    DESTINATION ${INSTALL_PATH}/cmake/${PROJECT_NAME}
    COMPONENT ${PROJECT_NAME}
)
