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
    std::cout << "Sphynx Engine has started." << std::endl;
    Sphynx::Logger::Start();
    return run(argc, argv);
}

/**
 * @brief This is to avoid redefining main as the engine defines it. We rename the user main to ::run and call it.
 * 
 */
#define main ::run

/** @} */
