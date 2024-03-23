set(CMAKE_INCLUDE_CURRENT_DIR ON)

file(GLOB_RECURSE SOURCES "*.cpp" "*.cxx" "*.cc")
file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE RESOURCES "*.qrc")
file(GLOB_RECURSE FORMS *.ui)
source_group("" FILES ${SOURCES} ${HEADERS} ${RESOURCES} ${FORMS})
set(SRCS ${SOURCES} ${HEADERS} ${RESOURCES} ${FORMS})

add_executable(${PROJECT_NAME} WIN32 ${SRCS})

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets)
target_link_libraries(${PROJECT_NAME} SingleApplication)
target_link_libraries(${PROJECT_NAME} qhotkey)
target_link_libraries(${PROJECT_NAME} QOnlineTranslator)

