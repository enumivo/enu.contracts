cmake_minimum_required( VERSION 3.5 )

project(contracts)

set(ENUMIVO_WASM_OLD_BEHAVIOR "Off")
find_package(enumivo.cdt)

add_subdirectory(enu.bios)
add_subdirectory(enu.msig)
add_subdirectory(enu.system)
add_subdirectory(enu.token)
add_subdirectory(enu.wrap)
