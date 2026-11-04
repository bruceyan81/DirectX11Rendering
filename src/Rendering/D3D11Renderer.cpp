#include "D3D11Renderer.h"

#include <d3d11.h>
#include <dxgi1_2.h>
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
        Microsoft::WRL::ComPtr<IDXGISwapChain1>     swapChain_;
        D3D_FEATURE_LEVEL                           featureLevel_{};
    };

    D3D11Renderer::D3D11Renderer() : impl_(std::make_unique<Impl>())
    {
    }

    D3D11Renderer::~D3D11Renderer()
    {
    }

    D3D11RendererInitResult
    D3D11Renderer::initialize(HWND windowHandle, std::uint32_t clientWidth, std::uint32_t clientHeight)
    {
        if (::IsWindow(windowHandle) == FALSE || clientWidth == 0 || clientHeight == 0)
        {
            return {D3D11RendererInitStatus::InvalidWindowParameters, {}};
        }

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

        // 作成済みデバイスと同一系統の DXGI Factory を取得
        Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;

        const HRESULT dxgiDeviceResult = device.As(&dxgiDevice);

        if (FAILED(dxgiDeviceResult))
        {
            return {
                D3D11RendererInitStatus::Failure,
                {
                    "ID3D11Device::QueryInterface(IDXGIDevice)",
                    static_cast<std::uint32_t>(dxgiDeviceResult),
                },
            };
        }

        Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;

        const HRESULT adapterResult = dxgiDevice->GetAdapter(adapter.GetAddressOf());

        if (FAILED(adapterResult))
        {
            return {
                D3D11RendererInitStatus::Failure,
                {
                    "IDXGIDevice::GetAdapter",
                    static_cast<std::uint32_t>(adapterResult),
                },
            };
        }

        Microsoft::WRL::ComPtr<IDXGIFactory2> factory;

        const HRESULT factoryResult = adapter->GetParent(IID_PPV_ARGS(factory.GetAddressOf()));

        if (FAILED(factoryResult))
        {
            return {
                D3D11RendererInitStatus::Failure,
                {
                    "IDXGIAdapter::GetParent(IDXGIFactory2)",
                    static_cast<std::uint32_t>(factoryResult),
                },
            };
        }

        DXGI_SWAP_CHAIN_DESC1 swapChainDescription{};
        swapChainDescription.Width = clientWidth;
        swapChainDescription.Height = clientHeight;
        swapChainDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDescription.Stereo = FALSE;
        swapChainDescription.SampleDesc.Count = 1;
        swapChainDescription.SampleDesc.Quality = 0;
        swapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDescription.BufferCount = 2;
        swapChainDescription.Scaling = DXGI_SCALING_STRETCH;
        swapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDescription.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        swapChainDescription.Flags = 0;

        Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain;

        const HRESULT swapChainResult = factory->CreateSwapChainForHwnd(
            device.Get(),
            windowHandle,
            &swapChainDescription,
            nullptr,
            nullptr,
            swapChain.GetAddressOf()
        );

        if (FAILED(swapChainResult))
        {
            return {
                D3D11RendererInitStatus::Failure,
                {
                    "IDXGIFactory2::CreateSwapChainForHwnd",
                    static_cast<std::uint32_t>(swapChainResult),
                },
            };
        }

        // DXGI 既定の Alt+Enter 全画面切り替えを無効化
        const HRESULT windowAssociationResult = factory->MakeWindowAssociation(windowHandle, DXGI_MWA_NO_ALT_ENTER);

        if (FAILED(windowAssociationResult))
        {
            return {
                D3D11RendererInitStatus::Failure,
                {
                    "IDXGIFactory2::MakeWindowAssociation",
                    static_cast<std::uint32_t>(windowAssociationResult),
                },
            };
        }

        impl_->device_ = std::move(device);
        impl_->immediateContext_ = std::move(immediateContext);
        impl_->swapChain_ = std::move(swapChain);
        impl_->featureLevel_ = featureLevel;

        const wchar_t* featureLevelMessage =
            featureLevel == D3D_FEATURE_LEVEL_11_1 ? L"D3D11 Feature Level: 11_1\n" : L"D3D11 Feature Level: 11_0\n";

        ::OutputDebugStringW(featureLevelMessage);

        return {D3D11RendererInitStatus::Success, {}};
    }

} // namespace RenderingSpace
