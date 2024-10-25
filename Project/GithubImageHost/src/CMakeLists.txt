GENERATE_PROGECT(GitHubImageHost)

add_executable(${PROJECT_NAME} WIN32 ${GitHubImageHost_SRCS})

target_link_libraries(${PROJECT_NAME} Qt5::Gui Qt5::Widgets Qt5::Network Qt5::Concurrent)
