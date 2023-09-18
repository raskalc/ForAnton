cmake_minimum_required(VERSION 3.25)
project(projectMPI C)

set(CMAKE_C_STANDARD 17)
# Set the path to the MS-MPI installation directory
set(MPI_ROOT "C:/Program Files/Microsoft MPI")  # Update this path if necessary

# Find MPI package
find_package(MPI REQUIRED)

# Add MPI include directories to your project
include_directories(${MPI_INCLUDE_PATH})

# Link your executable with MPI libraries

add_executable(projectMPI main.c)
target_link_libraries(projectMPI ${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES})
