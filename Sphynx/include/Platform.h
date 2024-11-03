// File: Sphynx/include/Platform.h
#pragma once

enum class Sph_Platform : unsigned short {
    Windows, Linux, MacOS, IOS, Android, Unknown
};

constexpr Sph_Platform GetPlatform() noexcept{
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    return Sph_Platform::Windows;
#elif __APPLE__
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR
    return Sph_Platform::IOS; // iOS Simulator
#elif TARGET_OS_MACCATALYST
    return Sph_Platform::MacOS; // Mac Catalyst
#elif TARGET_OS_IPHONE
    return Sph_Platform::IOS; // iOS device
#elif TARGET_OS_MAC
    return Sph_Platform::MacOS; // macOS
#else
#error "Unknown Apple platform"
#endif
#elif __ANDROID__
    return Sph_Platform::Android;
#elif __linux__
    return Sph_Platform::Linux;
#elif __unix__ // all unices not caught above
    return Sph_Platform::Linux; // Assuming Unix-like systems are similar to Linux
#elif defined(_POSIX_VERSION)
    return Sph_Platform::Linux; // POSIX systems
#else
    return Sph_Platform::Unknown;
#endif
}

constexpr Sph_Platform Platform = GetPlatform();
