add_executable(enu.sudo.wasm ${CMAKE_CURRENT_SOURCE_DIR}/src/enu.sudo.cpp)
target_include_directories(enu.sudo.wasm
   PUBLIC 
   ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(enu.sudo.wasm
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/abi/enu.sudo.abi" "${CMAKE__CURRENT_BINARY_DIR}" COPYONLY)
#install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION ${WASM_ROOT}/enumivo.wasmsdk/include)
