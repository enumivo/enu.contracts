cmake_minimum_required( VERSION 3.5 )

set(ENUMIVO_VERSION_MIN "1.4")
set(ENUMIVO_VERSION_SOFT_MAX "1.4")
#set(ENUMIVO_VERSION_HARD_MAX "")

find_package(enumivo)

### Check the version of enumivo
set(VERSION_MATCH_ERROR_MSG "")
ENUMIVO_CHECK_VERSION(VERSION_OUTPUT "${ENUMIVO_VERSION}"
                                   "${ENUMIVO_VERSION_MIN}"
                                   "${ENUMIVO_VERSION_SOFT_MAX}"
                                   "${ENUMIVO_VERSION_HARD_MAX}"
                                   VERSION_MATCH_ERROR_MSG)
if(VERSION_OUTPUT STREQUAL "MATCH")
   message(STATUS "Using enumivo version ${ENUMIVO_VERSION}")
elseif(VERSION_OUTPUT STREQUAL "WARN")
   message(WARNING "Using enumivo version ${ENUMIVO_VERSION} even though it exceeds the maximum supported version of ${ENUMIVO_VERSION_SOFT_MAX}; continuing with configuration, however build may fail.\nIt is recommended to use enumivo version ${ENUMIVO_VERSION_SOFT_MAX}.x")
else() # INVALID OR MISMATCH
   message(FATAL_ERROR "Found enumivo version ${ENUMIVO_VERSION} but it does not satisfy version requirements: ${VERSION_MATCH_ERROR_MSG}\nPlease use enumivo version ${ENUMIVO_VERSION_SOFT_MAX}.x")
endif(VERSION_OUTPUT STREQUAL "MATCH")


enable_testing()

configure_file(${CMAKE_SOURCE_DIR}/contracts.hpp.in ${CMAKE_BINARY_DIR}/contracts.hpp)

include_directories(${CMAKE_BINARY_DIR})

file(GLOB UNIT_TESTS "*.cpp" "*.hpp")

add_enumivo_test( unit_test ${UNIT_TESTS} )
