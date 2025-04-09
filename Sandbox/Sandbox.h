#pragma once
#ifdef SNDBOX_MAIN
#else
#define SNDBOX_MAIN
#include <Sphynx.h>

int run(int argc, char** argv){
    Sphynx::EngineConfig config = Sphynx::Engine::GetPlatformConfig("Sandbox", { 0,0,0,1 });
    Sphynx::Engine::Init(config);
    Sphynx::Engine::Loop();
    //Sphynx::Engine::CreateWindow()
    return 0;
}
#endif