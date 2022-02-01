
include_directories(${CMAKE_SOURCE_DIR}/include)

set(network_source_files
  packetsend.c
  capture.c
)

add_library(network ${network_source_files})

target_link_libraries(network algorithm)
target_link_libraries(network process)
target_link_libraries(network pcap)
