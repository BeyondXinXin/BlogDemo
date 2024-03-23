include_directories("editor")
include_directories("palette")
include_directories("tool")
include_directories("data")



file(GLOB_RECURSE SOURCES "*.cpp" "*.cxx" "*.cc")
file(GLOB_RECURSE RESOURCES "*.qrc")
file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE FORMS *.ui)
set(SRCS ${SOURCES} ${HEADERS} ${RESOURCES} ${FORMS})
source_group("" FILES ${SRCS})

set(Tadpole_Resource_DIR ${RESOURCES} CACHE PATH "Tadpole Resource Resource")

add_library(Tadpole STATIC ${SRCS})


target_link_libraries(
    Tadpole
    Qt5::Gui
    Qt5::Widgets
    Qt5::Sql
    Qt5::Network
    Qt5::Concurrent
    )

