cmake_minimum_required(VERSION 3.5)

project(demo LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

set(PROJECT_SOURCES
    src/Icons.qrc
    src/input.png
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/mainwindow.ui
    src/mididevicestree.cpp
    src/mididevicestree.h
    src/midieventlistwidget.cpp
    src/midieventlistwidget.h
    src/miditestsource.cpp
    src/miditestsource.h
    src/miditestsource.ui
    src/MyMidi.pro
    src/output.png
    src/qlabeldroptarget.cpp
    src/qlabeldroptarget.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(HFMidiLibDemo
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(HFMidiLibDemo SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(HFMidiLibDemo
            ${PROJECT_SOURCES}
        )
    endif()
endif()

add_subdirectory(../ HFMidiLib)
target_link_libraries(HFMidiLibDemo PRIVATE HFMidiLib)
target_include_directories(HFMidiLibDemo PRIVATE . )
target_link_libraries(HFMidiLibDemo PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
