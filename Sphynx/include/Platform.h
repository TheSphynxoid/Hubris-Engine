// File: Sphynx/include/Platform.h
#pragma once

enum class Sph_Platform : unsigned short {
    Windows, Linux, MacOS, IOS, Android, Unknown
};

[[nodiscard]] constexpr inline Sph_Platform GetPlatform() noexcept{
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #define SPH_WINDOWS
    return Sph_Platform::Windows;
#elif __APPLE__
#include <TargetConditionals.h>
#define SPH_APPLE
#if TARGET_IPHONE_SIMULATOR
    #define SPH_IOS
    return Sph_Platform::IOS; // iOS Simulator
#elif TARGET_OS_MACCATALYST
    #define SPH_MACOS
    return Sph_Platform::MacOS; // Mac Catalyst
#elif TARGET_OS_IPHONE
    #define SPH_IOS
    return Sph_Platform::IOS; // iOS device
#elif TARGET_OS_MAC
    #define SPH_MACOS
    return Sph_Platform::MacOS; // macOS
#else
#error "Unknown Apple platform"
#endif
#elif __ANDROID__
    #define SPH_ANDROID
    return Sph_Platform::Android;
#elif __linux__
    #define SPH_LINUX
    return Sph_Platform::Linux;
#elif __unix__ // all unices not caught above
    #define SPH_LINUX
    return Sph_Platform::Linux; // Assuming Unix-like systems are similar to Linux
#elif defined(_POSIX_VERSION)
    return Sph_Platform::Linux; // POSIX systems
#else
    return Sph_Platform::Unknown;
#endif
}

constexpr Sph_Platform Platform = GetPlatform();
