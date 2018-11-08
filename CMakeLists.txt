cmake_minimum_required(VERSION 3.5)
project(enumivo_contracts VERSION 1.5.0)

set(ENUMIVO_CDT_VERSION_MIN "1.4")
set(ENUMIVO_CDT_VERSION_SOFT_MAX "1.4")
#set(ENUMIVO_CDT_VERSION_HARD_MAX "")

find_package(enumivo.cdt)

### Check the version of enumivo.cdt
set(VERSION_MATCH_ERROR_MSG "")
ENUMIVO_CHECK_VERSION(VERSION_OUTPUT "${ENUMIVO_CDT_VERSION}"
                                   "${ENUMIVO_CDT_VERSION_MIN}"
                                   "${ENUMIVO_CDT_VERSION_SOFT_MAX}"
                                   "${ENUMIVO_CDT_VERSION_HARD_MAX}"
                                   VERSION_MATCH_ERROR_MSG)
if(VERSION_OUTPUT STREQUAL "MATCH")
   message(STATUS "Using enumivo.cdt version ${ENUMIVO_CDT_VERSION}")
elseif(VERSION_OUTPUT STREQUAL "WARN")
   message(WARNING "Using enumivo.cdt version ${ENUMIVO_CDT_VERSION} even though it exceeds the maximum supported version of ${ENUMIVO_CDT_VERSION_SOFT_MAX}; continuing with configuration, however build may fail.\nIt is recommended to use enumivo.cdt version ${ENUMIVO_CDT_VERSION_SOFT_MAX}.x")
else() # INVALID OR MISMATCH
   message(FATAL_ERROR "Found enumivo.cdt version ${ENUMIVO_CDT_VERSION} but it does not satisfy version requirements: ${VERSION_MATCH_ERROR_MSG}\nPlease use enumivo.cdt version ${ENUMIVO_CDT_VERSION_SOFT_MAX}.x")
endif(VERSION_OUTPUT STREQUAL "MATCH")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
   set(TEST_BUILD_TYPE "Debug")
   set(CMAKE_BUILD_TYPE "Release")
else()
   set(TEST_BUILD_TYPE ${CMAKE_BUILD_TYPE})
endif()


add_subdirectory(enu.bios)
add_subdirectory(enu.msig)
add_subdirectory(enu.wrap)
add_subdirectory(enu.system)
add_subdirectory(enu.token)

if (APPLE)
   set(OPENSSL_ROOT "/usr/local/opt/openssl")
elseif (UNIX)
   set(OPENSSL_ROOT "/usr/include/openssl")
endif()
set(SECP256K1_ROOT "/usr/local")

include(UnitTestsExternalProject.txt)
