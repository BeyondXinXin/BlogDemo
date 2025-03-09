GENERATE_PROGECT(ProtocalPackage)

add_executable(${PROJECT_NAME} WIN32 ${ProtocalPackage_SRCS})

target_link_libraries(${PROJECT_NAME} Qt5::Gui Qt5::Widgets)
