cmake_minimum_required(VERSION 3.17)

set(CMAKE_DEBUG_POSTFIX d)

find_package(PkgConfig REQUIRED)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)

if(EMSCRIPTEN)
    set(CMAKE_FIND_ROOT_PATH ${INSTALL_PATH}/wasm/cmake/)
endif()

set(Boost_INCLUDE_DIR "${INSTALL_PATH}/deploy/include")
message("CMAKE_FIND_ROOT_PATH is ${CMAKE_FIND_ROOT_PATH}")
message("INSTALL_PATH is ${INSTALL_PATH}")

if(EMSCRIPTEN)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -s USE_PTHREADS=1 -s PTHREAD_POOL_SIZE=4")
endif()

add_library(${PROJECT_NAME}
    caret.cpp
    caret_state.cpp
    selection.cpp
    editor_state.cpp
    task.cpp
    document.cpp
    element.cpp
    block.cpp
    paragraph.cpp
    row.cpp
    str.cpp
    image.cpp
    style.cpp
    text.cpp
    window.cpp
    util.cpp
    result_codes.cpp
    solver.cpp
    solver_task.cpp
    web_socket.cpp
    parser_string.cpp
    undo.cpp
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
    formulas/fences.cpp
    formulas/assignment.cpp
    formulas/unit.cpp
    formulas/subscript.cpp
    formulas/exclamation.cpp
    formulas/and.cpp
    formulas/or.cpp
    formulas/xor.cpp
    formulas/percent.cpp
    formulas/code_column.cpp
    formulas/iteration.cpp
    formulas/sum.cpp
    formulas/product.cpp
    formulas/comma.cpp
)

target_compile_options(${PROJECT_NAME} PUBLIC -Wno-deprecated-declarations)
target_compile_options(${PROJECT_NAME} PUBLIC -Wno-unused-command-line-argument)
target_compile_options(${PROJECT_NAME} PUBLIC -Wno-ambiguous-reversed-operator)

set_target_properties(${PROJECT_NAME} PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
	POSITION_INDEPENDENT_CODE ON
    CMAKE_INCLUDE_CURRENT_DIR ON
)

if(EMSCRIPTEN)
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        stdc++
        pthread
    )
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        stdc++
        pthread
        ssl
        crypto
    )
endif()

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    ${INSTALL_PATH}/include
)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<INSTALL_INTERFACE:${INSTALL_PATH}/include>
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${THIRD_PARTY_PATH}
)

if(EMSCRIPTEN)
    install(
        TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}
        DESTINATION ${INSTALL_PATH}/wasm
        COMPONENT ${PROJECT_NAME}
    )
else()
    install(
        TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}
        DESTINATION ${INSTALL_PATH}/lib
        COMPONENT ${PROJECT_NAME}
    )
endif()

install(
    FILES 
        document.h
        caret_state.h
        editor_state.h
        selection.h
        result_codes.h
        caret.h
        element.h
        block.h
        text.h
        solver.h
        solver_task.h
        task.h
        window.h
        style.h
        util.h
        config.h
        web_socket.h
        parser_string.h
        undo.h
    DESTINATION ${INSTALL_PATH}/include/${PROJECT_NAME}
    COMPONENT ${PROJECT_NAME}
)

if(EMSCRIPTEN)
    install(
        EXPORT ${PROJECT_NAME}
        DESTINATION ${INSTALL_PATH}/wasm/cmake/${PROJECT_NAME}
        COMPONENT ${PROJECT_NAME}
        FILE ${PROJECT_NAME}.cmake
    )
else()
    install(
        EXPORT ${PROJECT_NAME}
        DESTINATION ${INSTALL_PATH}/lib/cmake/${PROJECT_NAME}
        COMPONENT ${PROJECT_NAME}
        FILE ${PROJECT_NAME}.cmake
    )
endif()

if(EMSCRIPTEN)
    install(
        FILES ${PROJECT_NAME}-config.cmake
        DESTINATION ${INSTALL_PATH}/wasm/cmake/${PROJECT_NAME}
        COMPONENT ${PROJECT_NAME}
    )
else()
    install(
        FILES ${PROJECT_NAME}-config.cmake
        DESTINATION ${INSTALL_PATH}/lib/cmake/${PROJECT_NAME}
        COMPONENT ${PROJECT_NAME}
    )
endif()
