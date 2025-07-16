set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(Sandbox "Sandbox.cpp" "Sandbox.h")

#target_link_libraries(Sandbox PRIVATE EngineStub)
target_link_libraries(Sandbox PRIVATE HubrisEngine)

# set(CMAKE_BINARY_DIR "${CMAKE_SOURCE_DIR}/build")
# set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
# set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
 

add_custom_command(
    TARGET Sandbox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/shaders
            $<TARGET_FILE_DIR:Sandbox>/shaders
    COMMENT "Copying assets to output directory"
)