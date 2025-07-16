#pragma once
#ifdef SNDBOX_MAIN
#else
#define SNDBOX_MAIN
#include <Hubris.h>
#include <HubrisGraphics.h>
#include <Core/EventBus.h>

Hubris::Handle<Hubris::Graphics::Shader> VertShader;
Hubris::Handle<Hubris::Graphics::Shader> FragShader;

void OnStart(const Hubris::Core::OnStart& e){
    Hubris::Logger::Log("Client On Start Called");
    auto vertShaderCode = Hubris::IO::readFile("shaders/vert.spv");
    if(!vertShaderCode.size()){
        Hubris::Logger::Log("Unable to read vert.spv");
        Hubris::Engine::Shutdown();
        return;
    }
    auto fragShaderCode = Hubris::IO::readFile("shaders/frag.spv");
    if(!vertShaderCode.size()){
        Hubris::Logger::Log("Unable to read frag.spv");
        Hubris::Engine::Shutdown();
        return;
    }

    VertShader = Hubris::Graphics::Shader::Create(vertShaderCode, Hubris::Graphics::ShaderStage::Vertex);
    if(!VertShader->Valid()){
        Hubris::Logger::Fatal("Failed to create Shader");
        return;
    }
    FragShader = Hubris::Graphics::Shader::Create(fragShaderCode, Hubris::Graphics::ShaderStage::Fragment);
    if(!FragShader->Valid()){
        Hubris::Logger::Fatal("Failed to create Shader");
        return;
    }
}

int run(int argc, char** argv){
    Hubris::EngineConfig config = Hubris::Engine::GetPlatformConfig("Sandbox", { 0,0,0,1 });
    Hubris::Core::StaticEventBus<Hubris::Core::OnStart>::Subscribe(&OnStart);
    Hubris::Engine::Init(config);
    // Hubris::Graphics::Shader::Create(Hubris::IO::ResourceManager::ReadFile("shaders/frag.spv").get_raw())
    Hubris::Engine::Run();
    
    //Hubris::Engine::CreateWindow()
    return 0;
}
#endif