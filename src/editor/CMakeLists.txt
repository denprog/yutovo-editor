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
    formulas/formula.cpp
    formulas/code.cpp
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
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    stdc++
    stdc++fs
    pthread
    boost_serialization
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ../../third_party
)
