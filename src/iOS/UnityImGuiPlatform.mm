/* iOS: creates Metal backend using the UIKit window and Unity Metal interface. */
#include "MetalBackend.hpp"
#include "PluginInternal.h"
#include "TouchInterceptorView.h"
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#include <UnityPluginAPI/IUnityGraphicsMetal.h>

extern IUnityInterfaces* gUnityInterfaces;
extern IUnityGraphicsMetalV1* gMetalGraphics;

extern "C"
{
    void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces* unityInterfaces);
    void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginUnload();
    void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
        UnityRegisterPlugin(void (*load)(IUnityInterfaces*), void (*unload)());
}

namespace
{
    bool gInitialized = false;
    __strong TouchInterceptorView* gTouchView = nil;

    UnityImGui::MetalFrameData GetFrameData()
    {
        UnityImGui::MetalFrameData data{};
        if (!gMetalGraphics) return data;
        data.renderPassDescriptor = (MTL::RenderPassDescriptor*)gMetalGraphics->CurrentRenderPassDescriptor();
        data.commandBuffer = (MTL::CommandBuffer*)gMetalGraphics->CurrentCommandBuffer();
        data.renderCommandEncoder = (MTL::RenderCommandEncoder*)gMetalGraphics->CurrentCommandEncoder();
        return data;
    }
} // namespace

void CreateImGuiRenderer(void* /*platformArg0*/, void* /*platformArg1*/)
{
    // iOS static libs must self-register so Unity calls UnityPluginLoad
    if (!gUnityInterfaces)
        UnityRegisterPlugin(UnityPluginLoad, UnityPluginUnload);

    if (!gMetalGraphics) return;

    UIWindow* window = [UIApplication sharedApplication].keyWindow;
    if (!window) return;

    CGRect frame = window.rootViewController.view.frame;
    gTouchView = [[TouchInterceptorView alloc] initWithFrame:frame];
    gTouchView.backgroundColor = [UIColor clearColor];
    gTouchView.userInteractionEnabled = YES;
    [window addSubview:gTouchView];
    [window bringSubviewToFront:gTouchView];

    CGSize logicalSize = gTouchView.bounds.size;
    CGFloat scale = window.screen.scale;
    if (scale <= 0.1f) scale = 1.0f;

    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* iniPath = [[paths objectAtIndex:0] stringByAppendingPathComponent:@"imgui.ini"];
    std::string imguiConfigPath([iniPath UTF8String]);

    UnityImGui::Metal_Init(*(MTL::Device*)gMetalGraphics->MetalDevice(), &GetFrameData,
                           static_cast<uint32_t>(logicalSize.width  * scale),
                           static_cast<uint32_t>(logicalSize.height * scale),
                           static_cast<float>(scale), imguiConfigPath);
    gInitialized = true;
}

void DestroyImGuiRenderer()
{
    if (gInitialized)
    {
        UnityImGui::Metal_Shutdown();
        gInitialized = false;
    }
    if (gTouchView)
    {
        [gTouchView removeFromSuperview];
        gTouchView = nil;
    }
}

void RenderImGui()
{
    if (gInitialized) UnityImGui::Metal_Render();
}
