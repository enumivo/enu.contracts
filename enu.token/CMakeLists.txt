add_executable(enu.token.wasm ${CMAKE_CURRENT_SOURCE_DIR}/src/enu.token.cpp)
target_include_directories(enu.token.wasm
   PUBLIC 
   ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(enu.token.wasm
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/abi/enu.token.abi" "${CMAKE_CURRENT_BINARY_DIR}" COPYONLY)
#install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION ${WASM_ROOT}/enumivo.wasmsdk/include)
