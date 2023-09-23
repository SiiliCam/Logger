# CMakeList.txt : CMake project for Logger, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (SampleLogger main.cpp  )

target_link_libraries(SampleLogger PUBLIC Logger)
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET SampleLogger PROPERTY CXX_STANDARD 20)
endif()

add_dependencies(SampleLogger Logger)

# TODO: Add tests and install targets if needed.