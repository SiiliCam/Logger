# CMakeList.txt : CMake project for Logger, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (Sample main.cpp  )

target_link_libraries(Sample PUBLIC Logger)
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Sample PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.