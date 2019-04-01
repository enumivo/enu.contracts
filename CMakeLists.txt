cmake_minimum_required(VERSION 3.5)
<<<<<<< HEAD
project(enumivo_contracts VERSION 1.5.2)

set(ENUMIVO_CDT_VERSION_MIN "1.4")
set(ENUMIVO_CDT_VERSION_SOFT_MAX "1.5")
#set(ENUMIVO_CDT_VERSION_HARD_MAX "")
=======

project(eosio_contracts)

set(VERSION_MAJOR 1)
set(VERSION_MINOR 6)
set(VERSION_PATCH 0)
#set(VERSION_SUFFIX rc3)

if (VERSION_SUFFIX)
   set(VERSION_FULL "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}-${VERSION_SUFFIX}")
else()
   set(VERSION_FULL "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
endif()

include(ExternalProject)
>>>>>>> upstream/master

find_package(enumivo.cdt)

<<<<<<< HEAD
### Check the version of enumivo.cdt
=======
message(STATUS "Building eosio.contracts v${VERSION_FULL}")

set(EOSIO_CDT_VERSION_MIN "1.5")
set(EOSIO_CDT_VERSION_SOFT_MAX "1.5")
#set(EOSIO_CDT_VERSION_HARD_MAX "")

### Check the version of eosio.cdt
>>>>>>> upstream/master
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

ExternalProject_Add(
   contracts_project
   SOURCE_DIR ${CMAKE_SOURCE_DIR}/contracts
   BINARY_DIR ${CMAKE_BINARY_DIR}/contracts
   CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${EOSIO_CDT_ROOT}/lib/cmake/eosio.cdt/EosioWasmToolchain.cmake
   UPDATE_COMMAND ""
   PATCH_COMMAND ""
   TEST_COMMAND ""
   INSTALL_COMMAND ""
   BUILD_ALWAYS 1
)

<<<<<<< HEAD
add_subdirectory(enu.bios)
add_subdirectory(enu.msig)
add_subdirectory(enu.wrap)
add_subdirectory(enu.system)
add_subdirectory(enu.token)
=======
if (APPLE)
   set(OPENSSL_ROOT "/usr/local/opt/openssl")
elseif (UNIX)
   set(OPENSSL_ROOT "/usr/include/openssl")
endif()
set(SECP256K1_ROOT "/usr/local")
>>>>>>> upstream/master

if (APPLE)
   set(OPENSSL_ROOT "/usr/local/opt/openssl")
elseif (UNIX)
   set(OPENSSL_ROOT "/usr/include/openssl")
endif()
set(SECP256K1_ROOT "/usr/local")

string(REPLACE ";" "|" TEST_FRAMEWORK_PATH "${CMAKE_FRAMEWORK_PATH}")
string(REPLACE ";" "|" TEST_MODULE_PATH "${CMAKE_MODULE_PATH}")

ExternalProject_Add(
  contracts_unit_tests
  LIST_SEPARATOR | # Use the alternate list separator
  CMAKE_ARGS -DCMAKE_BUILD_TYPE=${TEST_BUILD_TYPE} -DCMAKE_FRAMEWORK_PATH=${TEST_FRAMEWORK_PATH} -DCMAKE_MODULE_PATH=${TEST_MODULE_PATH} -DEOSIO_ROOT=${EOSIO_ROOT} -DLLVM_DIR=${LLVM_DIR}
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/tests
  BINARY_DIR ${CMAKE_BINARY_DIR}/tests
  BUILD_ALWAYS 1
  TEST_COMMAND   ""
  INSTALL_COMMAND ""
)
