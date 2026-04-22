#include "GLESBackend.hpp"
#include "PluginInternal.h"
#include <imgui.h>
#include <imgui_impl_android.h>
#include <imgui_impl_opengl3.h>

namespace
{
    ANativeWindow* gNativeWindow = nullptr;
    std::string gImguiConfigPath;
    bool gImGuiBackendInitialized = false;
    bool gStored = false;

    void EnsureImGuiBackendInitialized()
    {
        if (gImGuiBackendInitialized) return;

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigWindowsMoveFromTitleBarOnly = true;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.DisplaySize = ImVec2(static_cast<float>(ANativeWindow_getWidth(gNativeWindow)),
                                static_cast<float>(ANativeWindow_getHeight(gNativeWindow)));
        io.IniFilename = nullptr;
        ImGui::StyleColorsDark();
        ImGui_ImplAndroid_Init(gNativeWindow);
        ImGui_ImplOpenGL3_Init("#version 300 es");
        ImGui::GetStyle().ScaleAllSizes(3.0f);
        gImGuiBackendInitialized = true;
    }
} // namespace

namespace UnityImGui
{
    void GLES_Init(ANativeWindow* nativeWindow, std::string imguiConfigPath)
    {
        gNativeWindow = nativeWindow;
        gImguiConfigPath = std::move(imguiConfigPath);
        gStored = true;
        // Actual imgui init deferred to Render — needs current GL context
    }

    void GLES_Shutdown()
    {
        if (gImGuiBackendInitialized)
        {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplAndroid_Shutdown();
            ImGui::DestroyContext();
            gImGuiBackendInitialized = false;
        }
        gNativeWindow = nullptr;
        gImguiConfigPath.clear();
        gStored = false;
    }

    void GLES_Render()
    {
        if (!gStored) return;
        EnsureImGuiBackendInitialized();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplAndroid_NewFrame();
        ImGuiIO& io = ImGui::GetIO();
        if (io.DisplaySize.x <= 0.0f || io.DisplaySize.y <= 0.0f) return;
        ImGui::NewFrame();
        if (gImGuiRenderCallback) gImGuiRenderCallback();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
} // namespace UnityImGui
