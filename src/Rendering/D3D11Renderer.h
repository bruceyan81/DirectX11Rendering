#pragma once

#include <Windows.h>

#include <cstdint>
#include <memory>
#include <string_view>

namespace RenderingSpace
{
    struct D3D11HResultError final
    {
        std::string_view functionName_;
        std::uint32_t    errorCode_{0};
    };

    enum class D3D11RendererInitStatus
    {
        Success,
        Failure,
        InvalidWindowParameters
    };

    struct D3D11RendererInitResult final
    {
        D3D11RendererInitStatus status_{D3D11RendererInitStatus::Failure};
        D3D11HResultError       error_;
    };

    class D3D11Renderer final
    {
    public:
        D3D11Renderer();
        ~D3D11Renderer();

        D3D11Renderer(const D3D11Renderer&) = delete;
        D3D11Renderer& operator=(const D3D11Renderer&) = delete;
        D3D11Renderer(D3D11Renderer&&) = delete;
        D3D11Renderer& operator=(D3D11Renderer&&) = delete;

        [[nodiscard]] D3D11RendererInitResult
        initialize(HWND windowHandle, std::uint32_t clientWidth, std::uint32_t clientHeight);

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };
} // namespace RenderingSpace
