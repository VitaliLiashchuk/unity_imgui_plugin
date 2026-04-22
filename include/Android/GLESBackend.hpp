#pragma once

#include <android/native_window.h>
#include <string>

namespace UnityImGui
{
    void GLES_Init(ANativeWindow* nativeWindow, std::string imguiConfigPath);
    void GLES_Shutdown();
    void GLES_Render();
} // namespace UnityImGui
