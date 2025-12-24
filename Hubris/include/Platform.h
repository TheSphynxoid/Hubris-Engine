// File: Hubris/include/Platform.h
#pragma once

enum class Hbr_Platform : unsigned short {
    Windows, Linux, MacOS, IOS, Android, Unknown
};

[[nodiscard]] consteval inline Hbr_Platform GetPlatform() noexcept{
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #define HBR_WINDOWS
    #define WIN32_LEAN_AND_MEAN 
    //#include "Windows.h"
    return Hbr_Platform::Windows;
#elif __APPLE__
#include <TargetConditionals.h>
#define HBR_APPLE
#if TARGET_IPHONE_SIMULATOR
    #define HBR_IOS
    return Hbr_Platform::IOS; // iOS Simulator
#elif TARGET_OS_MACCATALYST
    #define HBR_MACOS
    return Hbr_Platform::MacOS; // Mac Catalyst
#elif TARGET_OS_IPHONE
    #define HBR_IOS
    return Hbr_Platform::IOS; // iOS device
#elif TARGET_OS_MAC
    #define HBR_MACOS
    return Hbr_Platform::MacOS; // macOS
#else
#error "Unknown Apple platform"
#endif
#elif __ANDROID__
    #define HBR_ANDROID
    return Hbr_Platform::Android;
#elif __linux__
    #define HBR_LINUX
    return Hbr_Platform::Linux;
#elif __unix__ // all unices not caught above
    #define HBR_LINUX
    return Hbr_Platform::Linux; // Assuming Unix-like systems are similar to Linux
#elif defined(_POSIX_VERSION)
    return Hbr_Platform::Linux; // POSIX systems
#else
    return Hbr_Platform::Unknown;
#endif
}

constinit const Hbr_Platform Platform = GetPlatform();

enum class RenderAPI : unsigned short {
	OpenGL, Vulkan, DX12, DX11, None
};
