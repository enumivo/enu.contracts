add_executable(enu.system.wasm ${CMAKE_CURRENT_SOURCE_DIR}/src/enu.system.cpp)
target_include_directories(enu.system.wasm
   PUBLIC 
   ${CMAKE_CURRENT_SOURCE_DIR}/include
   ${CMAKE_CURRENT_SOURCE_DIR}/../enu.token/include)

set_target_properties(enu.system.wasm
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/abi/enu.system.abi" "${CMAKE_CURRENT_BINARY_DIR}" COPYONLY)

#install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION ${WASM_ROOT}/enu.wasmsdk/include)
