#include "MetalBackend.hpp"
#include "PluginInternal.h"
#include <imgui.h>
#include <imgui_impl_metal.h>
#include <time.h>

namespace
{
    bool gInitialized = false;
    UnityImGui::MetalGetFrameDataFunc gGetFrameData = nullptr;
    std::string gImguiConfigPath;
    double gElapsedTime = 0.0;

    inline double NowSeconds()
    {
        return static_cast<double>(clock_gettime_nsec_np(CLOCK_UPTIME_RAW)) / 1e9;
    }
} // namespace

namespace UnityImGui
{
    void Metal_Init(MTL::Device& device, MetalGetFrameDataFunc getFrameData, uint32_t windowWidth,
                    uint32_t windowHeight, float windowScale, std::string imguiConfigPath)
    {
        gGetFrameData = getFrameData;
        gImguiConfigPath = std::move(imguiConfigPath);
        gElapsedTime = 0.0;

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigWindowsMoveFromTitleBarOnly = true;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.DisplaySize = ImVec2(static_cast<float>(windowWidth), static_cast<float>(windowHeight));
        io.IniFilename = gImguiConfigPath.empty() ? nullptr : gImguiConfigPath.c_str();
        io.DisplayFramebufferScale = ImVec2(windowScale, windowScale);
        io.FontGlobalScale = windowScale;
        ImGui::StyleColorsDark();
        ImGui_ImplMetal_Init(&device);
        gInitialized = true;
    }

    void Metal_Shutdown()
    {
        if (!gInitialized) return;
        ImGui_ImplMetal_Shutdown();
        ImGui::DestroyContext();
        gGetFrameData = nullptr;
        gImguiConfigPath.clear();
        gInitialized = false;
    }

    void Metal_Render()
    {
        if (!gInitialized || !gGetFrameData) return;

        MetalFrameData frame = gGetFrameData();
        ImGuiIO& io = ImGui::GetIO();
        if (io.DisplaySize.x <= 0.0f || io.DisplaySize.y <= 0.0f) return;

        ImGui_ImplMetal_NewFrame(frame.renderPassDescriptor);

        double now = NowSeconds();
        io.DeltaTime = (gElapsedTime == 0.0) ? (1.0f / 60.0f)
                                             : static_cast<float>(now - gElapsedTime);
        gElapsedTime = now;

        ImGui::NewFrame();
        if (gImGuiRenderCallback) gImGuiRenderCallback();
        ImGui::Render();

        ImDrawData* drawData = ImGui::GetDrawData();
        if (drawData && drawData->Valid && frame.commandBuffer && frame.renderCommandEncoder)
            ImGui_ImplMetal_RenderDrawData(drawData, frame.commandBuffer, frame.renderCommandEncoder);
    }
} // namespace UnityImGui
