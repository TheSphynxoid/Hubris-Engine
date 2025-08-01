#include "pch.h"
#include "Core/Graphics/GL/GLPlatform.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>


Hubris::ErrorCode Hubris::Graphics::GL::GLPlatform::InitGL()
{
    Logger::Log("OpenGL Backend init started");
    if(!gladLoadGL()){
        Logger::Fatal("Failed to load GLAD");
        return ErrorCode::NOT_INITIALIZED | ErrorCode::RUNTIME_ERROR;
    }
    Logger::Log("GLAD loaded successfully");
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwInitHint(GLFW_VERSION_MAJOR, 4);
    glfwInitHint(GLFW_VERSION_MINOR, 6);

    if(glfwInit() == GLFW_FALSE){
        Logger::Log("Failed to initalize GLFW");
        return ErrorCode::NOT_INITIALIZED | ErrorCode::RUNTIME_ERROR;
    }
    Logger::Log("GLFW successfully initalized");
    Logger::Log("OpenGL Backend initalized.");
    return ErrorCode::OK;
}