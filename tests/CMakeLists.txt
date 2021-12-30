
include_directories(${CMAKE_SOURCE_DIR}/include)

find_package(Check REQUIRED)
include_directories(${CHECK_INCLUDE_DIRS})

set(TEST_SOURCES
  algorithm/check_heap.c
  algorithm/check_node.c
  check.c
)

set(LINK_LIBS algorithm ${CHECK_LIBRARIES})

# SET(GCC_COVERAGE_COMPILE_FLAGS "-fprofile-arcs -ftest-coverage")
SET(GCC_COVERAGE_LINK_FLAGS "-pthread")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")

add_executable(check_heap ${TEST_SOURCES})
target_link_libraries(check_heap ${LINK_LIBS} ${LINK_LIBS})
