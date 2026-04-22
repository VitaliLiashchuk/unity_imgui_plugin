#pragma once

#include "UnityImGuiPlugin.h"

extern UnityImGuiRenderCallback gImGuiRenderCallback;
extern int gImGuiRenderEventId;

extern void CreateImGuiRenderer(void* platformArg0, void* platformArg1);
extern void DestroyImGuiRenderer();
extern void RenderImGui();
