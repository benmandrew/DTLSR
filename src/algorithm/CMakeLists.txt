
include_directories(${CMAKE_SOURCE_DIR}/include)

set(algorithm_source_files
	heap.c
	local_graph.c
	ls_graph.c
	node.c
	pathfind.c
	link_hist.c
)

add_library(algorithm STATIC ${algorithm_source_files})
add_library(algorithm_dt STATIC ${algorithm_source_files})

target_compile_definitions(algorithm_dt PRIVATE DTLSR)

target_link_libraries(algorithm network process m)
target_link_libraries(algorithm_dt network_dt process_dt m)

# install(TARGETS algorithm DESTINATION lib)
