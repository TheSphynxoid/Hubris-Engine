/**
 * @file EntryPoint.h
 * @author TheSphynx
 * @brief The reason this file exists it to allow multiple modals of executions,
 * The engine will "Hijack" the main (Static lib) and return the call to the user through run(),
 * 2) Engine Controlled loop (Relying only on the EventBus)
 * 3) a mix of both, the Engine handles entry, 
 * then calls the user run without calling the looping mechanism
 * @code {.cpp}
 * int run(int argc, char** argv){
    Hubris::EngineConfig config = Hubris::Engine::GetPlatformConfig("Sandbox", { 0,0,0,1 });
    Hubris::Core::StaticEventBus<Hubris::Core::OnStart>::Subscribe(&OnStart);
    Hubris::Engine::Init(config);
    Hubris::Engine::Run(); //Engine::Run() retuns control to the engine.
    
    //Hubris::Engine::CreateWindow()
    return 0;
    }
 * @endcode
 * @version 0.2-VFS
 * @date 2025-12-25
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#pragma once
#include <iostream>
#include <Engine.h>
#include <Logger.h>

/**
 * @defgroup Entrypoint Entry point details.
 * @{
 */
extern "C"{
    /**
     * @brief This is the user entry point. This should be implemented by the user.
     */
    extern int run(int argc, char** argv);
}

/**
 * @brief Slight preparation needed by the engine for first time use.
 * after the initialization is complete, the engine yields control to user by calling ::run. The engine is inteneded to work as a platform.
 * entry points signature should be:
 * @code {.cpp}
 * int main(int argc, char** argv) //there is a macro defines main as run.
 * @endcode
 * or
 * @code {.cpp}
 * int run(int argc, char** argv) //define run directly.
 * @endcode
 * 
 */
int main(int argc, char** argv) {
    std::cout << "Hubris Engine has started." << std::endl;
    Hubris::Logger::Start();
    return run(argc, argv);
}

/**
 * @brief This is to avoid redefining main as the engine defines it. We rename the user main to ::run and call it.
 * 
 */
#define main ::run

/** @} */
