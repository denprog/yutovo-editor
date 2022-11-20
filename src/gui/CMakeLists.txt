cmake_minimum_required(VERSION 3.17)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QT_SELECT qt5.14)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(${PROJECT_NAME}_gui
    main.cpp
    document_widget.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    resources.qrc
    qt_window.cpp
)

target_compile_options(${PROJECT_NAME}_gui PRIVATE -fPIC)

target_include_directories(${PROJECT_NAME}_gui PRIVATE
    ..
    ../../third_party
)

target_link_libraries(${PROJECT_NAME}_gui PRIVATE 
    ${PROJECT_NAME}
    Qt5::Widgets
)
