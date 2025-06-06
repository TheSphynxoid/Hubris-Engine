#pragma once
#ifdef SPH_LOGGER
#else
#define SPH_LOGGER
#define LOG_FILE_LIMIT 5
#define LOG_FILE_MAX_SIZE 5 * 1024 * 1024 //5 MB is the default log file size limit.
#include <iostream>
#include <utility>
#include <chrono>
#include <thread>
#include <iomanip>
#include <sstream>
#include <unordered_map>
#include "fmt/core.h"
#include <fmt/std.h>

namespace Sphynx {
    class Logger {
    private:
        static inline std::unordered_map<std::string, FILE*> LogFiles;
        static inline FILE* LogFile = nullptr;
    
        static std::string getCurrentTime() {
            auto now = std::chrono::system_clock::now();
            auto in_time_t = std::chrono::system_clock::to_time_t(now);
            //
            return fmt::format("{:%Y-%m-%d %H:%M:%S}", fmt::localtime(in_time_t));;
        }   

        static FILE* OpenAndReturn(std::string fileName){
            FILE* f;
            errno_t err = fopen_s(&f, fileName.c_str(), "a+");
            if(err != 0){
                std::cerr << "Logger: Error opening file " << fileName << std::endl;
            }
            LogFiles[fileName] = f;
            return f;
        }

    public:
        /**
         * @brief Starts the logger.
         * 
         */
        static void Start(){
            // int logIndex = 0;
            // while(logIndex < LOG_FILE_LIMIT){
            //     if(std::filesystem::file_size(fmt::format("Engine{}.log", logIndex ? std::to_string(logIndex) : "")) > LOG_FILE_MAX_SIZE){
            //         ++logIndex;
            //         continue;
            //     }
            //     break;
            // }
            // //TODO: have a way to replace old Files.
            errno_t err = fopen_s(&LogFile, "Engine.log", "a+");
            if(err != 0){
                std::cout << "Fatal: Failed to open log file\n";
            }
        }
        /**
         * @brief Flushes and closes all the files used for logging.
         * 
         */
        static void Close(){
            fflush(LogFile);
            fclose(LogFile);
            for(const auto& file : LogFiles){
                fflush(file.second);
                fclose(file.second);
            }
        }
        /**
         * @brief Flushes and keeps the files open.
         * 
         */
        static void Flush(){
            std::cout.flush();
            fflush(LogFile);
            for(const auto& file : LogFiles){
                fflush(file.second);
            }
        }
        /**
         * @brief Logs in a file, if the file is used for the first time, it is opened and kept in cache.
         */
        template<typename ...Args>
        static void FileLog(std::string fileName, const char* message, Args&& ...args){
            FILE* file = LogFiles[fileName];
            file = (file ? file : OpenAndReturn(fileName));
            std::cout << fmt::format(message, std::forward<Args>(args)...) << std::endl;
            fmt::println(file, "({})[ThreadID: {}] Fatal: {}", getCurrentTime(), std::this_thread::get_id(), fmt::format(message, std::forward<Args>(args)...));
        }

        //template<typename ...Args>
        //static void Log(fmt::string_view message, Args&&... args) {
        //    const auto formatted = fmt::format(message, std::forward<Args>(args)...);
        //    std::cout << formatted << std::endl;
        //    if (!LogFile) return;
        //    fmt::println(LogFile, "({})[ThreadID: {}] Info: {}", getCurrentTime(), std::this_thread::get_id(), formatted);
        //    fflush(LogFile);
        //}

        
        static void Log(const char* message) {
            const auto formatted = fmt::format("({})[ThreadID: {}] Info: {}", getCurrentTime(), std::this_thread::get_id(), message);
            std::cout << formatted << std::endl;
            if (!LogFile) return;
            fmt::println(LogFile, "{}", formatted);
            fflush(LogFile);
        }

        static void Log(const std::string& message) {
            const auto formatted = fmt::format("({})[ThreadID: {}] Info: {}", getCurrentTime(), std::this_thread::get_id(), message.c_str());
            std::cout << formatted << std::endl;
            if (!LogFile) return;
            fmt::println(LogFile, "{}", formatted);
            fflush(LogFile);
        }

        template<typename ...Args>
        static void Log(fmt::format_string<Args...> message, Args&&... args) {
            //I wonder if this can be done in one operation ? Recursive formatting ?
            const auto formatted = fmt::format(message, std::forward<Args>(args)...);
            const auto finalFormatted = fmt::format("({})[ThreadID: {}] Info: {}\n", getCurrentTime(), std::this_thread::get_id(), formatted);
            std::cout << finalFormatted << std::endl;
            if (!LogFile) return;
            fmt::println(LogFile, "{}", finalFormatted);
            fflush(LogFile);
        }

        /*template<typename ...Args>
        static void Fatal(fmt::string_view message, Args&&... args) {
            const auto formatted = fmt::format(message, std::forward<Args>(args)...);
            std::cerr << fmt::format("({})[ThreadID: {}] Fatal: {}\n", getCurrentTime(), std::this_thread::get_id(), formatted);
            if (!LogFile) return;
            fmt::println(LogFile, "({})[ThreadID: {}] Fatal: {}", getCurrentTime(), std::this_thread::get_id(), formatted);
            fflush(LogFile);
        }*/

        template<typename ...Args>
        static void Fatal(fmt::format_string<Args...> message, Args&&... args) {
            const auto formatted = fmt::format(message, std::forward<Args>(args)...);
            std::cerr << fmt::format("({})[ThreadID: {}] Fatal: {}\n", getCurrentTime(), std::this_thread::get_id(), formatted);
            if (!LogFile) return;
            fmt::println(LogFile, "({})[ThreadID: {}] Fatal: {}", getCurrentTime(), std::this_thread::get_id(), formatted);
            fflush(LogFile);
        }

    };
}

#endif