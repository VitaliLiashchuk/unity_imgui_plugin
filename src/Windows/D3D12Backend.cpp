#include "D3D12Backend.hpp"
#include "PluginInternal.h"
#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>

namespace
{
    ID3D12DescriptorHeap* gDescriptorHeap = nullptr;
    UnityImGui::D3D12GetCommandListFunc gGetCommandList = nullptr;
    UnityImGui::D3D12ExecuteCommandListFunc gExecuteCommandList = nullptr;
    bool gInitialized = false;
} // namespace

namespace UnityImGui
{
    bool D3D12_Init(HWND hwnd, ID3D12Device* device, int framesInFlight,
                    DXGI_FORMAT renderFormat, D3D12GetCommandListFunc getCommandList,
                    D3D12ExecuteCommandListFunc executeCommandList)
    {
        gGetCommandList = getCommandList;
        gExecuteCommandList = executeCommandList;

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigWindowsMoveFromTitleBarOnly = true;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        ImGui::StyleColorsDark();
        io.Fonts->Build();

        D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        heapDesc.NumDescriptors = 64;
        if (FAILED(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&gDescriptorHeap))))
            return false;

        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplDX12_Init(device, framesInFlight, renderFormat, gDescriptorHeap,
                            gDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
                            gDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
        gInitialized = true;
        return true;
    }

    void D3D12_Shutdown()
    {
        if (!gInitialized) return;
        gDescriptorHeap->Release();
        gDescriptorHeap = nullptr;
        ImGui_ImplDX12_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        gGetCommandList = nullptr;
        gExecuteCommandList = nullptr;
        gInitialized = false;
    }

    void D3D12_Render()
    {
        if (!gInitialized) return;
        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGuiIO& io = ImGui::GetIO();
        if (io.DisplaySize.x <= 0.0f || io.DisplaySize.y <= 0.0f) return;
        ImGui::NewFrame();
        if (gImGuiRenderCallback) gImGuiRenderCallback();
        ImGui::Render();
        if (auto* commandList = gGetCommandList ? gGetCommandList() : nullptr)
        {
            commandList->SetDescriptorHeaps(1, &gDescriptorHeap);
            ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
            if (gExecuteCommandList) gExecuteCommandList(commandList);
        }
    }
} // namespace UnityImGui
