cmake_minimum_required(VERSION 3.25)
project(projectMPI CXX)

set(CMAKE_CXX_STANDARD 23)
# Set the path to the MS-MPI installation directory
set(MPI_ROOT "C:/Program Files/Microsoft MPI")  # Update this path if necessary

# Find MPI package
find_package(MPI REQUIRED)
set(CMAKE_EXE_LINKER_FLAGS "-static")
# Add MPI include directories to your project
include_directories(${MPI_INCLUDE_PATH})

# Link your executable with MPI libraries

add_executable(projectMPI main.cpp main.cpp)
target_link_libraries(projectMPI ${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES})
