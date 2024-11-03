#pragma once
#ifdef SNDBOX_MAIN
#else
#define SNDBOX_MAIN
#include <Entrypoint.h>

int run(int argc, char** argv){

    Sphynx::Engine::Init(Sphynx::Engine::GetPlatformConfig());
    return 0;
}
#endif