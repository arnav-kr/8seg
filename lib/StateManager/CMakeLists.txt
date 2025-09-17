add_library(statemanager state_manager.cpp)

target_include_directories(statemanager
  PUBLIC
  include
)

target_link_libraries(statemanager
  pico_stdlib
  fs
)
