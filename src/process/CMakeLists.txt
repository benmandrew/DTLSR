
include_directories(${CMAKE_SOURCE_DIR}/include)

set(process_source_files
  core_node.c
	daemonise.c
	fd_event.c
	fileio.c
	logging.c
	route_control.c
)

add_library(process ${process_source_files})

target_link_libraries(process algorithm)
target_link_libraries(process network)
