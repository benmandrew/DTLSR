
include_directories(${CMAKE_SOURCE_DIR}/include)

set(algorithm_source_files
  heap.c
	local_graph.c
	ls_graph.c
	node.c
	pathfind.c
	link_hist.c
)

add_library(algorithm ${algorithm_source_files})

target_link_libraries(algorithm network)
target_link_libraries(algorithm process)
target_link_libraries(algorithm m)
