cmake_minimum_required(VERSION 3.1.0)

include(cmake/Generate.cmake)
include(cmake/clean-all.cmake)

project(BlogDemo_ITK)

list(APPEND CMAKE_PREFIX_PATH "D:/Lib_Src/ITK-5.4rc01/build")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/bin/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin/Release)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(BUILD_SHARED_LIBS "" ON)


find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Sql Test Xml SerialPort Concurrent Network Multimedia SerialPort)
add_definitions(-DQT_MESSAGELOGCONTEXT)
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

add_subdirectory(operation_dcm_tag)
add_subdirectory(imgs_to_mha)

add_custom_target(clean-all
   COMMAND ${CMAKE_BUILD_TOOL} clean
   COMMAND ${CMAKE_COMMAND} -P clean-all.cmake
)

