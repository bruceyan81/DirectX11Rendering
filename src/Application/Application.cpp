#include "Application.h"

#include "Platform/Window/Window.h"

#include <cstdlib>
#include <Windows.h>

#include <string>

namespace ApplicationSpace
{
    namespace
    {
        void showWin32Error(const PlatformSpace::WindowSpace::Window::Win32Error& error)
        {
            const std::wstring functionName(error.functionName_.begin(), error.functionName_.end());

            const std::wstring message = functionName + L" failed\nError code: " + std::to_wstring(error.errorCode_);

            ::MessageBoxW(nullptr, message.c_str(), L"DirectX11Rendering Error", MB_OK | MB_ICONERROR);
        }
    } // namespace

    int Application::run()
    {
        PlatformSpace::WindowSpace::Window window;

        const auto initializationResult = window.initialize();

        if (initializationResult.status_ == PlatformSpace::WindowSpace::Window::InitializationStatus::Failure)
        {
            showWin32Error(initializationResult.error_);
            return EXIT_FAILURE;
        }

        window.show();

        while (true)
        {
            const PlatformSpace::WindowSpace::Window::MessageProcessingResult messageResult =
                window.processNextMessage();

            if (messageResult.status_ == PlatformSpace::WindowSpace::Window::MessageProcessingStatus::Failure)
            {
                showWin32Error(messageResult.error_);
                return EXIT_FAILURE;
            }

            if (messageResult.status_ == PlatformSpace::WindowSpace::Window::MessageProcessingStatus::Quit)
            {
                return messageResult.exitCode_;
            }
        }
    }
} // namespace ApplicationSpace
