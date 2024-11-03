set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(Sandbox "Sandbox.cpp" "Sandbox.h")

#target_link_libraries(Sandbox PRIVATE EngineStub)
target_link_libraries(Sandbox PRIVATE SphynxEngine)

set(CMAKE_BINARY_DIR "${CMAKE_SOURCE_DIR}/build")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
