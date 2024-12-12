project(api_serve)
include_directories(${3rdpartyPath})

glob_headers_and_sources(API_SERVE_FILES ./)
add_executable(api_serve ${API_SERVE_FILES})
target_link_libraries(api_serve ${HV_LIBRARIES})
