#pragma once
#include "ThreadPool.h"

namespace Sphynx::Core {
    //This is used for Core Event Bus
    template <typename Event>
    class StaticEventBus {
    public:
        using HandlerFunc = void(*)(const Event&);

        static void Subscribe(HandlerFunc func) {
            handlers.push_back(func);
        }

        static void Dispatch(const Event& event) {
            for (auto handler : handlers)
                handler(event);
        }

    private:
        static inline std::vector<HandlerFunc> handlers;
    };

    struct OnUpdate {

    };

    struct OnStart {

    };
}