#pragma once
#ifdef SNDBOX_MAIN
#else
#define SNDBOX_MAIN
#include <Sphynx.h>
#include <SphynxGraphics.h>
#include <Core/EventBus.h>

Sphynx::Handle<Sphynx::Graphics::Shader> VertShader;
Sphynx::Handle<Sphynx::Graphics::Shader> FragShader;

void OnStart(const Sphynx::Core::OnStart& e){
    Sphynx::Logger::Log("Client On Start Called");
    auto vertShaderCode = Sphynx::IO::readFile("shaders/vert.spv");
    if(!vertShaderCode.size()){
        Sphynx::Logger::Log("Unable to read vert.spv");
        Sphynx::Engine::Shutdown();
        return;
    }
    auto fragShaderCode = Sphynx::IO::readFile("shaders/frag.spv");
    if(!vertShaderCode.size()){
        Sphynx::Logger::Log("Unable to read frag.spv");
        Sphynx::Engine::Shutdown();
        return;
    }

    VertShader = Sphynx::Graphics::Shader::Create(vertShaderCode, Sphynx::Graphics::ShaderStage::Vertex);
    if(!VertShader->Valid()){
        Sphynx::Logger::Fatal("Failed to create Shader");
        return;
    }
    FragShader = Sphynx::Graphics::Shader::Create(fragShaderCode, Sphynx::Graphics::ShaderStage::Fragment);
    if(!FragShader->Valid()){
        Sphynx::Logger::Fatal("Failed to create Shader");
        return;
    }
}

int run(int argc, char** argv){
    Sphynx::EngineConfig config = Sphynx::Engine::GetPlatformConfig("Sandbox", { 0,0,0,1 });
    Sphynx::Core::StaticEventBus<Sphynx::Core::OnStart>::Subscribe(&OnStart);
    Sphynx::Engine::Init(config);
    // Sphynx::Graphics::Shader::Create(Sphynx::IO::ResourceManager::ReadFile("shaders/frag.spv").get_raw())
    Sphynx::Engine::Run();
    
    //Sphynx::Engine::CreateWindow()
    return 0;
}
#endif