cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

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

target_link_libraries(${PROJECT_NAME} PRIVATE 
    stdc++
    stdc++fs
    pthread
    boost_serialization
    zmq
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ../../third_party
)
