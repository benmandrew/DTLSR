
cmake_minimum_required( VERSION 3.9.1 )

project( routing_int )

include_directories(${CMAKE_SOURCE_DIR}/include)

add_subdirectory(algorithm)
add_subdirectory(network)
add_subdirectory(process)

set(lsr_source_files lsr_main.c)
set(dtlsr_source_files dtlsr_main.c)
set(heartbeat_source_files heartbeat_main.c)
set(iperf_source_files iperf_main.c)

add_executable(lsr ${lsr_source_files})
add_executable(dtlsr ${dtlsr_source_files})
add_executable(heartbeat ${heartbeat_source_files})
add_executable(iperf ${iperf_source_files})

target_link_libraries(lsr algorithm network process m)
target_link_libraries(dtlsr algorithm network process m)
target_link_libraries(heartbeat algorithm network process m)
target_link_libraries(iperf algorithm network process m)
