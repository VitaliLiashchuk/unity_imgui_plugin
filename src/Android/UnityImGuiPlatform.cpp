/* Android: UnityPluginLoad/Unload + renderer creation via JNI surface access. */
// clang-format off
#include <GLES3/gl31.h>
#include <GLES2/gl2ext.h>
// clang-format on
#include "GLESBackend.hpp"
#include "VulkanImGuiRenderer.hpp"
#include "PluginInternal.h"
#include <UnityPluginAPI/IUnityGraphics.h>
#include <UnityPluginAPI/IUnityGraphicsVulkan.h>
#include <UnityPluginAPI/IUnityInterface.h>
#include <android/log.h>
#include <android/native_window_jni.h>
#include <imgui.h>
#include <jni.h>
#include <memory>

#define ALOG_E(...) __android_log_print(ANDROID_LOG_ERROR, "UnityImGui", __VA_ARGS__)
#define ALOG_D(...) __android_log_print(ANDROID_LOG_DEBUG, "UnityImGui", __VA_ARGS__)

static IUnityInterfaces* gUnityInterfaces = nullptr;
static IUnityGraphicsVulkan* gVulkanGraphics = nullptr;
static ANativeWindow* gNativeWindow = nullptr;

static std::unique_ptr<UnityImGui::VulkanImGuiRenderer> gVulkanRenderer;
static bool gGLESActive = false;

// Touch input forwarded from Java
extern "C" JNIEXPORT void JNICALL
Java_com_unityimgui_TouchHelper_onTouchEvent(JNIEnv*, jclass, jfloat x, jfloat y, jint action)
{
    ImGuiIO& io = ImGui::GetIO();
    io.AddMousePosEvent(x, y);
    io.AddMouseButtonEvent(0, (action == 0 || action == 2));
}

static UnityVulkanPluginEventConfig MakeVulkanEventConfig(bool insideRenderPass)
{
    UnityVulkanPluginEventConfig cfg{};
    cfg.graphicsQueueAccess = insideRenderPass ? kUnityVulkanGraphicsQueueAccess_DontCare
                                               : kUnityVulkanGraphicsQueueAccess_Allow;
    cfg.renderPassPrecondition = insideRenderPass ? kUnityVulkanRenderPass_EnsureInside
                                                  : kUnityVulkanRenderPass_EnsureOutside;
    cfg.flags = kUnityVulkanEventConfigFlag_EnsurePreviousFrameSubmission |
                kUnityVulkanEventConfigFlag_FlushCommandBuffers;
    return cfg;
}

static void UNITY_INTERFACE_API OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType)
{
    if (eventType == kUnityGfxDeviceEventInitialize)
    {
        auto* graphics = gUnityInterfaces->Get<IUnityGraphics>();
        if (graphics->GetRenderer() == kUnityGfxRendererVulkan)
        {
            gVulkanGraphics = gUnityInterfaces->Get<IUnityGraphicsVulkan>();
            UnityVulkanPluginEventConfig cfg = MakeVulkanEventConfig(true);
            gVulkanGraphics->ConfigureEvent(gImGuiRenderEventId, &cfg);
        }
    }
    else if (eventType == kUnityGfxDeviceEventShutdown)
    {
        gVulkanGraphics = nullptr;
    }
}

