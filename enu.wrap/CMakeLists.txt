add_contract(enu.wrap enu.wrap ${CMAKE_CURRENT_SOURCE_DIR}/src/enu.wrap.cpp)
target_include_directories(enu.wrap.wasm
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(enu.wrap.wasm
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
