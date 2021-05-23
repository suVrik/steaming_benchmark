cmake_minimum_required(VERSION 3.19)

project(streaming_benchmark)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release")

file(GLOB_RECURSE SOURCES "source/*.cpp" "source/*.h")
add_executable(streaming_benchmark ${SOURCES})

option(USE_MULTITHREADING "Use thread pools to speed up file loading." ON)
option(USE_ASYNCHRONOUS_IO "Use asynchronous io to free main thread/worker threads from sys calls." ON)

if(USE_MULTITHREADING)
    target_compile_definitions(streaming_benchmark PRIVATE USE_MULTITHREADING)
endif()

if(USE_ASYNCHRONOUS_IO)
    target_compile_definitions(streaming_benchmark PRIVATE USE_ASYNCHRONOUS_IO)
endif()
