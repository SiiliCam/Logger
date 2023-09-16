# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Logger")


set(Boost_DIR "C:/local/boost_1_83_0/stage/lib/cmake/Boost-1.83.0/")
SET(Boost_ADDITIONAL_VERSIONS 1.83.0)
find_package(Boost 1.83.0 REQUIRED COMPONENTS system log log_setup)

# Include sub-projects.
add_subdirectory ("Logger")
add_subdirectory ("sampleapplication")