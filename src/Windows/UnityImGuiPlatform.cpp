#include "D3D11Backend.hpp"
#include "D3D12Backend.hpp"
#include "PluginInternal.h"
#include <UnityPluginAPI/IUnityGraphics.h>
#include <UnityPluginAPI/IUnityGraphicsD3D11.h>
#include <UnityPluginAPI/IUnityGraphicsD3D12.h>
#include <UnityPluginAPI/IUnityInterface.h>
#include <d3d11.h>
#include <d3d12.h>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <windows.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

namespace
{
    IUnityInterfaces* gUnityInterfaces = nullptr;
    IUnityGraphicsD3D11* gD3D11Graphics = nullptr;
    IUnityGraphicsD3D12v6* gD3D12Graphics = nullptr;
    UnityGfxRenderer gActiveRenderer = kUnityGfxRendererNull;

    HWND    gHookedHwnd        = nullptr;
    WNDPROC gOriginalWndProc   = nullptr;
    ID3D12CommandAllocator* gD3D12Allocator = nullptr;
    ID3D12GraphicsCommandList* gD3D12CmdList = nullptr;

    LRESULT CALLBACK CustomWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) return TRUE;

        switch (msg)
        {
        case WM_POINTERDOWN:
        case WM_POINTERUPDATE:
        case WM_POINTERUP:
        {
            if (ImGui::GetCurrentContext() == nullptr) break;
            UINT32 pointerId = GET_POINTERID_WPARAM(wParam);
            POINTER_INFO info{};
            if (GetPointerInfo(pointerId, &info) &&
                (info.pointerFlags & POINTER_FLAG_PRIMARY) &&
                (info.pointerType == PT_TOUCH || info.pointerType == PT_PEN))
            {
                POINT pt = info.ptPixelLocation;
                ScreenToClient(hWnd, &pt);
                ImGuiIO& io = ImGui::GetIO();
                io.AddMouseSourceEvent(info.pointerType == PT_TOUCH
                                           ? ImGuiMouseSource_TouchScreen
                                           : ImGuiMouseSource_Pen);
                io.AddMousePosEvent(static_cast<float>(pt.x), static_cast<float>(pt.y));
                if (msg == WM_POINTERDOWN)
                    io.AddMouseButtonEvent(0, true);
                else if (msg == WM_POINTERUP)
                    io.AddMouseButtonEvent(0, false);
            }
            break;
        }
        }

        return CallWindowProc(gOriginalWndProc, hWnd, msg, wParam, lParam);
    }

    ID3D12GraphicsCommandList* GetD3D12CommandList()
    {
        auto* device = gD3D12Graphics->GetDevice();

        if (gD3D12CmdList) { gD3D12CmdList->Release(); gD3D12CmdList = nullptr; }

        if (!gD3D12Allocator)
            device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                           IID_PPV_ARGS(&gD3D12Allocator));
        else
            gD3D12Allocator->Reset();

        device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, gD3D12Allocator, nullptr,
                                  IID_PPV_ARGS(&gD3D12CmdList));
        return gD3D12CmdList;
    }

    void ExecuteD3D12CommandList(ID3D12GraphicsCommandList* cmdList)
    {
        cmdList->Close();
        ID3D12CommandList* lists[] = {cmdList};
        gD3D12Graphics->GetCommandQueue()->ExecuteCommandLists(1, lists);
    }

    UnityD3D12PluginEventConfig MakeD3D12EventConfig()
    {
        UnityD3D12PluginEventConfig cfg{};
        cfg.graphicsQueueAccess = kUnityD3D12GraphicsQueueAccess_Allow;
        cfg.ensureActiveRenderTextureIsBound = true;
        cfg.flags = kUnityD3D12EventConfigFlag_ModifiesCommandBuffersState |
                    kUnityD3D12EventConfigFlag_SyncWorkerThreads |
                    kUnityD3D12EventConfigFlag_FlushCommandBuffers;
        return cfg;
    }

    void UNITY_INTERFACE_API OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType)
    {
        if (eventType == kUnityGfxDeviceEventInitialize)
        {
            auto* graphics = gUnityInterfaces->Get<IUnityGraphics>();
            UnityGfxRenderer renderer = graphics->GetRenderer();
            if (renderer == kUnityGfxRendererD3D11)
                gD3D11Graphics = gUnityInterfaces->Get<IUnityGraphicsD3D11>();
            else if (renderer == kUnityGfxRendererD3D12)
            {
                gD3D12Graphics = gUnityInterfaces->Get<IUnityGraphicsD3D12v6>();
                UnityD3D12PluginEventConfig cfg = MakeD3D12EventConfig();
                gD3D12Graphics->ConfigureEvent(gImGuiRenderEventId, &cfg);
            }
        }
        else if (eventType == kUnityGfxDeviceEventShutdown)
        {
            gD3D11Graphics = nullptr;
            gD3D12Graphics = nullptr;
        }
    }
} // namespace

