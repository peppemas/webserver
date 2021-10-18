cmake_minimum_required(VERSION 3.20)
project(WebServer)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

add_executable(WebServer main.cpp httplib.h popl.hpp)

target_link_libraries(WebServer -static ws2_32 pthread)
