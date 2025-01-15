#pragma once
#ifdef SNDBOX_MAIN
#else
#define SNDBOX_MAIN
#include <Entrypoint.h>

int run(int argc, char** argv){

    Sphynx::Engine::Init(Sphynx::Engine::GetPlatformConfig("Sandbox", { 0,0,0,1 }));
    //Sphynx::Engine::CreateWindow()
    return 0;
}
#endif