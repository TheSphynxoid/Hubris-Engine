#pragma once
#ifdef SNDBOX_MAIN
#else
#define SNDBOX_MAIN
#include <Hubris.h>
#include <HubrisGraphics.h>
#include <Core/EventBus.h>
#include <IO/ResourceManager.h>

void OnStart(const Hubris::Core::OnStart& e){
    Hubris::Logger::Log("Client On Start Called");
    auto& rm = Hubris::IO::ResourceManager::Instance();

    // Single obvious load path: ResourceManager::Load<Shader>. Bytes are fetched via VFS,
    // the stage is read from the SPIR-V binary, the typed Shader is cached and owned by RM;
    // we receive a non-owning view.
    Hubris::Graphics::Shader* vert = rm.Load<Hubris::Graphics::Shader>("shaders://shader_vert.spv");
    if(!vert || !vert->Valid()){
        Hubris::Logger::Log("Unable to load shader_vert.spv");
        Hubris::Engine::Shutdown();
        return;
    }
    Hubris::Graphics::Shader* frag = rm.Load<Hubris::Graphics::Shader>("shaders://shader_frag.spv");
    if(!frag || !frag->Valid()){
        Hubris::Logger::Log("Unable to load shader_frag.spv");
        Hubris::Engine::Shutdown();
        return;
    }

    // PipelineDescriptor holds non-owning Shader* views; RM owns the canonical objects.
    Hubris::Graphics::PipelineDescriptor desc;
    desc.shaders.reserve(2);
    desc.shaders.push_back(vert);
    desc.shaders.push_back(frag);

    Hubris::Handle<Hubris::Graphics::Pipeline> p = Hubris::Graphics::Pipeline::Create(desc);

    Hubris::Logger::Log("Sandbox pipeline created successfully.");
    // Success: return and let Engine::Run() keep the window open.
    // (Shutdown() is only for the failure branches above.)
}

int run(int argc, char** argv){
    Hubris::EngineConfig config = Hubris::Engine::GetPlatformConfig("Sandbox", { 0,0,0,1 });
    // Client owns its mounts entirely — the engine defines no default mount set.
    config.resources.mounts.push_back({ "shaders://", "assets/shaders", 100 });
    Hubris::Core::StaticEventBus<Hubris::Core::OnStart>::Subscribe(&OnStart);
    // Client registers the typed factories it needs BEFORE Init (OnStart fires during
    // Run and may load assets). Engine presumes nothing about factories or mounts.
    Hubris::Graphics::RegisterAssetFactories();
    Hubris::Engine::Init(config);
    Hubris::Engine::CreateWindow({ 800, 600, "Sandbox" });  // explicit, optional (headless without it)
    Hubris::Engine::Run();

    return 0;
}
#endif