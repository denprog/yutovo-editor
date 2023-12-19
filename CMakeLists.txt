cmake_minimum_required(VERSION 3.17)

project(yutovo_editor
    VERSION 0.1.0 
    LANGUAGES CXX
)

add_compile_definitions(DEBUG)

set(INSTALL_PATH "$ENV{YUTOVO_DEPLOY}")
set(THIRD_PARTY_PATH "$ENV{YUTOVO_DEPLOY}/../third_party/")

add_subdirectory(src)
if(NOT DEFINED EMSCRIPTEN)
    add_subdirectory(test)
endif()
