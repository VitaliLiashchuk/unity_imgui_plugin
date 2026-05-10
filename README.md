# UnityImGuiPlugin

Unity native plugin that renders [Dear ImGui](https://github.com/ocornut/imgui) using Unity's own graphics device.  
Supports **Metal** (iOS / macOS), **OpenGL ES 3 / Vulkan** (Android), **D3D11 / D3D12** (Windows).

No third-party SDK required — only Unity Plugin API + imgui.

---

## Install (UPM)

In Unity: **Window → Package Manager → + → Install package from git URL…**

```
https://github.com/VitaliLiashchuk/unity_imgui_plugin.git
```

Or add to `Packages/manifest.json`:

```json
"com.liashchuk.unityimgui": "https://github.com/VitaliLiashchuk/unity_imgui_plugin.git"
```

Pin a version with `#<tag>` (e.g. `#v0.1.0`).

After install, attach `UnityImGuiPlugin` component to a GameObject and subscribe to `UnityImGuiPlugin.OnImGuiDraw` (see [Unity C# integration](#unity-c-integration)).

---

## Repository layout

```
cmake/                     CMake helpers (imgui target, android AAR packer)
include/                   Public headers per platform
src/                       Sources per platform
package/                   UPM package (Runtime C# + built Plugins/)
thirdparty/
  imgui/                   submodule — ocornut/imgui
  volk/                    submodule — zeux/volk
  vma/                     submodule — GPUOpen/VulkanMemoryAllocator
  metal_cpp/               Apple metal-cpp
  UnityPluginAPI/          Unity low-level plugin headers
```

---

## Prerequisites

| Platform | Requirement |
|----------|-------------|
| iOS / macOS | Xcode 15+, CMake 3.20+ |
| Android | Android NDK r27+, CMake 3.20+ |
| Windows | Visual Studio 2022, CMake 3.20+ |

---

## Clone

```bash
git clone --recurse-submodules https://github.com/VitaliLiashchuk/unity_imgui_plugin.git
cd unity_imgui_plugin
```

If already cloned without submodules:
```bash
git submodule update --init --recursive
```

---

## Build

### macOS (Metal)

```bash
cmake -S . -B build/macos \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"
cmake --build build/macos --config Release
cmake --install build/macos --config Release
```

Output: `Plugins/Darwin/UnityImGuiPlugin.dylib`

---

### iOS (Metal)

```bash
cmake -S . -B build/ios \
  -DCMAKE_SYSTEM_NAME=iOS \
  -DCMAKE_OSX_ARCHITECTURES=arm64 \
  -DCMAKE_OSX_DEPLOYMENT_TARGET=14.0 \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build/ios --config Release
cmake --install build/ios --config Release
```

Output: `Plugins/iOS/UnityImGuiPlugin.a`

---

### Android (OpenGL ES 3 / Vulkan)

Build both ABIs then pack into an AAR:

```bash
NDK=$ANDROID_SDK_ROOT/ndk/27.2.12479018

# arm64-v8a (device)
cmake -S . -B build/android-arm64 \
  -DCMAKE_TOOLCHAIN_FILE=$NDK/build/cmake/android.toolchain.cmake \
  -DANDROID_ABI=arm64-v8a \
  -DANDROID_PLATFORM=android-26 \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build/android-arm64 --config Release
cmake --install build/android-arm64 --config Release

# x86_64 (emulator)
cmake -S . -B build/android-x86_64 \
  -DCMAKE_TOOLCHAIN_FILE=$NDK/build/cmake/android.toolchain.cmake \
  -DANDROID_ABI=x86_64 \
  -DANDROID_PLATFORM=android-26 \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build/android-x86_64 --config Release
cmake --install build/android-x86_64 --config Release

# Pack AAR
cmake -P cmake/android_aar.cmake
```

Output: `Plugins/Android/UnityImGuiPlugin.aar`

---

### Windows (D3D11 / D3D12)

```bat
cmake -S . -B build\windows -A x64
cmake --build build\windows --config Release
cmake --install build\windows --config Release
```

Output: `Plugins\Windows\x86_64\UnityImGuiPlugin.dll`

---

## Unity C# integration

Add `UnityImGuiPlugin.cs` (from `package/Runtime/`) to your scene's GameObject.

```csharp
using System;
using System.Collections;
using System.Runtime.InteropServices;
using AOT;
using UnityEngine;

public class UnityImGuiPlugin : MonoBehaviour
{
#if UNITY_IOS && !UNITY_EDITOR
    const string PluginName = "__Internal";
#else
    const string PluginName = "UnityImGuiPlugin";
#endif

    [DllImport(PluginName)] static extern IntPtr GetRenderEventFunc();
    [DllImport(PluginName)] static extern int    UnityImGui_GetEventId();
    [DllImport(PluginName)] static extern void   UnityImGui_SetRenderCallback(Action callback);
    [DllImport(PluginName)] static extern void   UnityImGui_Initialize(IntPtr arg0, IntPtr arg1);
    [DllImport(PluginName)] static extern void   UnityImGui_Shutdown();

    public static event Action OnImGuiDraw;

    // Rooted to prevent GC collection on IL2CPP
    static readonly Action _onRenderDelegate = OnRender;

    void Start()
    {
        UnityImGui_SetRenderCallback(_onRenderDelegate);

#if UNITY_ANDROID && !UNITY_EDITOR
        using var player = new AndroidJavaClass("com.unity3d.player.UnityPlayer");
        using var activity = player.GetStatic<AndroidJavaObject>("currentActivity");
        UnityImGui_Initialize(activity.GetRawObject(), IntPtr.Zero);
#else
        UnityImGui_Initialize(IntPtr.Zero, IntPtr.Zero);
#endif
    }

    void OnDestroy() => UnityImGui_Shutdown();

    void OnEnable()  => StartCoroutine(RenderLoop());
    void OnDisable() => StopAllCoroutines();

    IEnumerator RenderLoop()
    {
        var waitForEndOfFrame = new WaitForEndOfFrame();
        while (true)
        {
            yield return waitForEndOfFrame;
            GL.IssuePluginEvent(GetRenderEventFunc(), UnityImGui_GetEventId());
        }
    }

    [MonoPInvokeCallback(typeof(Action))]
    static void OnRender()
    {
        OnImGuiDraw?.Invoke();
    }
}
```

Subscribe to `UnityImGuiPlugin.OnImGuiDraw` from any script to draw ImGui windows:

```csharp
void OnEnable()  => UnityImGuiPlugin.OnImGuiDraw += DrawGUI;
void OnDisable() => UnityImGuiPlugin.OnImGuiDraw -= DrawGUI;

void DrawGUI()
{
    ImGui.Begin("My Window");
    ImGui.Text("Hello from ImGui!");
    ImGui.End();
}
```

---

## CMake options

| Variable | Default | Description |
|----------|---------|-------------|
| `CMAKE_BUILD_TYPE` | — | `Debug` / `Release` / `RelWithDebInfo` |
| `CMAKE_OSX_ARCHITECTURES` | host arch | `arm64;x86_64` for universal macOS binary |
| `ANDROID_ABI` | — | `arm64-v8a`, `x86_64` |
| `ANDROID_PLATFORM` | — | Min API level, e.g. `android-26` |

---

## License

MIT — see [LICENSE](LICENSE).

Third-party libraries retain their own licenses: see `thirdparty/*/LICENSE*`.
