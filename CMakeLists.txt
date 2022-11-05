cmake_minimum_required(VERSION 3.17)

project(yutovo 
    VERSION 0.1.0 
    LANGUAGES CXX
)

add_compile_definitions(DEBUG)

add_subdirectory(src)
add_subdirectory(test)
