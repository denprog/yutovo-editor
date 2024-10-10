cmake_minimum_required(VERSION 3.17)

project(yutovo_editor
    VERSION 0.1.0 
    LANGUAGES CXX
)

add_compile_definitions(DEBUG)

set(INSTALL_PATH "$ENV{YUTOVO_DEPLOY}")
set(THIRD_PARTY_PATH "$ENV{YUTOVO_DEPLOY}/../third_party/")

# set(REMOTE_SOLVER true)
# add_compile_definitions(REMOTE_SOLVER)

add_subdirectory(src)
if((NOT DEFINED EMSCRIPTEN) AND (NOT DEFINED no-test))
    add_subdirectory(test)
endif()
