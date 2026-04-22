#include "PluginInternal.h"
#include "UnityImGuiPlugin.h"
#include <UnityPluginAPI/IUnityRenderingExtensions.h>

UnityImGuiRenderCallback gImGuiRenderCallback = nullptr;
int gImGuiRenderEventId = -1;

static void UNITY_INTERFACE_API OnRenderEvent(int eventId)
{
    if (eventId == gImGuiRenderEventId)
        RenderImGui();
}

extern "C"
{
    UNITY_INTERFACE_EXPORT UnityRenderingEvent UNITY_INTERFACE_API GetRenderEventFunc()
    {
        return OnRenderEvent;
    }

    UNITY_INTERFACE_EXPORT int UNITY_INTERFACE_API UnityImGui_GetEventId()
    {
        return gImGuiRenderEventId;
    }

    UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API
    UnityImGui_SetRenderCallback(UnityImGuiRenderCallback callback)
    {
        gImGuiRenderCallback = callback;
    }

    UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API
    UnityImGui_Initialize(void* platformArg0, void* platformArg1)
    {
        CreateImGuiRenderer(platformArg0, platformArg1);
    }

    UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API UnityImGui_Shutdown()
    {
        DestroyImGuiRenderer();
    }
}
