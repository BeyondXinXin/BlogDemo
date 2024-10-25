cmake_minimum_required(VERSION 3.1.0)
project(CodingTranslate)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)
add_definitions(-DQT_MESSAGELOGCONTEXT)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin/Release)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)


add_subdirectory(libs)
add_subdirectory(src)
