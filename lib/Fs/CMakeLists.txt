add_library(fs STATIC
  fs.cpp
)

target_include_directories(fs
  PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_libraries(fs
  pico_stdlib
  hardware_flash
  hardware_sync
)
