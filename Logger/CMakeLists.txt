
# Create the Logger library as an INTERFACE library
add_library(Logger INTERFACE)

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Set include directories for Logger
target_include_directories(Logger INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Copy the include files to the binary directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/include/ 
     DESTINATION ${CMAKE_BINARY_DIR}/include/)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/Logger.hpp
               ${CMAKE_BINARY_DIR}/include/Logger.hpp COPYONLY)
