add_library(webserver webserver.cpp)
target_include_directories(webserver PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(webserver
  PUBLIC
  pico_stdlib
  wifi
)
