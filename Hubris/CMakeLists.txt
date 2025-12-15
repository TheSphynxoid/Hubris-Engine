cmake_minimum_required(VERSION 3.26)
# project(HubrisEngine VERSION 0.1.0 LANGUAGES CXX)

set(HEADERS
"include/EntryPoint.h"
"include/pch.h" "include/Memory.h"
"include/Core/ThreadPool.h" "include/Engine.h" "include/Core/ThreaddingServer.h" "include/Core/Graphics/GL/GLPlatform.h" "include/Logger.h" 
"include/Core/Graphics/Vulkan/vkWindow.h" "include/Core/Graphics/Vulkan/vkRenderer.h" "include/Core/Graphics/Vulkan/vkBackend.h" "include/Platform.h" "include/IO/ResourceManager.h" "include/Core/Utils.h" "include/Core/Graphics/Format.h" 
"include/Core/Graphics/Vulkan/Utility.h" "include/Core/Graphics/Vulkan/vkPipeline.h" "include/Core/Graphics/Vulkan/vkSwapchain.h" "include/Core/Graphics/Shader.h" "include/Core/Graphics/Image.h" "include/Core/Graphics/Vulkan/vkShader.h" "include/Core/Graphics/Pipeline.h"  "include/List.h"  "include/Core/EventBus.h" "include/HubrisGraphics.h")
set(SOURCES
"src/pch.cpp" "src/Memory_inst.cpp" "src/Logger.cpp" "src/IO/ResourceManager.cpp" "src/Entrypoint.cpp" "src/Engine.cpp" "src/Core/Graphics/Vulkan/vkWindow.cpp" "src/Core/Graphics/Vulkan/vkRenderer.cpp"
"src/Core/Graphics/Vulkan/vkShader.cpp" "src/Core/Graphics/Window.cpp" "src/Core/Graphics/Shader.cpp" "src/Core/Graphics/Pipeline.cpp" "src/Core/Graphics/GL/GLPlatform.cpp" "src/Core/Graphics/Vulkan/vkSwapchain.cpp" "src/Core/Graphics/Vulkan/vkPipeline.cpp")

message(${CMAKE_CURRENT_SOURCE_DIR})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files" FILES ${SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include PREFIX "Header Files" FILES ${HEADERS})

add_library(HubrisEngine STATIC ${HEADERS} ${SOURCES})

target_compile_features(HubrisEngine PUBLIC cxx_std_20)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# Add the GLAD library from dep/glad
add_subdirectory(${CMAKE_SOURCE_DIR}/dep/glad ${CMAKE_BINARY_DIR}/glad)

# Link the GLAD library to your executable
target_link_libraries(HubrisEngine PRIVATE glad)

# find_package(SPIRV-Tools-reduce CONFIG REQUIRED)
find_package(glslang CONFIG REQUIRED)
find_package(SPIRV-Tools CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

# find_package(SPIRV-Tools-lint CONFIG REQUIRED)
# find_package(SPIRV-Tools-opt CONFIG REQUIRED)
# find_package(SPIRV-Tools-link CONFIG REQUIRED)

find_package(OpenGL REQUIRED)
target_link_libraries(HubrisEngine PRIVATE OpenGL::GL)
# The static libary is always available.
# It offers full public symbol visibility.
target_link_libraries(HubrisEngine PRIVATE SPIRV-Tools-static)
target_link_libraries(HubrisEngine PRIVATE fmt::fmt)

target_link_libraries(HubrisEngine PRIVATE assimp::assimp)
target_link_libraries(HubrisEngine PRIVATE glfw)
target_link_libraries(HubrisEngine PUBLIC glm::glm)

# target_link_libraries(HubrisEngine PRIVATE SPIRV-Tools-link SPIRV-Tools-lint SPIRV-Tools-opt SPIRV-Tools-reduce glslang::glslang glslang::glslang-default-resource-limits glslang::SPIRV glslang::SPVRemapper)
# target_link_libraries(HubrisEngine PRIVATE glslang::glslang glslang::glslang-default-resource-limits glslang::SPIRV glslang::SPVRemapper)
target_link_libraries(HubrisEngine PRIVATE glslang::glslang glslang::glslang-default-resource-limits glslang::SPIRV)

# find_package(OpenMP REQUIRED)

# # Link OpenMP to your target
# target_link_libraries(MyEngine PUBLIC OpenMP::OpenMP_CXX)

# if(OpenMP_CXX_FOUND)
#     target_compile_options(HubrisEngine PUBLIC ${OpenMP_CXX_FLAGS})
# endif()

target_precompile_headers(HubrisEngine PRIVATE "include/pch.h")

target_include_directories(HubrisEngine 
PUBLIC
    ${CMAKE_SOURCE_DIR}/Hubris/include
PRIVATE
    ${CMAKE_SOURCE_DIR}/Hubris/src
)
