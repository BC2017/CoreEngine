#include "Engine/RHI/DX12/DX12TriangleRenderer.hpp"

#include "Engine/RHI/RendererBackend.hpp"
#include "Engine/Tools/RuntimeDebugOverlay.hpp"

#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

#include <cstddef>
#include <cstdio>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

using Microsoft::WRL::ComPtr;

namespace HFEngine::RHI::DX12
{
    namespace
    {
        constexpr UINT FrameCount = 2;

        std::string ToUtf8(const wchar_t* text)
        {
            if (text == nullptr || text[0] == L'\0')
            {
                return {};
            }

            const int required = WideCharToMultiByte(CP_UTF8, 0, text, -1, nullptr, 0, nullptr, nullptr);
            if (required <= 1)
            {
                return {};
            }

            std::string output(static_cast<std::size_t>(required - 1), '\0');
            WideCharToMultiByte(CP_UTF8, 0, text, -1, output.data(), required, nullptr, nullptr);
            return output;
        }

        std::string ExecutableDirectory()
        {
            wchar_t path[MAX_PATH]{};
            const DWORD size = GetModuleFileNameW(nullptr, path, MAX_PATH);
            if (size == 0 || size == MAX_PATH)
            {
                return ".";
            }

            std::wstring widePath(path, size);
            const std::size_t slash = widePath.find_last_of(L"\\/");
            if (slash != std::wstring::npos)
            {
                widePath.resize(slash);
            }

            return ToUtf8(widePath.c_str());
        }

        std::vector<std::uint8_t> ReadBinary(const std::string& path)
        {
            std::ifstream file(path, std::ios::binary | std::ios::ate);
            if (!file)
            {
                return {};
            }

            const std::streamsize size = file.tellg();
            if (size <= 0)
            {
                return {};
            }

            std::vector<std::uint8_t> data(static_cast<std::size_t>(size));
            file.seekg(0, std::ios::beg);
            file.read(reinterpret_cast<char*>(data.data()), size);
            return data;
        }

        bool Succeeded(HRESULT result, std::string& message, const char* operation)
        {
            if (SUCCEEDED(result))
            {
                return true;
            }

            message = operation;
            message += " failed with HRESULT 0x";

            char buffer[16]{};
            std::snprintf(buffer, sizeof(buffer), "%08X", static_cast<unsigned int>(result));
            message += buffer;
            return false;
        }

        void Transition(
            ID3D12GraphicsCommandList* commandList,
            ID3D12Resource* resource,
            D3D12_RESOURCE_STATES before,
            D3D12_RESOURCE_STATES after)
        {
            D3D12_RESOURCE_BARRIER barrier{};
            barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrier.Transition.pResource = resource;
            barrier.Transition.StateBefore = before;
            barrier.Transition.StateAfter = after;
            barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            commandList->ResourceBarrier(1, &barrier);
        }

        struct Dx12State
        {
            ComPtr<IDXGIFactory6> factory;
            ComPtr<IDXGIAdapter1> adapter;
            ComPtr<ID3D12Device> device;
            ComPtr<ID3D12CommandQueue> commandQueue;
            ComPtr<IDXGISwapChain3> swapchain;
            ComPtr<ID3D12DescriptorHeap> rtvHeap;
            ComPtr<ID3D12DescriptorHeap> imguiSrvHeap;
            ComPtr<ID3D12Resource> renderTargets[FrameCount];
            ComPtr<ID3D12CommandAllocator> commandAllocator;
            ComPtr<ID3D12RootSignature> rootSignature;
            ComPtr<ID3D12PipelineState> pipelineState;
            ComPtr<ID3D12GraphicsCommandList> commandList;
            ComPtr<ID3D12Fence> fence;
            HANDLE fenceEvent = nullptr;
            UINT rtvDescriptorSize = 0;
            UINT frameIndex = 0;
            UINT64 fenceValue = 0;
            std::string adapterName;
            bool imguiDescriptorAllocated = false;

            ~Dx12State()
            {
                if (fenceEvent != nullptr)
                {
                    CloseHandle(fenceEvent);
                }
            }
        };

        void AllocateImGuiDescriptor(
            ImGui_ImplDX12_InitInfo* info,
            D3D12_CPU_DESCRIPTOR_HANDLE* outCpuDescHandle,
            D3D12_GPU_DESCRIPTOR_HANDLE* outGpuDescHandle)
        {
            auto* state = static_cast<Dx12State*>(info->UserData);
            state->imguiDescriptorAllocated = true;
            *outCpuDescHandle = state->imguiSrvHeap->GetCPUDescriptorHandleForHeapStart();
            *outGpuDescHandle = state->imguiSrvHeap->GetGPUDescriptorHandleForHeapStart();
        }

