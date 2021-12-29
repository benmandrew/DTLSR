
cmake_minimum_required( VERSION 3.9.1 )

project( routing_int )

include_directories(${CMAKE_SOURCE_DIR}/include)

add_subdirectory(algorithm)
add_subdirectory(network)
add_subdirectory(process)

set(graph_source_files graph_main.c)
set(heartbeat_source_files heartbeat_main.c)

add_executable(graph ${graph_source_files})
add_executable(heartbeat ${heartbeat_source_files})

target_link_libraries(graph algorithm network process)
target_link_libraries(heartbeat algorithm network process)
