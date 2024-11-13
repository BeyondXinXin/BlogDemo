

project(SingleApplication LANGUAGES CXX)



add_library(${PROJECT_NAME} STATIC
    singleapplication.cpp
    singleapplication_p.cpp
)


# Find dependencies
set(QT_COMPONENTS Core Network)
set(QT_LIBRARIES Qt5::Core Qt5::Network)
set(QAPPLICATION_CLASS QApplication)

list(APPEND QT_COMPONENTS Widgets)
list(APPEND QT_LIBRARIES Qt5::Widgets)

find_package(Qt5 COMPONENTS ${QT_COMPONENTS} REQUIRED)

target_link_libraries(${PROJECT_NAME} PUBLIC ${QT_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE advapi32)
target_compile_definitions(${PROJECT_NAME} PUBLIC QAPPLICATION_CLASS=${QAPPLICATION_CLASS})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
