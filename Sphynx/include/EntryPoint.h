#pragma once
#include <iostream>
#include <Engine.h>
#include <Logger.h>
extern int run(int argc, char** argv);

//Slight preparation needed by the engine for first time use.
//after the initialization is complete, the engine yeild control to user. The engine is inteneded to work as a platform.
int main(int argc, char** argv) {
    std::cout << "Sphynx Engine has started." << std::endl;
    Sphynx::Logger::Start();
    return run(argc, argv);
}
