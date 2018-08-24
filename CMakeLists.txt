cmake_minimum_required(VERSION 3.5)
project(enumivo_contracts VERSION 1.2.1)

set(ENUMIVO_DEPENDENCY "1.2")
set(ENUMIVO_WASMSDK_DEPENDENCY "1.1")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
   set(TEST_BUILD_TYPE "Debug")
   set(CMAKE_BUILD_TYPE "Release")
else()
   set(TEST_BUILD_TYPE ${CMAKE_BUILD_TYPE})
endif()

if(ENUMIVO_ROOT STREQUAL "" OR NOT ENUMIVO_ROOT)
   set(ENUMIVO_ROOT "/usr/local/enumivo")
endif()

if(ENUMIVO_WASMSDK_ROOT STREQUAL "" OR NOT ENUMIVO_WASMSDK_ROOT)
   set(ENUMIVO_WASMSDK_ROOT "/usr/local/enumivo.wasmsdk")
endif()

list(APPEND CMAKE_MODULE_PATH ${ENUMIVO_WASMSDK_ROOT}/lib/cmake)
include(EnumivoWasmToolchain)

### Check the version of wasmsdk
string(FIND "${ENUMIVO_WASMSDK_VERSION}" "${ENUMIVO_WASMSDK_DEPENDENCY}" output)

if (NOT "${output}" EQUAL 0)
   message(FATAL_ERROR "Incorrect Enumivo.WasmSDK version, please use version ${ENUMIVO_WASMSDK_DEPENDENCY}.x")
endif()

include_directories(AFTER ${BOOST_ROOT}/include)
add_subdirectory(enu.msig)
add_subdirectory(enu.sudo)
add_subdirectory(enu.system)
add_subdirectory(enu.token)

if (APPLE)
   set(OPENSSL_ROOT "/usr/local/opt/openssl")
elseif (UNIX)
   set(OPENSSL_ROOT "/usr/include/openssl")
endif()
set(SECP256K1_ROOT "/usr/local")

include(UnitTestsExternalProject.txt)
