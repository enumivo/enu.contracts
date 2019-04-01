add_contract(enu.msig enu.msig ${CMAKE_CURRENT_SOURCE_DIR}/src/enu.msig.cpp)

target_include_directories(enu.msig
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(enu.msig
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
