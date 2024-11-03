#pragma once
#ifdef SPH_LOGGER
#else
#define SPH_LOGGER
#include <iostream>
#include <utility>
#include <chrono>
#include <thread>
#include <iomanip>
#include <sstream>
#include <unordered_map>
#include "fmt/core.h"

namespace Sphynx {
    class Logger {
    private:
        static inline std::unordered_map<std::string, FILE*> LogFiles;
        static inline FILE* LogFile = nullptr;
    
        static std::string getCurrentTime() {
            auto now = std::chrono::system_clock::now();
            auto in_time_t = std::chrono::system_clock::to_time_t(now);
            //
            std::stringstream ss;
            ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
            return ss.str();
        }   

        static FILE* OpenAndSet(std::string fileName){
            FILE* f;
            errno_t err = fopen_s(&f, fileName.c_str(), "a+");
            if(err != 0){
                std::cerr << "Logger: Error opening file " << fileName << std::endl;
            }
            LogFiles[fileName] = f;
            return f;
        }

    public:
    
        static void Start(){
            LogFile = fopen("Engine.log", "a+");
        }
        template<typename ...Args>
        static void FileLog(std::string fileName, const char* message, Args&& ...args){
            FILE* file = LogFiles[fileName];
            file = (file ? file : OpenAndSet(fileName));
            std::cout << fmt::format(message, std::forward<Args>(args)...) << std::endl;
            fmt::println(file, "({})[ThreadID: {}] Fatal: {}", getCurrentTime(), std::this_thread::get_id(), fmt::format(message, std::forward<Args>(args)...));
        }
        template<typename ...Args>
        static void Log(const char* message,Args&&  ...args){
            std::cout << fmt::format(message, std::forward<Args>(args)...) << std::endl;
            fmt::println(LogFile, "({})[ThreadID: {}] Info: {}", getCurrentTime(), std::this_thread::get_id(), fmt::format(message, std::forward<Args>(args)...));
        }
        template<typename ...Args>
        static void Fatal(const char* message,Args&&  ...args){
            std::cerr << fmt::format("({})[ThreadID: {}] Info: {}\n", getCurrentTime(), message, std::forward<Args>(args)...);
            fmt::println(LogFile, "({})[ThreadID: {}] Fatal: {}", getCurrentTime(), std::this_thread::get_id(), fmt::format(message, std::forward<Args>(args)...));
        }
    };
}

#endif