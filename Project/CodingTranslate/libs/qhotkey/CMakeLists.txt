cmake_minimum_required(VERSION 3.1)

project(qhotkey VERSION 1.2.2 LANGUAGES CXX)

#set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

qt5_wrap_cpp(MOC_HEADERS
    QHotkey/qhotkey.h
    QHotkey/qhotkey_p.h)

set(LIBS
    Qt5::Core
    Qt5::Widgets)

set(SRC_FILES
    QHotkey/qhotkey.cpp)

set(SRC_FILES ${SRC_FILES} QHotkey/qhotkey_win.cpp)

add_library(qhotkey ${SRC_FILES} ${MOC_HEADERS})
target_link_libraries(qhotkey ${LIBS})

target_include_directories(qhotkey
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/QHotkey>
    $<INSTALL_INTERFACE:include>)

set_target_properties(qhotkey PROPERTIES
    SOVERSION ${PROJECT_VERSION_MAJOR}
    VERSION ${PROJECT_VERSION})

