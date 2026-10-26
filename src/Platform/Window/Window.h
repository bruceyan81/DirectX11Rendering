#pragma once

#include <cstdint>
#include <memory>
#include <string_view>

namespace PlatformSpace::WindowSpace
{
    class Window final
    {
    public:
        struct Win32Error final
        {
            std::string_view functionName_;
            std::uint32_t    errorCode_{0};
        };

        enum class InitializationStatus
        {
            Success,
            Failure
        };

        struct InitializationResult final
        {
            InitializationStatus status_{InitializationStatus::Failure};
            Win32Error           error_;
        };

        enum class MessageProcessingStatus
        {
            Dispatched,
            Quit,
            Failure
        };

        struct MessageProcessingResult final
        {
            MessageProcessingStatus status_{MessageProcessingStatus::Failure};
            int                     exitCode_{0};
            Win32Error              error_;
        };

        Window();
        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;
        Window(Window&&) = delete;
        Window& operator=(Window&&) = delete;

        [[nodiscard]] InitializationResult    initialize();
        void                                  show();
        [[nodiscard]] MessageProcessingResult processNextMessage();

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };
} // namespace PlatformSpace::WindowSpace
