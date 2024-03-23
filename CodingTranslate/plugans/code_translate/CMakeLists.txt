project(code_translate)

file(GLOB_RECURSE SOURCES "*.cpp" "*.cxx" "*.cc")
file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE RESOURCES "*.qrc")
file(GLOB_RECURSE FORMS *.ui)
source_group("" FILES ${SOURCES} ${HEADERS} ${RESOURCES} ${FORMS})
set(SRCS ${SOURCES} ${HEADERS} ${RESOURCES} ${FORMS})

add_library(${PROJECT_NAME} SHARED ${SRCS})

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets)
target_link_libraries(${PROJECT_NAME} QOnlineTranslator)
target_link_libraries(${PROJECT_NAME} SingleApplication)
target_link_libraries(${PROJECT_NAME} qhotkey)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>)

