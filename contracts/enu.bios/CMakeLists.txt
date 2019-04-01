add_contract(enu.bios enu.bios ${CMAKE_CURRENT_SOURCE_DIR}/src/enu.bios.cpp)

target_include_directories(enu.bios
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(enu.bios
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
