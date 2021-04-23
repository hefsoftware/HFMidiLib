cmake_minimum_required(VERSION 3.5)

project(HFMidiLib LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Widgets REQUIRED)

if(UNIX AND NOT APPLE)
  set(LINUX TRUE)
endif()

# OS-specific sources
set (HFMidiLib-OSSources )
set (HFMidiLib-OSLink )

if(LINUX)
  list(APPEND HFMidiLib-OSSources
    HFMidiLib/OS/midilinux.h
    HFMidiLib/OS/midilinux.cpp
  )
elseif(WIN32)
  list(APPEND HFMidiLib-OSSources
    HFMidiLib/OS/midiwin.h
    HFMidiLib/OS/midiwin.cpp
  )
endif()


add_library(HFMidiLib STATIC
    HFMidiLib/port.h
    HFMidiLib/device.cpp
    HFMidiLib/device.h
    HFMidiLib/devicedescription.cpp
    HFMidiLib/devicedescription.h
    HFMidiLib/driver.cpp
    HFMidiLib/driver.h
    HFMidiLib/framework.cpp
    HFMidiLib/framework.h
    HFMidiLib/port.cpp
    ${HFMidiLib-OSSources}
)

target_link_libraries(HFMidiLib PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets)
if(LINUX)
elseif(WIN32)
  target_link_libraries(HFMidiLib PUBLIC winmm)
endif()


target_include_directories(HFMidiLib PUBLIC . )
target_compile_definitions(HFMidiLib PRIVATE HFMIDICMAKE_LIBRARY)
