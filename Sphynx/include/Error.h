#pragma once
#include <type_traits>

namespace Sphynx{
    
    enum class ErrorCode : uint32_t {
        OK = 0,
        NOT_INITIALIZED = 1 << 0,
        INVALID_OPERATION = 1 << 1,
        UNSUPPORTED = 1 << 2,
        NOT_IMPLEMENTED = 1 << 3,
        NO_MEMORY = 1 << 4,
        OUT_OF_RANGE = 1 << 5,
        INVALID_ARGUMENT = 1 << 6,
        NOT_AVAILABLE = 1 << 7,
        NO_PERMISSION = 1 << 8,
        FATAL_ERROR = 1 << 9,
        NON_FATAL_ERROR = 1 << 10,
        RUNTIME_ERROR = 1 << 11,
        OPERATION_FAILED = 1 << 12,
        CORE_ERROR = 1 << 13,
        INTERNAL_ERROR = 1 << 14,
        UNKNOWN = 1 << 15,
        FAILED = 1 << 16
    };
    
    // Enable bitwise operators for ErrorCode
    constexpr ErrorCode operator|(ErrorCode lhs, ErrorCode rhs) {
        return static_cast<ErrorCode>(
            static_cast<std::underlying_type<ErrorCode>::type>(lhs) |
            static_cast<std::underlying_type<ErrorCode>::type>(rhs)
        );
    }
    
    constexpr ErrorCode& operator|=(ErrorCode& lhs, ErrorCode rhs) {
        lhs = lhs | rhs;
        return lhs;
    }
    
    constexpr ErrorCode operator&(ErrorCode lhs, ErrorCode rhs) {
        return static_cast<ErrorCode>(
            static_cast<std::underlying_type<ErrorCode>::type>(lhs) &
            static_cast<std::underlying_type<ErrorCode>::type>(rhs)
        );
    }
    
    constexpr ErrorCode& operator&=(ErrorCode& lhs, ErrorCode rhs) {
        lhs = lhs & rhs;
        return lhs;
    }
    
    constexpr bool operator!(ErrorCode e) {
        return e == ErrorCode::OK;
    }

    constexpr bool HasError(ErrorCode combined, ErrorCode specific) {
    return (combined & specific) == specific;
}
}