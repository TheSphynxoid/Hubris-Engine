#pragma once
#ifdef SPH_WINDOW
#else
#define SPH_WINDOW
#include "glm/glm.hpp"
/**
 * @brief Contains all graphics related classes and structs.
 */
namespace Sphynx::Graphics
{
    /**
     * @brief Screen-space Dimensions.
     */
    struct Viewport{
        int Xpos,Ypos,Width,Height;
    };
    class Window{
    public:
        // Window() = default;
        virtual ~Window() = 0;
        /**
         * @brief Intializes the rendering backend and creates the Window.
         * @exception Can throw.
         */
        virtual void Init() = 0;
        /**
         * @brief Poll events.
         */
        virtual void Update()noexcept = 0;
        /**
         * @brief Closes the Window and frees resources.
         */
        virtual void Close()noexcept = 0;
        /**
         * @brief Gets the window dimension.
         * 
         * @return The Dimensions wrapped in a Viewport Object.
         */
        virtual Viewport GetViewport()const noexcept = 0;
        /**
         * @return returns true if the Window is valid.
         */
        virtual bool IsValid()const noexcept = 0;

        static void Create(const glm::ivec2& size, const char* title);
    };
} // namespace Sphynx::Graphics

#endif