void CreateImGuiRenderer(void* activity, void* javaVm)
{
    if (!gUnityInterfaces || !activity || !javaVm)
    {
        ALOG_E("CreateImGuiRenderer: missing interfaces or activity");
        return;
    }

    JavaVM* vm = static_cast<JavaVM*>(javaVm);
    JNIEnv* env = nullptr;
    vm->AttachCurrentThread(&env, nullptr);
    if (!env) { ALOG_E("Failed to attach JNI thread"); return; }

    jobject activityObj = static_cast<jobject>(activity);
    jclass activityClass = env->GetObjectClass(activityObj);

    // Resolve imgui ini path from files dir
    jmethodID getFilesDirMethod = env->GetMethodID(activityClass, "getFilesDir", "()Ljava/io/File;");
    jobject fileObj = env->CallObjectMethod(activityObj, getFilesDirMethod);
    jclass fileClass = env->GetObjectClass(fileObj);
    jmethodID getAbsPathMethod = env->GetMethodID(fileClass, "getAbsolutePath", "()Ljava/lang/String;");
    auto pathStr = reinterpret_cast<jstring>(env->CallObjectMethod(fileObj, getAbsPathMethod));
    const char* pathCStr = env->GetStringUTFChars(pathStr, nullptr);
    std::string imguiIniPath = std::string(pathCStr) + "/imgui.ini";
    env->ReleaseStringUTFChars(pathStr, pathCStr);

    // Get surface from UnityPlayer's SurfaceView (Unity 2023+)
    jmethodID getClassLoaderMethod =
        env->GetMethodID(activityClass, "getClassLoader", "()Ljava/lang/ClassLoader;");
    jobject classLoader = env->CallObjectMethod(activityObj, getClassLoaderMethod);
    jclass classLoaderClass = env->GetObjectClass(classLoader);
    jmethodID loadClassMethod =
        env->GetMethodID(classLoaderClass, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");

    // Try Unity 2023+ getSurfaceView
    jfieldID mUnityPlayerField = nullptr;
    {
        jfieldID f = env->GetFieldID(activityClass, "mUnityPlayer",
                                     "Lcom/unity3d/player/UnityPlayerForActivityOrService;");
        if (env->ExceptionCheck()) env->ExceptionClear();
        if (f) mUnityPlayerField = f;
    }
    if (!mUnityPlayerField)
    {
        jfieldID f = env->GetFieldID(activityClass, "mUnityPlayer",
                                     "Lcom/unity3d/player/UnityPlayer;");
        if (env->ExceptionCheck()) env->ExceptionClear();
        if (f) mUnityPlayerField = f;
    }

    if (!mUnityPlayerField) { ALOG_E("Could not find mUnityPlayer field"); return; }

    jobject unityPlayer = env->GetObjectField(activityObj, mUnityPlayerField);
    jclass unityPlayerClass = env->GetObjectClass(unityPlayer);
    jmethodID getSurfaceView =
        env->GetMethodID(unityPlayerClass, "getSurfaceView", "()Landroid/view/SurfaceView;");
    if (env->ExceptionCheck()) { env->ExceptionClear(); getSurfaceView = nullptr; }

    jobject surfaceView = getSurfaceView
        ? env->CallObjectMethod(unityPlayer, getSurfaceView)
        : nullptr;

    if (!surfaceView) { ALOG_E("Could not get SurfaceView"); return; }

    jclass svClass = env->GetObjectClass(surfaceView);
    jmethodID getHolder = env->GetMethodID(svClass, "getHolder", "()Landroid/view/SurfaceHolder;");
    jobject holder = env->CallObjectMethod(surfaceView, getHolder);
    jclass holderClass = env->GetObjectClass(holder);
    jmethodID getSurface = env->GetMethodID(holderClass, "getSurface", "()Landroid/view/Surface;");
    jobject surface = env->CallObjectMethod(holder, getSurface);

    gNativeWindow = ANativeWindow_fromSurface(env, surface);
    if (!gNativeWindow) { ALOG_E("ANativeWindow_fromSurface failed"); return; }

    if (!gVulkanGraphics)
    {
        UnityImGui::GLES_Init(gNativeWindow, imguiIniPath);
        gGLESActive = true;
    }
    else
    {
        const UnityVulkanInstance inst = gVulkanGraphics->Instance();
        gVulkanRenderer = std::make_unique<UnityImGui::VulkanImGuiRenderer>(
            gNativeWindow, inst.instance, inst.physicalDevice, inst.device,
            inst.getInstanceProcAddr, inst.graphicsQueue, inst.queueFamilyIndex,
            VK_FORMAT_R8G8B8A8_UNORM, 3,
            []() -> VkCommandBuffer
            {
                gVulkanGraphics->EnsureInsideRenderPass();
                UnityVulkanRecordingState state{};
                if (!gVulkanGraphics->CommandRecordingState(&state, kUnityVulkanGraphicsQueueAccess_DontCare))
                    return VK_NULL_HANDLE;
                return state.commandBuffer;
            });
    }
}

void DestroyImGuiRenderer()
{
    if (gGLESActive) { UnityImGui::GLES_Shutdown(); gGLESActive = false; }
    gVulkanRenderer.reset();
    if (gNativeWindow)
    {
        ANativeWindow_release(gNativeWindow);
        gNativeWindow = nullptr;
    }
}

void RenderImGui()
{
    if (gVulkanRenderer) gVulkanRenderer->Render();
    if (gGLESActive) UnityImGui::GLES_Render();
}

extern "C"
{
    void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces* unityInterfaces)
    {
        gUnityInterfaces = unityInterfaces;
        auto* graphics = unityInterfaces->Get<IUnityGraphics>();
        gImGuiRenderEventId = graphics->ReserveEventIDRange(1);
        graphics->RegisterDeviceEventCallback(OnGraphicsDeviceEvent);
        OnGraphicsDeviceEvent(kUnityGfxDeviceEventInitialize);
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
        gVulkanGraphics = nullptr;
    }
}
