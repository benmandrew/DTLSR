
include_directories(${CMAKE_SOURCE_DIR}/include)

set(process_source_files
  core_node.c
	daemonise.c
	fd_event.c
	fileio.c
	logging.c
	route_control.c
)

add_library(process STATIC ${process_source_files})
add_library(process_dt STATIC ${process_source_files})

target_compile_definitions(process_dt PRIVATE DTLSR)

target_link_libraries(process algorithm network)
target_link_libraries(process_dt algorithm_dt network_dt)

# install(TARGETS process DESTINATION lib)
