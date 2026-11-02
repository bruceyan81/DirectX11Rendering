#include "Application.h"

#include "Platform/Window/Window.h"
#include "Rendering/D3D11Renderer.h"

#include <Windows.h>

#include <cstdlib>
#include <format>
#include <string>

namespace ApplicationSpace
{
    namespace
    {
        void showWin32Error(const PlatformSpace::WindowSpace::WindowWin32Error& error)
        {
            const std::wstring functionName(error.functionName_.begin(), error.functionName_.end());

            const std::wstring message = functionName + L" failed\nError code: " + std::to_wstring(error.errorCode_);

            ::MessageBoxW(nullptr, message.c_str(), L"DirectX11Rendering Error", MB_OK | MB_ICONERROR);
        }

        void showD3D11Error(const RenderingSpace::D3D11HResultError& error)
        {
            const std::wstring functionName(error.functionName_.begin(), error.functionName_.end());

            // {} に API 名を挿入し {:08X} で HRESULT をゼロ埋めした8桁の大文字16進数に変換
            const std::wstring message = std::format(L"{} failed\nHRESULT: 0x{:08X}", functionName, error.errorCode_);

            ::MessageBoxW(nullptr, message.c_str(), L"DirectX11Rendering Error", MB_OK | MB_ICONERROR);
        }
    } // namespace

    int Application::run()
    {
        PlatformSpace::WindowSpace::Window window;

        const auto windowInitializationResult = window.initialize();

        if (windowInitializationResult.status_ == PlatformSpace::WindowSpace::WindowInitializationStatus::Failure)
        {
            showWin32Error(windowInitializationResult.error_);
            return EXIT_FAILURE;
        }

        RenderingSpace::D3D11Renderer renderer;

        const auto rendererInitializationResult = renderer.initialize();

        if (rendererInitializationResult.status_ == RenderingSpace::D3D11RendererInitStatus::Failure)
        {
            showD3D11Error(rendererInitializationResult.error_);
            return EXIT_FAILURE;
        }

        window.show();

        while (true)
        {
            const PlatformSpace::WindowSpace::WindowMessageProcessingResult messageResult =
                window.processNextMessage();

            if (messageResult.status_ == PlatformSpace::WindowSpace::WindowMessageProcessingStatus::Failure)
            {
                showWin32Error(messageResult.error_);
                return EXIT_FAILURE;
            }

            if (messageResult.status_ == PlatformSpace::WindowSpace::WindowMessageProcessingStatus::Quit)
            {
                return messageResult.exitCode_;
            }
        }
    }
} // namespace ApplicationSpace
