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
    auto& vfs = Hubris::IO::VFS::VFS();

    auto vertShaderCode = vfs.Load("shaders://shader_vert.spv");
    if(!vertShaderCode || vertShaderCode->Size() == 0){
        Hubris::Logger::Log("Unable to read shader_vert.spv");
        Hubris::Engine::Shutdown();
        return;
    }
    auto fragShaderCode = vfs.Load("shaders://shader_frag.spv");
    if(!fragShaderCode || fragShaderCode->Size() == 0){
        Hubris::Logger::Log("Unable to read shader_frag.spv");
        Hubris::Engine::Shutdown();
        return;
    }

    VertShader = Hubris::Graphics::Shader::Create(vertShaderCode.value().data, Hubris::Graphics::ShaderStage::Vertex);
    if(!VertShader->Valid()){
        Hubris::Logger::Fatal("Failed to create Shader");
        return;
    }

    vfs.Unload(*vertShaderCode);

    FragShader = Hubris::Graphics::Shader::Create(fragShaderCode.value().data, Hubris::Graphics::ShaderStage::Fragment);
    if(!FragShader->Valid()){
        Hubris::Logger::Fatal("Failed to create Shader");
        return;
    }

    vfs.Unload(*fragShaderCode);

    Hubris::Graphics::PipelineDescriptor desc;
    desc.shaders.reserve(2);
    desc.shaders.emplace_back(std::move(VertShader));
    desc.shaders.emplace_back(std::move(FragShader));
    
    Hubris::Handle<Hubris::Graphics::Pipeline> p = Hubris::Graphics::Pipeline::Create(desc);

    Hubris::Logger::Log("Sandbox pipeline created successfully.");
    // Success: return and let Engine::Run() keep the window open.
    // (Shutdown() is only for the failure branches above.)
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