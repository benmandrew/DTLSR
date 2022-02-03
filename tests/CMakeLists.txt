
include_directories(${CMAKE_SOURCE_DIR}/include)

find_package(Check REQUIRED)
include_directories(${CHECK_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(TEST_SOURCES
  check.c
  algorithm/check_heap.c
  algorithm/check_node.c
  algorithm/check_pathfind.c
  algorithm/check_link_hist.c
  network/check_capture.c
)

set(LINK_LIBS algorithm ${CHECK_LIBRARIES})

# SET(SANITIZE "-fsanitize=address -fsanitize=undefined")
SET(SANITIZE "")

SET(GCC_COVERAGE_COMPILE_FLAGS "-fprofile-arcs -ftest-coverage ${SANITIZE}")
SET(GCC_COVERAGE_LINK_FLAGS "-pthread -lm ${SANITIZE}")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")

add_executable(check ${TEST_SOURCES})
target_link_libraries(check ${LINK_LIBS} ${LINK_LIBS})
