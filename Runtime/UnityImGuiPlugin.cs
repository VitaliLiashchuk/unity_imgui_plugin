using System;
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

    void OnEnable()  => Camera.onPostRender += OnCameraPostRender;
    void OnDisable() => Camera.onPostRender -= OnCameraPostRender;

    void OnCameraPostRender(Camera cam)
    {
        if (cam.CompareTag("MainCamera"))
            GL.IssuePluginEvent(GetRenderEventFunc(), UnityImGui_GetEventId());
    }

    [MonoPInvokeCallback(typeof(Action))]
    static void OnRender()
    {
        OnImGuiDraw?.Invoke();
    }
}
