
cmake_minimum_required(VERSION 3.9.1)
project(routing LANGUAGES C)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
# set(CK_FORK "no")

## Check checks
include(CheckCSourceCompiles)
include(CheckCSourceRuns)
include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckLibraryExists)
include(CheckSymbolExists)
include(CheckTypeSize)

## Datatype checks
check_type_size(intmax_t INTMAX_T)
check_type_size(uintmax_t UINTMAX_T)
check_type_size(pid_t PID_T)
if(NOT HAVE_PID_T)
  if(WIN32)
    set(pid_t "int")
  else(WIN32)
    MESSAGE(FATAL_ERROR "pid_t doesn't exist on this platform?")
  endif(WIN32)
endif(NOT HAVE_PID_T)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(CMAKE_BUILD_TYPE Debug)

add_compile_options(-O3 -D_FORTIFY_SOURCE=2 -fasynchronous-unwind-tables -fpie -Wl,-pie -fpic -shared -fstack-clash-protection -fstack-protector -Werror=implicit-function-declaration)

# SET(GCC_COVERAGE_COMPILE_FLAGS "-fprofile-arcs -ftest-coverage -fsanitize=address -fsanitize=undefined")
# SET(GCC_COVERAGE_LINK_FLAGS "-pthread -lm -fsanitize=address -fsanitize=undefined")
# SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")

add_subdirectory(src)
add_subdirectory(tests)

enable_testing()
add_test(NAME check COMMAND check)
