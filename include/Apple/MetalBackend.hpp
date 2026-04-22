#pragma once

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnested-anon-types"
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#pragma clang diagnostic ignored "-Wunused-parameter"
#include <MetalKit/MetalKit.hpp>
#pragma clang diagnostic pop
#include <cstdint>
#include <string>

namespace UnityImGui
{
    struct MetalFrameData
    {
        MTL::RenderPassDescriptor* renderPassDescriptor{};
        MTL::CommandBuffer* commandBuffer{};
        MTL::RenderCommandEncoder* renderCommandEncoder{};
    };

    using MetalGetFrameDataFunc = MetalFrameData (*)();

    void Metal_Init(MTL::Device& device, MetalGetFrameDataFunc getFrameData, uint32_t windowWidth,
                    uint32_t windowHeight, float windowScale, std::string imguiConfigPath);
    void Metal_Shutdown();
    void Metal_Render();
} // namespace UnityImGui
