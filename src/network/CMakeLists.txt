
include_directories(${CMAKE_SOURCE_DIR}/include)

set(network_source_files
  packetsend.c
  capture.c
)

add_library(network STATIC ${network_source_files})
add_library(network_dt STATIC ${network_source_files})

target_compile_definitions(network_dt PRIVATE DTLSR)

target_link_libraries(network algorithm process pcap)
target_link_libraries(network_dt algorithm_dt process_dt pcap)

# install(TARGETS network DESTINATION lib)
