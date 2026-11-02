#include "D3D11Renderer.h"

#include <d3d11.h>
#include <wrl/client.h>

#include <Windows.h>

#include <iterator>
#include <utility>

namespace RenderingSpace
{
    namespace
    {
        constexpr D3D_FEATURE_LEVEL kRequestedFeatureLevels[]{D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0};
    } // namespace

    struct D3D11Renderer::Impl final
    {
        Microsoft::WRL::ComPtr<ID3D11Device>        device_;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediateContext_;
        D3D_FEATURE_LEVEL                           featureLevel_{};
    };

    D3D11Renderer::D3D11Renderer() : impl_(std::make_unique<Impl>())
    {
    }

    D3D11Renderer::~D3D11Renderer()
    {
    }

    D3D11RendererInitResult D3D11Renderer::initialize()
    {
        Microsoft::WRL::ComPtr<ID3D11Device>        device;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediateContext;
        D3D_FEATURE_LEVEL                           featureLevel{};

        UINT creationFlags = 0;

#if defined(_DEBUG)
        creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        const HRESULT result = ::D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            creationFlags,
            kRequestedFeatureLevels,
            static_cast<UINT>(std::size(kRequestedFeatureLevels)),
            D3D11_SDK_VERSION,
            device.GetAddressOf(),
            &featureLevel,
            immediateContext.GetAddressOf()
        );

        if (FAILED(result))
        {
            return {
                D3D11RendererInitStatus::Failure,
                {
                    "D3D11CreateDevice",
                    static_cast<std::uint32_t>(result),
                },
            };
        }

        impl_->device_ = std::move(device);
        impl_->immediateContext_ = std::move(immediateContext);
        impl_->featureLevel_ = featureLevel;

        const wchar_t* featureLevelMessage =
            featureLevel == D3D_FEATURE_LEVEL_11_1 ? L"D3D11 Feature Level: 11_1\n" : L"D3D11 Feature Level: 11_0\n";

        ::OutputDebugStringW(featureLevelMessage);

        return {D3D11RendererInitStatus::Success, {}};
    }

} // namespace RenderingSpace
