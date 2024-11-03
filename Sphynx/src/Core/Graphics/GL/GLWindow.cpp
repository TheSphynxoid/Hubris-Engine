#include "pch.h"
#include "GLWindow.h"
#include <glad/glad.h>
#include <glfw3.h>

void Sphynx::Graphics::GL::GLWindow::Init()
{
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwInitHint(GLFW_VERSION_MAJOR, 4);
    glfwInitHint(GLFW_VERSION_MINOR, 6);

    if(glfwInit() == GLFW_FALSE){
        std::cout << "Unable to init glfw" << std::endl;
    }
}