        void FreeImGuiDescriptor(
            ImGui_ImplDX12_InitInfo* info,
            D3D12_CPU_DESCRIPTOR_HANDLE,
            D3D12_GPU_DESCRIPTOR_HANDLE)
        {
            auto* state = static_cast<Dx12State*>(info->UserData);
            state->imguiDescriptorAllocated = false;
        }

        bool EnableDebugLayerIfRequested(bool enabled, std::string& message)
        {
            if (!enabled)
            {
                return true;
            }

            ComPtr<ID3D12Debug> debugController;
            const HRESULT result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
            if (FAILED(result))
            {
                message = "DX12 debug layer requested but unavailable; install Graphics Tools or use --no-validation";
                return false;
            }

            debugController->EnableDebugLayer();
            return true;
        }

        bool SelectAdapter(Dx12State& state, std::string& message)
        {
            for (UINT adapterIndex = 0;
                 state.factory->EnumAdapterByGpuPreference(
                     adapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&state.adapter)) !=
                 DXGI_ERROR_NOT_FOUND;
                 ++adapterIndex)
            {
                DXGI_ADAPTER_DESC1 desc{};
                state.adapter->GetDesc1(&desc);
                if ((desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) != 0)
                {
                    continue;
                }

                if (SUCCEEDED(D3D12CreateDevice(state.adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&state.device))))
                {
                    state.adapterName = ToUtf8(desc.Description);
                    return true;
                }
            }

