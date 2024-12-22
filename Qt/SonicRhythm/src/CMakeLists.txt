file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")

set(SRCS ${SOURCES} ${HEADERS})
source_group("" FILES ${SRCS})

add_executable(${PROJECT_NAME} WIN32 ${SRCS})

target_link_libraries(${PROJECT_NAME} 
    Qt::Charts
    Qt::Core
    Qt::Gui
    Qt::Multimedia
    kissfft
)

