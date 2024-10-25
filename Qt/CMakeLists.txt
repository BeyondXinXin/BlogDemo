cmake_minimum_required(VERSION 3.1.0)
project(QWidgetDemo)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(BUILD_SHARED_LIBS ON)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Concurrent)
find_package(Qt5LinguistTools)

macro(GENERATE_PROJECT PROJECT_NAME)
   project(${PROJECT_NAME})

   file(GLOB_RECURSE SOURCES "${PROJECT_NAME}/*.cpp" "${PROJECT_NAME}/*.cxx" "${PROJECT_NAME}/*.cc")
   file(GLOB_RECURSE HEADERS "${PROJECT_NAME}/*.h")
   file(GLOB_RECURSE FORMS "${PROJECT_NAME}/*.ui")
   file(GLOB_RECURSE RESOURCES "${PROJECT_NAME}/*.qrc" "${PROJECT_NAME}/*.rc")

   source_group("src" FILES ${HEADERS} ${SOURCES} ${FORMS} ${RESOURCES})

   set(SRCS ${SOURCES} ${HEADERS} ${FORMS} ${RESOURCES})

   add_executable(${PROJECT_NAME} WIN32 ${SRCS})

   target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME})
   target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Concurrent)
endmacro()

GENERATE_PROJECT(DisappearLabel)
GENERATE_PROJECT(DoubleSlider)
GENERATE_PROJECT(BullsEye)
GENERATE_PROJECT(PictureBlockSelection)