set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(Sandbox "Sandbox.cpp" "Sandbox.h")

#target_link_libraries(Sandbox PRIVATE EngineStub)
target_link_libraries(Sandbox PRIVATE HubrisEngine)