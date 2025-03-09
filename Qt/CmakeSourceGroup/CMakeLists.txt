project(CmakeSourceGroup)

file(GLOB_RECURSE SOURCES "*.cpp" "*.cxx" "*.cc")
file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE RESOURCES "*.qrc")
file(GLOB_RECURSE FORMS *.ui)

set(SRCS ${SOURCES} ${HEADERS} ${RESOURCES} ${FORMS})

# 图一

# 图二
# source_group("" FILES ${SRCS})
# 图三
# source_group("SOURCES" FILES ${SRCS})
# 图四
# source_group(TREE ${CMAKE_SOURCE_DIR} ${SRCS})

if(WIN32)
  set(exec_flag WIN32)
elseif(MACOS)
  set(exec_flag MACOSX_BUNDLE)
else()
  set(exec_flag)
endif()

add_executable(${PROJECT_NAME} ${exec_flag} ${SRCS})

target_link_libraries(${PROJECT_NAME} Qt5::Gui Qt5::Widgets)
