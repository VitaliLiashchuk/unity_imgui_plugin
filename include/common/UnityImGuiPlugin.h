#pragma once
#include <stdint.h>
#include <UnityPluginAPI/IUnityInterface.h>
#include <UnityPluginAPI/IUnityRenderingExtensions.h>

typedef void (*UnityImGuiRenderCallback)();

#ifdef __cplusplus
extern "C" {
#endif

/* Returns function pointer for GL.IssuePluginEvent */
UNITY_INTERFACE_EXPORT UnityRenderingEvent UNITY_INTERFACE_API GetRenderEventFunc();

/* Returns the render event ID to pass to GL.IssuePluginEvent */
UNITY_INTERFACE_EXPORT int UNITY_INTERFACE_API UnityImGui_GetEventId();

/* Register C# callback invoked inside an ImGui frame */
UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API
    UnityImGui_SetRenderCallback(UnityImGuiRenderCallback callback);

/* Create imgui renderer — call from C# after Unity is fully initialised.
   On Android, pass the jobject Activity and JavaVM* as the two arguments.
   On all other platforms pass nullptr, nullptr. */
UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API
    UnityImGui_Initialize(void* platformArg0, void* platformArg1);

/* Destroy imgui renderer */
UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API UnityImGui_Shutdown();

#ifdef __cplusplus
}
#endif
