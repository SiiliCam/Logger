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


if (ANDROID)
  set(Boost_ROOT "C:/local/Boost-for-Android/build/build")  # Replace this with the actual path
  include_directories(${Boost_ROOT}/include)

  if (ANDROID_ABI STREQUAL "armeabi-v7a")
      set(Boost_LIBRARY_DIR "${Boost_ROOT}/armeabi-v7a/boost/lib")
  elseif (ANDROID_ABI STREQUAL "arm64-v8a")
      set(Boost_LIBRARY_DIR "${Boost_ROOT}/arm64-v8a/boost/lib")
  elseif (ANDROID_ABI STREQUAL "x86_64")
      set(Boost_LIBRARY_DIR "${Boost_ROOT}/x86_64/boost/lib")
  elseif (ANDROID_ABI STREQUAL "x86")
      set(Boost_LIBRARY_DIR "${Boost_ROOT}/x86/boost/lib")
  endif()

  link_directories(${Boost_LIBRARY_DIR})

else()
  set(Boost_DIR "C:/local/boost_1_83_0/stage/lib/cmake/Boost-1.83.0/")
  find_package(Boost 1.83.0 REQUIRED COMPONENTS system log log_setup)
endif()

# Include sub-projects.
add_subdirectory ("Logger")
add_subdirectory ("sampleapplication")