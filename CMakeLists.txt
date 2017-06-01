cmake_minimum_required(VERSION 3.7)
project(ParallelBelmanFord)

set(CMAKE_CXX_STANDARD 11)

find_package (Threads)
find_package(MPI)

include_directories(SYSTEM ${MPI_INCLUDE_PATH})

add_executable(sequence main.cpp)
add_executable(pThreads pthreads.cpp)
add_executable(mpiProg mpi.cpp)

target_link_libraries (pThreads ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(mpiProg ${MPI_C_LIBRARIES})