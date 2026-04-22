/* macOS: creates Metal backend using the NSView with CAMetalLayer. */
#include "MetalBackend.hpp"
#include "PluginInternal.h"
#include <imgui.h>
#import <AppKit/AppKit.h>
#import <QuartzCore/CAMetalLayer.h>
#include <UnityPluginAPI/IUnityGraphicsMetal.h>

extern IUnityInterfaces* gUnityInterfaces;
extern IUnityGraphicsMetalV1* gMetalGraphics;

namespace
{
    bool gInitialized = false;
    NSView* gUnityMetalView = nil;

    NSView* FindDeepMetalView(NSView* root)
    {
        NSRect bounds = root.bounds;
        if ([root.layer isKindOfClass:[CAMetalLayer class]] && root.window.screen != nil &&
            bounds.size.width > 500 && bounds.size.height > 300)
        {
            return root;
        }
        for (NSView* sub in root.subviews)
        {
            NSView* found = FindDeepMetalView(sub);
            if (found) return found;
        }
        return nil;
    }

    void UpdateDisplaySizeFromTexture(NSView* metalView, NSUInteger w, NSUInteger h)
    {
        if (!metalView || !metalView.window) return;
        CGSize bounds = metalView.bounds.size;
        float scale = 1.0f;
        float margin = 100.0f;
        if (std::abs(static_cast<float>(h) - bounds.height * 2.0f) < margin) scale = 2.0f;
        else if (std::abs(static_cast<float>(h) - bounds.height * 1.0f) < margin) scale = 1.0f;

        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2(static_cast<float>(w), static_cast<float>(h));
        io.DisplayFramebufferScale = ImVec2(scale, scale);
    }

    UnityImGui::MetalFrameData GetFrameData()
    {
        UnityImGui::MetalFrameData data{};
        if (!gMetalGraphics) return data;
        data.renderPassDescriptor = (MTL::RenderPassDescriptor*)gMetalGraphics->CurrentRenderPassDescriptor();
        data.commandBuffer = (MTL::CommandBuffer*)gMetalGraphics->CurrentCommandBuffer();
        data.renderCommandEncoder = (MTL::RenderCommandEncoder*)gMetalGraphics->CurrentCommandEncoder();

        if (data.renderPassDescriptor)
        {
            auto* att = data.renderPassDescriptor->colorAttachments()->object(0);
            if (att)
            {
                auto* tex = att->texture();
                if (tex && gUnityMetalView)
                    UpdateDisplaySizeFromTexture(gUnityMetalView, tex->width(), tex->height());
            }
        }
        return data;
    }
} // namespace

void CreateImGuiRenderer(void* /*platformArg0*/, void* /*platformArg1*/)
{
    if (gInitialized) DestroyImGuiRenderer();
    if (!gMetalGraphics) return;

    for (NSWindow* win in [NSApp windows])
    {
        gUnityMetalView = FindDeepMetalView(win.contentView);
        if (gUnityMetalView) { [gUnityMetalView retain]; break; }
    }
    if (!gUnityMetalView) return;

    NSWindow* win = gUnityMetalView.window;
    if (!win) return;

    CGSize logicalSize = gUnityMetalView.bounds.size;
    CGFloat scale = win.screen.backingScaleFactor;
    if (scale <= 0.1f || isnan(scale)) scale = 1.0f;

    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
    if (paths.count == 0) return;
    NSString* iniPath = [[paths objectAtIndex:0] stringByAppendingPathComponent:@"imgui.ini"];
    std::string imguiConfigPath([iniPath UTF8String]);

    UnityImGui::Metal_Init(*(MTL::Device*)gMetalGraphics->MetalDevice(), &GetFrameData,
                           static_cast<uint32_t>(logicalSize.width),
                           static_cast<uint32_t>(logicalSize.height),
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
    if (gUnityMetalView)
    {
        [gUnityMetalView release];
        gUnityMetalView = nil;
    }
}

void RenderImGui()
{
    if (!ImGui::GetCurrentContext()) return;

    if (gUnityMetalView && gUnityMetalView.window)
    {
        NSPoint globalMouse = [gUnityMetalView.window mouseLocationOutsideOfEventStream];
        NSPoint localMouse = [gUnityMetalView convertPoint:globalMouse fromView:nil];
        localMouse.y = gUnityMetalView.bounds.size.height - localMouse.y - 46; // Unity Editor toolbar height
        ImGuiIO& io = ImGui::GetIO();
        io.MousePos = ImVec2(localMouse.x, localMouse.y);
        io.MouseDown[0] = ([NSEvent pressedMouseButtons] & 1) != 0;
    }

    if (gInitialized) UnityImGui::Metal_Render();
}
