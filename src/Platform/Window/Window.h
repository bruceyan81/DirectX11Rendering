#pragma once

#include <cstdint>
#include <memory>
#include <string_view>

namespace RenderingSpace
{
    class D3D11Renderer;
}

namespace PlatformSpace::WindowSpace
{
    struct WindowWin32Error final
    {
        std::string_view functionName_;
        std::uint32_t    errorCode_{0};
    };

    enum class WindowInitializationStatus
    {
        Success,
        Failure,
        InvalidClientArea
    };

    struct WindowInitializationResult final
    {
        WindowInitializationStatus status_{WindowInitializationStatus::Failure};
        WindowWin32Error           error_;
    };

    enum class WindowMessageProcessingStatus
    {
        Dispatched,
        Quit,
        Failure
    };

    struct WindowMessageProcessingResult final
    {
        WindowMessageProcessingStatus status_{WindowMessageProcessingStatus::Failure};
        int                           exitCode_{0};
        WindowWin32Error              error_;
    };

    class Window final
    {
    public:
        Window();
        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;
        Window(Window&&) = delete;
        Window& operator=(Window&&) = delete;

        [[nodiscard]] WindowInitializationResult    initialize();
        [[nodiscard]] WindowMessageProcessingResult processNextMessage();

        void show();

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;

        friend class RenderingSpace::D3D11Renderer;
    };
} // namespace PlatformSpace::WindowSpace