void CreateImGuiRenderer(void* /*platformArg0*/, void* /*platformArg1*/)
{
    if (gActiveRenderer != kUnityGfxRendererNull) DestroyImGuiRenderer();

    HWND hwnd = GetActiveWindow();
    if (!hwnd) hwnd = GetForegroundWindow();
    if (!hwnd) hwnd = FindWindowA("UnityWndClass", nullptr);
    if (!hwnd) hwnd = FindWindowA("Unity", nullptr);
    if (!hwnd) return;

    if (!gOriginalWndProc)
    {
        gHookedHwnd      = hwnd;
        gOriginalWndProc = reinterpret_cast<WNDPROC>(
            SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(CustomWndProc)));
    }

    auto* graphics = gUnityInterfaces->Get<IUnityGraphics>();
    UnityGfxRenderer renderer = graphics->GetRenderer();

    if (renderer == kUnityGfxRendererD3D11 && gD3D11Graphics)
    {
        ID3D11DeviceContext* ctx = nullptr;
        gD3D11Graphics->GetDevice()->GetImmediateContext(&ctx);
        UnityImGui::D3D11_Init(hwnd, gD3D11Graphics->GetDevice(), ctx);
        gActiveRenderer = kUnityGfxRendererD3D11;
    }
    else if (renderer == kUnityGfxRendererD3D12 && gD3D12Graphics)
    {
        if (UnityImGui::D3D12_Init(hwnd, gD3D12Graphics->GetDevice(), 3, DXGI_FORMAT_R8G8B8A8_UNORM,
                                   GetD3D12CommandList, ExecuteD3D12CommandList))
            gActiveRenderer = kUnityGfxRendererD3D12;
    }
}

void DestroyImGuiRenderer()
{
    if (gActiveRenderer == kUnityGfxRendererD3D11) UnityImGui::D3D11_Shutdown();
    else if (gActiveRenderer == kUnityGfxRendererD3D12) UnityImGui::D3D12_Shutdown();
    gActiveRenderer = kUnityGfxRendererNull;

    if (gOriginalWndProc && gHookedHwnd)
    {
        SetWindowLongPtr(gHookedHwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(gOriginalWndProc));
        gOriginalWndProc = nullptr;
        gHookedHwnd      = nullptr;
    }

    if (gD3D12CmdList)   { gD3D12CmdList->Release();   gD3D12CmdList   = nullptr; }
    if (gD3D12Allocator) { gD3D12Allocator->Release(); gD3D12Allocator = nullptr; }
}

void RenderImGui()
{
    if (gActiveRenderer == kUnityGfxRendererD3D11) UnityImGui::D3D11_Render();
    else if (gActiveRenderer == kUnityGfxRendererD3D12) UnityImGui::D3D12_Render();
}

extern "C"
{
    void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces* unityInterfaces)
    {
        gUnityInterfaces = unityInterfaces;
        auto* graphics = unityInterfaces->Get<IUnityGraphics>();
        gImGuiRenderEventId = graphics->ReserveEventIDRange(1);
        graphics->RegisterDeviceEventCallback(OnGraphicsDeviceEvent);
        OnGraphicsDeviceEvent(kUnityGfxDeviceEventInitialize);
    }

    void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginUnload()
    {
        DestroyImGuiRenderer();
        if (gUnityInterfaces)
        {
            auto* graphics = gUnityInterfaces->Get<IUnityGraphics>();
            graphics->UnregisterDeviceEventCallback(OnGraphicsDeviceEvent);
        }
        gUnityInterfaces = nullptr;
    }
}
