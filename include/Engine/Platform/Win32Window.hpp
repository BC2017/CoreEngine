#pragma once

#include <cstdint>
#include <string>
#include <string_view>

namespace HFEngine::Platform
{
    struct WindowDesc
    {
        std::wstring title = L"HFEngine";
        std::uint32_t width = 1280;
        std::uint32_t height = 720;
    };

    class Win32Window
    {
    public:
        Win32Window() = default;
        ~Win32Window();

        Win32Window(const Win32Window&) = delete;
        Win32Window& operator=(const Win32Window&) = delete;

        [[nodiscard]] bool Create(const WindowDesc& desc);
        void Destroy() noexcept;
        [[nodiscard]] bool PumpMessages() noexcept;

        [[nodiscard]] void* NativeHandle() const noexcept;
        [[nodiscard]] std::uint32_t Width() const noexcept;
        [[nodiscard]] std::uint32_t Height() const noexcept;
        [[nodiscard]] bool IsOpen() const noexcept;

    private:
        void* hwnd_ = nullptr;
        std::uint32_t width_ = 0;
        std::uint32_t height_ = 0;
        bool open_ = false;
    };
}
