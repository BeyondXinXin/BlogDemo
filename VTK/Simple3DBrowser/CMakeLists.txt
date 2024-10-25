cmake_minimum_required(VERSION 3.16.0)
project(Simple3DBrowser LANGUAGES CXX)
option(Simple3D_EXAMPLES "Build examples" OFF)

if(Simple3D_EXAMPLES)
	set(CMAKE_INCLUDE_CURRENT_DIR ON)
	set(CMAKE_AUTOMOC ON)
	set(CMAKE_AUTOUIC ON)
	set(CMAKE_AUTORCC ON)
	set(CMAKE_CXX_STANDARD 17)
	set(BUILD_SHARED_LIBS "" ON)

	set(BINDIR bin)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${BINDIR})
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Concurrent)
find_package(VTK 9.0 REQUIRED)
include_directories(${VTK_INCLUDE_DIRS})

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.cxx" "src/*.cc")
file(GLOB_RECURSE HEADERS "src/*.h")
file(GLOB_RECURSE FORMS *.ui)
file(GLOB_RECURSE RESOURCES "*.qrc" "*.rc")
list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

set(SRCS ${SOURCES} ${HEADERS} ${RESOURCES} ${FORMS})
source_group("" FILES ${SRCS})

add_library(Simple3DBrowser STATIC ${SRCS})

target_include_directories(Simple3DBrowser
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

vtk_module_autoinit(TARGETS Simple3DBrowser MODULES ${VTK_LIBRARIES})

target_link_libraries(
	Simple3DBrowser
	Qt6::Widgets
	Qt6::Concurrent
	${VTK_LIBRARIES}
)

if(Simple3D_EXAMPLES)
	add_executable(examples WIN32 src/main.cpp)
	target_link_libraries(examples Simple3DBrowser)
endif()

if(CMAKE_GENERATOR STREQUAL "Ninja")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
endif()
