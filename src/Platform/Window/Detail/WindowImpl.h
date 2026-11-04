#pragma once

#include "Platform/Window/Window.h"

#include <Windows.h>

#include <cstdint>

namespace PlatformSpace::WindowSpace
{
    struct Window::Impl final
    {
        ~Impl();

        static LRESULT CALLBACK windowProcedure(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);

        HINSTANCE     instance_{nullptr};
        HWND          windowHandle_{nullptr};
        ATOM          windowClassAtom_{0};
        std::uint32_t clientWidth_{0};
        std::uint32_t clientHeight_{0};
    };
} // namespace PlatformSpace::WindowSpace
