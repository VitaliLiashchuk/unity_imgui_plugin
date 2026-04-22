/* Common Apple (iOS + macOS) plugin load/unload and Metal interface access. */
#import <Foundation/Foundation.h>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnested-anon-types"
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#include <Metal/Metal.hpp>
#pragma clang diagnostic pop
#include <UnityPluginAPI/IUnityGraphics.h>
#include <UnityPluginAPI/IUnityGraphicsMetal.h>
#include <UnityPluginAPI/IUnityInterface.h>
#include "PluginInternal.h"

IUnityInterfaces* gUnityInterfaces = nullptr;
IUnityGraphicsMetalV1* gMetalGraphics = nullptr;

static void UNITY_INTERFACE_API OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType)
{
    if (eventType == kUnityGfxDeviceEventInitialize)
    {
        auto* graphics = gUnityInterfaces->Get<IUnityGraphics>();
        if (graphics->GetRenderer() == kUnityGfxRendererMetal)
            gMetalGraphics = gUnityInterfaces->Get<IUnityGraphicsMetalV1>();
    }
    else if (eventType == kUnityGfxDeviceEventShutdown)
    {
        DestroyImGuiRenderer();
        gMetalGraphics = nullptr;
    }
}

extern "C"
{
    void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces* unityInterfaces)
    {
        gUnityInterfaces = unityInterfaces;
        auto* graphics = unityInterfaces->Get<IUnityGraphics>();
        gImGuiRenderEventId = graphics->ReserveEventIDRange(1);
        graphics->RegisterDeviceEventCallback(OnGraphicsDeviceEvent);
        gMetalGraphics = unityInterfaces->Get<IUnityGraphicsMetalV1>();
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
        gMetalGraphics = nullptr;
    }
}
