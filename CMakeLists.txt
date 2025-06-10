# Require at least CMake version 3.10
cmake_minimum_required(VERSION 3.10)

# Define the project name
project(NIFTI_IO_PROJECT)

# Set the C++ standard to C++17 and make it required
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories for header files
include_directories(src)                   # Project headers in src/
include_directories(/usr/include/nifti)    # NIfTI library headers

# Directories to search for libraries when linking
link_directories(/usr/lib)                 # System library path

# Gather the common source file(s)
file(GLOB COMMON_SOURCES src/imgNIFTI.cpp)

# Main executable target
add_executable(nifti_io_test
    ${COMMON_SOURCES}  # Shared NIFTI reader/writer implementation
    src/main.cpp       # Test driver for reading/writing one file
)

# Batch image test executable target
add_executable(test_image_batch
    ${COMMON_SOURCES}  # Shared NIFTI reader/writer implementation
    src/testImgGen.cpp # Driver for batch image generation/testing
)

# Link required libraries to both executables
foreach(target IN ITEMS nifti_io_test test_image_batch)
    target_link_libraries(${target}
        niftiio    # libnifti I/O routines
        znz        # NIfTI compression support
        z          # zlib for general compression
        i3dcore    # Core i3d library
    )
endforeach()
