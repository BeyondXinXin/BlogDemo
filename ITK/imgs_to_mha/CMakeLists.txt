GENERATE_PROGECT(imgs_to_mha)

add_executable(${PROJECT_NAME} WIN32 ${imgs_to_mha_SRCS})

target_link_libraries(${PROJECT_NAME} Qt5::Gui Qt5::Widgets)
target_link_libraries(${PROJECT_NAME} ${ITK_LIBRARIES})

