
cmake_minimum_required( VERSION 3.9.1 )

project( routing_int )

include_directories(${CMAKE_SOURCE_DIR}/include)

add_subdirectory(algorithm)
add_subdirectory(network)
add_subdirectory(process)

set(lsr_source_files dtlsr_main.c)
set(dtlsr_source_files dtlsr_main.c)
set(heartbeat_source_files heartbeat_main.c)
set(default_source_files default_route_main.c)

add_executable(lsr ${lsr_source_files})
add_executable(dtlsr ${dtlsr_source_files})
add_executable(heartbeat ${heartbeat_source_files})
add_executable(default ${default_source_files})

target_compile_definitions(dtlsr PRIVATE DTLSR)

# add_compile_options(-Wno-deprecated-declarations)
# SET(GCC_COVERAGE_LINK_FLAGS "-Wno-deprecated-declarations")
# SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")

target_link_libraries(lsr algorithm network process m)
target_link_libraries(dtlsr algorithm_dt network_dt process_dt m)
target_link_libraries(heartbeat algorithm network process m)
target_link_libraries(default algorithm network process m)
