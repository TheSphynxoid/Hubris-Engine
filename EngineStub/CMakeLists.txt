# cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# project(EngineStub VERSION 0.1.0 LANGUAGES CXX ASM)

add_library(EngineStub STATIC "Entry.h" "Entry.cpp" "Loader.h")

set_target_properties(EngineStub PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(EngineStub PUBLIC SphynxEngine)

target_include_directories(EngineStub PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})