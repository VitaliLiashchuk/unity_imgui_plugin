#pragma once
#include <d3d11.h>
#include <windows.h>

namespace UnityImGui
{
    void D3D11_Init(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* context);
    void D3D11_Shutdown();
    void D3D11_Render();
} // namespace UnityImGui
