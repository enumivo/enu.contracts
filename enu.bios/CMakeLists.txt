add_executable(enu.bios.wasm ${CMAKE_CURRENT_SOURCE_DIR}/src/enu.bios.cpp)
target_include_directories(enu.bios.wasm
   PUBLIC 
   ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(enu.bios.wasm
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/abi/enu.bios.abi" "${CMAKE_CURRENT_BINARY_DIR}" COPYONLY)
