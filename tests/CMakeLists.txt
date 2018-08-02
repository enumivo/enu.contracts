cmake_minimum_required( VERSION 3.5 )

list(APPEND CMAKE_MODULE_PATH ${ENUMIVO_ROOT}/lib/cmake)
include(EnumivoTester)

### check the version of Enumivo
string(FIND "${ENUMIVO_VERSION}" "${ENUMIVO_DEPENDENCY}" output)
if (NOT "${output}" EQUAL 0)
   message(FATAL_ERROR "Incorrect Enumivo version, please use version ${ENUMIVO_DEPENDENCY}.x")
endif()


enable_testing()

configure_file(${CMAKE_SOURCE_DIR}/contracts.hpp.in ${CMAKE_BINARY_DIR}/contracts.hpp)

include_directories(${CMAKE_BINARY_DIR})

file(GLOB UNIT_TESTS "*.cpp" "*.hpp")

add_enumivo_test( unit_test ${UNIT_TESTS} )