            ComPtr<IDXGIAdapter> warpAdapter;
            if (!Succeeded(state.factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)), message, "EnumWarpAdapter"))
            {
                return false;
            }

            if (!Succeeded(
                    D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&state.device)),
                    message,
                    "D3D12CreateDevice(WARP)"))
            {
                return false;
            }

            DXGI_ADAPTER_DESC desc{};
            warpAdapter->GetDesc(&desc);
            state.adapterName = ToUtf8(desc.Description);
            return true;
        }

        bool InitializeDevice(const Core::EngineConfig& config, Platform::Win32Window& window, Dx12State& state, std::string& message)
        {
            if (!EnableDebugLayerIfRequested(config.enableValidation, message))
            {
                return false;
            }

            UINT factoryFlags = config.enableValidation ? DXGI_CREATE_FACTORY_DEBUG : 0;
            if (!Succeeded(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&state.factory)), message, "CreateDXGIFactory2"))
            {
                factoryFlags = 0;
                if (!Succeeded(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&state.factory)), message, "CreateDXGIFactory2"))
                {
                    return false;
                }
            }

            if (!SelectAdapter(state, message))
            {
                return false;
            }

            D3D12_COMMAND_QUEUE_DESC queueDesc{};
            queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
            if (!Succeeded(
                    state.device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&state.commandQueue)),
                    message,
                    "CreateCommandQueue"))
            {
                return false;
            }

            DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};
            swapchainDesc.BufferCount = FrameCount;
            swapchainDesc.Width = window.Width();
            swapchainDesc.Height = window.Height();
            swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
            swapchainDesc.SampleDesc.Count = 1;

            ComPtr<IDXGISwapChain1> swapchain;
            if (!Succeeded(
                    state.factory->CreateSwapChainForHwnd(
                        state.commandQueue.Get(),
                        static_cast<HWND>(window.NativeHandle()),
                        &swapchainDesc,
                        nullptr,
                        nullptr,
                        &swapchain),
                    message,
                    "CreateSwapChainForHwnd"))
            {
                return false;
            }

            state.factory->MakeWindowAssociation(static_cast<HWND>(window.NativeHandle()), DXGI_MWA_NO_ALT_ENTER);
            if (!Succeeded(swapchain.As(&state.swapchain), message, "Query IDXGISwapChain3"))
            {
                return false;
            }

            state.frameIndex = state.swapchain->GetCurrentBackBufferIndex();

            D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
            rtvHeapDesc.NumDescriptors = FrameCount;
            rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            if (!Succeeded(
                    state.device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&state.rtvHeap)),
                    message,
                    "CreateDescriptorHeap(RTV)"))
            {
                return false;
            }

            D3D12_DESCRIPTOR_HEAP_DESC imguiHeapDesc{};
            imguiHeapDesc.NumDescriptors = 1;
            imguiHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            imguiHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            if (!Succeeded(
                    state.device->CreateDescriptorHeap(&imguiHeapDesc, IID_PPV_ARGS(&state.imguiSrvHeap)),
                    message,
                    "CreateDescriptorHeap(ImGui SRV)"))
            {
                return false;
            }

            state.rtvDescriptorSize = state.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
            D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = state.rtvHeap->GetCPUDescriptorHandleForHeapStart();
            for (UINT frame = 0; frame < FrameCount; ++frame)
            {
                if (!Succeeded(
                        state.swapchain->GetBuffer(frame, IID_PPV_ARGS(&state.renderTargets[frame])),
                        message,
                        "GetBuffer"))
                {
                    return false;
                }

                state.device->CreateRenderTargetView(state.renderTargets[frame].Get(), nullptr, rtvHandle);
                rtvHandle.ptr += state.rtvDescriptorSize;
            }

            if (!Succeeded(
                    state.device->CreateCommandAllocator(
                        D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&state.commandAllocator)),
                    message,
                    "CreateCommandAllocator"))
            {
                return false;
            }

            return true;
        }

        bool InitializeImGui(const Core::EngineConfig& config, Platform::Win32Window& window, Dx12State& state, std::string& message)
        {
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGui::StyleColorsDark();

            if (!ImGui_ImplWin32_Init(static_cast<HWND>(window.NativeHandle())))
            {
                message = "ImGui_ImplWin32_Init failed";
                return false;
            }

            ImGui_ImplDX12_InitInfo initInfo;
            initInfo.Device = state.device.Get();
            initInfo.CommandQueue = state.commandQueue.Get();
            initInfo.NumFramesInFlight = FrameCount;
            initInfo.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
            initInfo.DSVFormat = DXGI_FORMAT_UNKNOWN;
            initInfo.UserData = &state;
            initInfo.SrvDescriptorHeap = state.imguiSrvHeap.Get();
            initInfo.SrvDescriptorAllocFn = AllocateImGuiDescriptor;
            initInfo.SrvDescriptorFreeFn = FreeImGuiDescriptor;

            if (!ImGui_ImplDX12_Init(&initInfo))
            {
                message = "ImGui_ImplDX12_Init failed";
                return false;
            }

            ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
            (void)config;
            return true;
        }

        void ShutdownImGui()
        {
            if (ImGui::GetCurrentContext() != nullptr)
            {
                ImGui_ImplDX12_Shutdown();
                ImGui_ImplWin32_Shutdown();
                ImGui::DestroyContext();
            }
        }

        bool InitializePipeline(Dx12State& state, std::string& message)
        {
            D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
            rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

            ComPtr<ID3DBlob> signature;
            ComPtr<ID3DBlob> error;
            if (!Succeeded(
                    D3D12SerializeRootSignature(
                        &rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error),
                    message,
                    "D3D12SerializeRootSignature"))
            {
                if (error)
                {
                    message += ": ";
                    message += static_cast<const char*>(error->GetBufferPointer());
                }
                return false;
            }

            if (!Succeeded(
                    state.device->CreateRootSignature(
                        0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&state.rootSignature)),
                    message,
                    "CreateRootSignature"))
            {
                return false;
            }

            const std::string shaderDir = ExecutableDirectory();
            const std::vector<std::uint8_t> vertexShader = ReadBinary(shaderDir + "\\Triangle.vs.dxil");
            const std::vector<std::uint8_t> pixelShader = ReadBinary(shaderDir + "\\Triangle.ps.dxil");
            if (vertexShader.empty() || pixelShader.empty())
            {
                message = "DXIL shader file is missing or empty";
                return false;
            }

            D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
            pipelineDesc.InputLayout = { nullptr, 0 };
            pipelineDesc.pRootSignature = state.rootSignature.Get();
            pipelineDesc.VS = { vertexShader.data(), vertexShader.size() };
            pipelineDesc.PS = { pixelShader.data(), pixelShader.size() };
            pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
            pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
            pipelineDesc.RasterizerState.DepthClipEnable = TRUE;
            pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
            pipelineDesc.DepthStencilState.DepthEnable = FALSE;
            pipelineDesc.DepthStencilState.StencilEnable = FALSE;
            pipelineDesc.SampleMask = UINT_MAX;
            pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            pipelineDesc.NumRenderTargets = 1;
            pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
            pipelineDesc.SampleDesc.Count = 1;

            if (!Succeeded(
                    state.device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&state.pipelineState)),
                    message,
                    "CreateGraphicsPipelineState"))
            {
                return false;
            }

            if (!Succeeded(
                    state.device->CreateCommandList(
                        0,
                        D3D12_COMMAND_LIST_TYPE_DIRECT,
                        state.commandAllocator.Get(),
                        state.pipelineState.Get(),
                        IID_PPV_ARGS(&state.commandList)),
                    message,
                    "CreateCommandList"))
            {
                return false;
            }

            state.commandList->Close();
            return true;
        }

        bool InitializeAssets(Dx12State& state, std::string& message)
        {
            if (!Succeeded(state.device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&state.fence)), message, "CreateFence"))
            {
                return false;
            }

            state.fenceValue = 1;
            state.fenceEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
            if (state.fenceEvent == nullptr)
            {
                message = "CreateEvent failed for DX12 fence";
                return false;
            }

            return true;
        }

        bool WaitForGpu(Dx12State& state, std::string& message)
        {
            const UINT64 fence = state.fenceValue;
            if (!Succeeded(state.commandQueue->Signal(state.fence.Get(), fence), message, "Signal(fence)"))
            {
                return false;
            }

            ++state.fenceValue;

            if (state.fence->GetCompletedValue() < fence)
            {
                if (!Succeeded(state.fence->SetEventOnCompletion(fence, state.fenceEvent), message, "SetEventOnCompletion"))
                {
                    return false;
                }

                WaitForSingleObject(state.fenceEvent, INFINITE);
            }

            state.frameIndex = state.swapchain->GetCurrentBackBufferIndex();
            return true;
        }

        bool RenderFrame(
            const Core::EngineConfig& config,
            Dx12State& state,
            Platform::Win32Window& window,
            unsigned int framesRendered,
            std::string& message)
        {
            if (!Succeeded(state.commandAllocator->Reset(), message, "CommandAllocator::Reset"))
            {
                return false;
            }

            if (!Succeeded(
                    state.commandList->Reset(state.commandAllocator.Get(), state.pipelineState.Get()),
                    message,
                    "CommandList::Reset"))
            {
                return false;
            }

            state.commandList->SetGraphicsRootSignature(state.rootSignature.Get());

            D3D12_VIEWPORT viewport{};
            viewport.Width = static_cast<float>(window.Width());
            viewport.Height = static_cast<float>(window.Height());
            viewport.MaxDepth = 1.0f;

            D3D12_RECT scissorRect{ 0, 0, static_cast<LONG>(window.Width()), static_cast<LONG>(window.Height()) };
            state.commandList->RSSetViewports(1, &viewport);
            state.commandList->RSSetScissorRects(1, &scissorRect);

            Transition(
                state.commandList.Get(),
                state.renderTargets[state.frameIndex].Get(),
                D3D12_RESOURCE_STATE_PRESENT,
                D3D12_RESOURCE_STATE_RENDER_TARGET);

            D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = state.rtvHeap->GetCPUDescriptorHandleForHeapStart();
            rtvHandle.ptr += static_cast<SIZE_T>(state.frameIndex) * state.rtvDescriptorSize;

            constexpr float clearColor[] = { 0.025f, 0.035f, 0.055f, 1.0f };
            state.commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
            state.commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
            state.commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            state.commandList->DrawInstanced(3, 1, 0, 0);

            ImGui_ImplDX12_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();
            Tools::DrawRuntimeDebugOverlay({
                "DirectX 12",
                state.adapterName,
                config.enableValidation,
                framesRendered,
                0.0f,
            });
            ImGui::Render();

            ID3D12DescriptorHeap* descriptorHeaps[] = { state.imguiSrvHeap.Get() };
            state.commandList->SetDescriptorHeaps(1, descriptorHeaps);
            ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), state.commandList.Get());

            Transition(
                state.commandList.Get(),
                state.renderTargets[state.frameIndex].Get(),
                D3D12_RESOURCE_STATE_RENDER_TARGET,
                D3D12_RESOURCE_STATE_PRESENT);

            if (!Succeeded(state.commandList->Close(), message, "CommandList::Close"))
            {
                return false;
            }

            ID3D12CommandList* commandLists[] = { state.commandList.Get() };
            state.commandQueue->ExecuteCommandLists(1, commandLists);

            if (!Succeeded(state.swapchain->Present(1, 0), message, "Swapchain::Present"))
            {
                return false;
            }

            return WaitForGpu(state, message);
        }
    }

    TriangleRunResult RunTriangleSandbox(const Core::EngineConfig& config, Platform::Win32Window& window)
    {
        TriangleRunResult result;

        Dx12State state;
        if (!InitializeDevice(config, window, state, result.message) ||
            !InitializePipeline(state, result.message) ||
            !InitializeAssets(state, result.message) ||
            !InitializeImGui(config, window, state, result.message))
        {
            ShutdownImGui();
            return result;
        }

        result.adapterName = state.adapterName;

        while (window.PumpMessages())
        {
            if (!RenderFrame(config, state, window, result.framesRendered, result.message))
            {
                ShutdownImGui();
                return result;
            }

            ++result.framesRendered;
            if (config.maxFrames != 0 && result.framesRendered >= config.maxFrames)
            {
                break;
            }
        }

        if (!WaitForGpu(state, result.message))
        {
            ShutdownImGui();
            return result;
        }

        ShutdownImGui();
        result.success = true;
        result.message = "ok";
        return result;
    }
}
