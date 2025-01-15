# CMakeList.txt : CMake project for Sphynx-Engine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.10)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
cmake_policy(SET CMP0141 NEW)
set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

if(DEFINED ENV{VCPKG_ROOT})
#set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
message("VCPKG found at: $ENV{VCPKG_ROOT}")
else()
message(SEND_ERROR "VCPKG not found")
endif()

project(SphynxEngine)

add_subdirectory("Sphynx")
add_subdirectory("EngineStub")
add_subdirectory("Sandbox")
# Add source to this project's executable.
# add_executable (Sphynx-Engine "Sphynx-Engine.cpp" "Sphynx-Engine.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET SphynxEngine PROPERTY CXX_STANDARD 17)
endif()

# TODO: Add tests and install targets if needed.
