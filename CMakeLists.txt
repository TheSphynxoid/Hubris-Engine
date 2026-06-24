# CMakeList.txt : CMake project for Hubris-Engine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.26)


set(CMAKE_CXX_STANDARD 20)
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
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
message("VCPKG found at: $ENV{VCPKG_ROOT}")
else()
message(SEND_ERROR "VCPKG not found")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
    message("Debug Build")
endif()

project(HubrisEngine)

# LTCG/LTO + PGO infrastructure. Options default to: LTCG on, PGO off.
# See cmake/PGO.cmake for the workflow and the "when to use PGO" caveat.
include("${CMAKE_SOURCE_DIR}/cmake/PGO.cmake")

# TODO: Add tests and install targets if needed.
# set(CMAKE_BINARY_DIR "${CMAKE_SOURCE_DIR}/build")
# set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
# set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")

foreach(OUTPUTCONFIG IN LISTS CMAKE_CONFIGURATION_TYPES)
    string(TOUPPER "${OUTPUTCONFIG}" OUTPUTCONFIG_UPPER)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} "${CMAKE_BINARY_DIR}/bin/${OUTPUTCONFIG}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} "${CMAKE_BINARY_DIR}/lib/${OUTPUTCONFIG}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} "${CMAKE_BINARY_DIR}/lib/${OUTPUTCONFIG}")
endforeach()

# Function to compile Slang shaders to SPIR-V
function(compile_slang_shaders)
  # Find all .slang files in Hubris/shaders
  file(GLOB_RECURSE SLANG_SOURCES "${CMAKE_SOURCE_DIR}/Hubris/shaders/*.slang")
  
  if(NOT SLANG_SOURCES)
    message(STATUS "No Slang shader sources found")
    return()
  endif()
  
  if(NOT SLANGC_EXECUTABLE)
    # Try to find slang compiler
    find_program(SLANGC_EXECUTABLE slangc HINTS "$ENV{VULKAN_SDK}/Bin" "$ENV{VK_SDK_PATH}/Bin")
    if(NOT SLANGC_EXECUTABLE)
      message(AUTHOR_WARNING "Slang compiler (slangc) not found. Shader compilation will be skipped. Set SLANGC_EXECUTABLE to the path of slangc.exe")
      return()
    endif()
  endif()
  
  # Create output directory for compiled shaders
  set(SHADER_OUTPUT_DIR "${CMAKE_BINARY_DIR}/assets/shaders")
  file(MAKE_DIRECTORY "${SHADER_OUTPUT_DIR}")
  
  # Compile each shader file (separate outputs per stage)
  foreach(SLANG_FILE ${SLANG_SOURCES})
    get_filename_component(SHADER_NAME ${SLANG_FILE} NAME_WE)
    
    set(VERT_OUTPUT "${SHADER_OUTPUT_DIR}/${SHADER_NAME}_vert.spv")
    set(FRAG_OUTPUT "${SHADER_OUTPUT_DIR}/${SHADER_NAME}_frag.spv")
    
    # Compile vertex shader
    add_custom_command(
      OUTPUT ${VERT_OUTPUT}
      COMMAND ${SLANGC_EXECUTABLE} "${SLANG_FILE}" -target spirv -profile spirv_1_4 -emit-spirv-directly -stage vertex -entry vertMain -o "${VERT_OUTPUT}"
      DEPENDS ${SLANG_FILE}
      COMMENT "Compiling vertex shader: ${SHADER_NAME}"
      VERBATIM
    )
    
    # Compile fragment shader
    add_custom_command(
      OUTPUT ${FRAG_OUTPUT}
      COMMAND ${SLANGC_EXECUTABLE} "${SLANG_FILE}" -target spirv -profile spirv_1_4 -emit-spirv-directly -stage fragment -entry fragMain -o "${FRAG_OUTPUT}"
      DEPENDS ${SLANG_FILE}
      COMMENT "Compiling fragment shader: ${SHADER_NAME}"
      VERBATIM
    )
    
    list(APPEND SHADER_OUTPUTS ${VERT_OUTPUT} ${FRAG_OUTPUT})
  endforeach()
  
  # Create a custom target that depends on all shader outputs
  add_custom_target(CompileShaders ALL DEPENDS ${SHADER_OUTPUTS})
  
  # Store the output directory for use in subdirectories
  set(COMPILED_SHADERS_DIR "${SHADER_OUTPUT_DIR}" CACHE INTERNAL "Directory containing compiled shaders")
endfunction()

# Compile shaders before processing subdirectories
compile_slang_shaders()

# Function to copy assets from engine to target executable
function(install_engine_assets TARGET)
  # Copy prebuilt assets
  if(EXISTS "${CMAKE_SOURCE_DIR}/Hubris/assets")
    add_custom_command(TARGET ${TARGET} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/Hubris/assets"
        "$<TARGET_FILE_DIR:${TARGET}>/assets"
      COMMENT "Copying prebuilt assets to ${TARGET}"
    )
  endif()
  
  # Copy compiled shaders
  if(EXISTS "${COMPILED_SHADERS_DIR}")
    add_custom_command(TARGET ${TARGET} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${COMPILED_SHADERS_DIR}"
        "$<TARGET_FILE_DIR:${TARGET}>/assets/shaders"
      COMMENT "Copying compiled shaders to ${TARGET}"
      # DEPENDS CompileShaders
    )
  endif()
endfunction()

add_subdirectory("Hubris")
# add_subdirectory("EngineStub")
add_subdirectory("Sandbox")
# Add source to this project's executable.
# add_executable (Hubris-Engine "Hubris-Engine.cpp" "Hubris-Engine.h")

# Apply LTCG (and PGO if enabled) to built targets.
hubris_apply_optimization(HubrisEngine)
hubris_apply_optimization(Sandbox)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET HubrisEngine PROPERTY CXX_STANDARD 20)
endif()