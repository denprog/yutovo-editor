cmake_minimum_required(VERSION 3.17)

project(yutovo_editor
    VERSION 0.1.0 
    LANGUAGES CXX
)

add_compile_definitions(DEBUG)

set(INSTALL_PATH "$ENV{YUTOVO_DEPLOY}")

add_subdirectory(src)
if(NOT DEFINED EMSCRIPTEN)
    add_subdirectory(test)
endif()
