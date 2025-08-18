cmake_minimum_required(VERSION 3.17)

project(yutovo-editor
    VERSION 0.1.0 
    LANGUAGES CXX
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
else()
    add_compile_definitions(RELEASE)
endif()

if ((NOT DEFINED no-test) AND (NOT DEFINED EMSCRIPTEN))
    add_compile_definitions(TEST)
endif()

if(WIN32)
    set(INSTALL_PATH "c:/Lang/vcpkg/installed/x64-windows")
    set(PKG_CONFIG_PATH "c:/Lang/vcpkg/installed/x64-windows/lib/pkgconfig")
else()
    set(INSTALL_PATH "$ENV{YUTOVO_DEPLOY}")
    set(THIRD_PARTY_PATH "$ENV{YUTOVO_DEPLOY}/../third_party/")
endif()

# set(REMOTE_SOLVER true)
# add_compile_definitions(REMOTE_SOLVER)

add_subdirectory(src)
if ((NOT DEFINED EMSCRIPTEN) AND (NOT DEFINED no-test))
    add_subdirectory(test)
endif()
