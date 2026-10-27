#include "Window.h"

#include <cassert>
#include <Windows.h>

namespace PlatformSpace::WindowSpace
{
    namespace
    {
        constexpr wchar_t kWindowClassName[] = L"DirectX11RenderingWindow";
        constexpr wchar_t kWindowTitle[] = L"DirectX 11 Rendering";
        constexpr int     kClientWidth = 1280;
        constexpr int     kClientHeight = 720;

        constexpr DWORD kWindowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
        constexpr DWORD kWindowExtendedStyle = 0;
    } // namespace

    struct Window::Impl final
    {
        ~Impl()
        {
            if (windowHandle_ != nullptr)
            {
                ::DestroyWindow(windowHandle_);
            }

            if (windowClassAtom_ != 0)
            {
                ::UnregisterClassW(kWindowClassName, instance_);
            }
        }

        static LRESULT CALLBACK windowProcedure(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
        {
            // 終了操作をスレッドの終了メッセージへ変換
            switch (message)
            {
                case WM_CLOSE:
                {
                    ::PostQuitMessage(0);
                    return 0;
                }

                case WM_KEYDOWN:
                {
                    if (wParam == VK_ESCAPE)
                    {
                        ::PostQuitMessage(0);
                        return 0;
                    }

                    break;
                }

                default:
                    break;
            }

            // 未処理のメッセージは Windows の標準処理に委譲
            return ::DefWindowProcW(windowHandle, message, wParam, lParam);
        }

        // GetModuleHandleW が返す非所有ハンドル
        HINSTANCE instance_{nullptr};
        HWND      windowHandle_{nullptr};
        ATOM      windowClassAtom_{0};
    };

    Window::Window() : impl_(std::make_unique<Impl>())
    {
    }

    Window::~Window() = default;

    Window::InitializationResult Window::initialize()
    {
        // nullptr は現在の実行ファイルのモジュールを指定
        impl_->instance_ = ::GetModuleHandleW(nullptr);

        if (impl_->instance_ == nullptr)
        {
            const DWORD errorCode = ::GetLastError();

            return {
                InitializationStatus::Failure,
                {
                    "GetModuleHandleW",
                    // Windows 型を公開ヘッダーへ伝播させないため標準整数型へ変換
                    static_cast<std::uint32_t>(errorCode),
                },
            };
        }

        // ウィンドウ生成時に使用する共通の属性を定義
        WNDCLASSEXW windowClass{};
        windowClass.cbSize = sizeof(windowClass);
        windowClass.lpfnWndProc = &Impl::windowProcedure;
        windowClass.hInstance = impl_->instance_;
        windowClass.hCursor = ::LoadCursorW(nullptr, IDC_ARROW);
        windowClass.hbrBackground = ::GetSysColorBrush(COLOR_WINDOW);
        windowClass.lpszClassName = kWindowClassName;

        if (windowClass.hCursor == nullptr)
        {
            const DWORD errorCode = ::GetLastError();

            return {InitializationStatus::Failure, {"LoadCursorW", static_cast<std::uint32_t>(errorCode)}};
        }

        // ウィンドウ生成に先立ってクラス情報を Windows に登録
        impl_->windowClassAtom_ = ::RegisterClassExW(&windowClass);

        if (impl_->windowClassAtom_ == 0)
        {
            const DWORD errorCode = ::GetLastError();

            return {
                InitializationStatus::Failure,
                {
                    "RegisterClassExW",
                    static_cast<std::uint32_t>(errorCode),
                },
            };
        }

        // 指定したクライアント領域からウィンドウ全体のサイズを算出
        RECT windowRect{0, 0, kClientWidth, kClientHeight};

        if (::AdjustWindowRectEx(&windowRect, kWindowStyle, FALSE, kWindowExtendedStyle) == FALSE)
        {
            const DWORD errorCode = ::GetLastError();

            return {
                InitializationStatus::Failure,
                {
                    "AdjustWindowRectEx",
                    static_cast<std::uint32_t>(errorCode),
                },
            };
        }

        const int windowWidth = windowRect.right - windowRect.left;
        const int windowHeight = windowRect.bottom - windowRect.top;

        // 登録済みのウィンドウクラスからウィンドウの実体を生成
        impl_->windowHandle_ = ::CreateWindowExW(
            kWindowExtendedStyle,
            kWindowClassName,
            kWindowTitle,
            kWindowStyle,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            windowWidth,
            windowHeight,
            nullptr,
            nullptr,
            impl_->instance_,
            nullptr
        );

        if (impl_->windowHandle_ == nullptr)
        {
            const DWORD errorCode = ::GetLastError();

            return {InitializationStatus::Failure, {"CreateWindowExW", static_cast<std::uint32_t>(errorCode)}};
        }

        // TODO: Propagate unexpected client-area dimensions after project-level error handling is available
        RECT clientRect{};

        if (::GetClientRect(impl_->windowHandle_, &clientRect) == FALSE)
        {
            const DWORD errorCode = ::GetLastError();

            return {
                InitializationStatus::Failure,
                {
                    "GetClientRect",
                    static_cast<std::uint32_t>(errorCode),
                },
            };
        }

        return {InitializationStatus::Success, {}};
    }

    void Window::show()
    {
        assert(impl_->windowHandle_ != nullptr);
        ::ShowWindow(impl_->windowHandle_, SW_SHOW);
    }

    Window::MessageProcessingResult Window::processNextMessage()
    {
        MSG message{};

        // 次のメッセージを取得して Windows のウィンドウ処理へ配送
        const BOOL getMessageResult = ::GetMessageW(&message, nullptr, 0, 0);

        if (getMessageResult == -1)
        {
            const DWORD errorCode = ::GetLastError();

            return {
                MessageProcessingStatus::Failure,
                0,
                {
                    "GetMessageW",
                    static_cast<std::uint32_t>(errorCode),
                }
            };
        }

        if (getMessageResult == 0)
        {
            return {
                MessageProcessingStatus::Quit,
                static_cast<int>(message.wParam),
                {}
            };
        }

        ::TranslateMessage(&message);
        ::DispatchMessageW(&message);

        return {
            MessageProcessingStatus::Dispatched,
            0,
            {}
        };
    }
} // namespace PlatformSpace::WindowSpace
