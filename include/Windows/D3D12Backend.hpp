#pragma once
#include <d3d12.h>
#include <dxgi.h>
#include <windows.h>

namespace UnityImGui
{
    using D3D12GetCommandListFunc = ID3D12GraphicsCommandList* (*)();
    using D3D12ExecuteCommandListFunc = void (*)(ID3D12GraphicsCommandList*);

    bool D3D12_Init(HWND hwnd, ID3D12Device* device, int framesInFlight,
                    DXGI_FORMAT renderFormat, D3D12GetCommandListFunc getCommandList,
                    D3D12ExecuteCommandListFunc executeCommandList);
    void D3D12_Shutdown();
    void D3D12_Render();
} // namespace UnityImGui
