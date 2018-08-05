configure_file("${CMAKE_CURRENT_SOURCE_DIR}/abi/enu.msig.abi" "${CMAKE_CURRENT_BINARY_DIR}" COPYONLY)

add_executable(enu.msig.wasm ${CMAKE_CURRENT_SOURCE_DIR}/src/enu.msig.cpp)
target_include_directories(enu.msig.wasm
   PUBLIC 
   ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(enu.msig.wasm
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

#install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION ${WASM_ROOT}/enumivo.wasmsdk/include)
