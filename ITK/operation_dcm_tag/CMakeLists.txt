GENERATE_PROGECT(operation_dcm_tag)

add_executable(${PROJECT_NAME} WIN32 ${operation_dcm_tag_SRCS})

target_link_libraries(${PROJECT_NAME} Qt5::Gui Qt5::Widgets)
target_link_libraries(${PROJECT_NAME} ${ITK_LIBRARIES})

