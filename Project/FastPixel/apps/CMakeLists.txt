
file(GLOB_RECURSE SOURCES "*.cpp" "*.cxx" "*.cc")
file(GLOB_RECURSE RESOURCES "*.qrc")
file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE FORMS *.ui)
set(SRCS ${SOURCES} ${HEADERS} ${RESOURCES} ${FORMS})
source_group("" FILES ${SRCS})


add_executable(${PROJECT_NAME} WIN32 ${SRCS}  ${Tadpole_Resource_DIR})

target_link_libraries(
    ${PROJECT_NAME}
    Qt5::Gui
    Qt5::Widgets
    Qt5::Sql
    Qt5::Network
    Qt5::Concurrent
    )

target_link_libraries(${PROJECT_NAME} Tadpole)
