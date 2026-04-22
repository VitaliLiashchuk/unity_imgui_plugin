#pragma once

#include <android/native_window.h>
#include <volk.h>
#include <vk_mem_alloc.h>

namespace UnityImGui
{
    using GetCommandBufferFunc = VkCommandBuffer (*)();

    class VulkanImGuiRenderer
    {
    public:
        VulkanImGuiRenderer(ANativeWindow* nativeWindow, VkInstance instance,
                            VkPhysicalDevice physicalDevice, VkDevice device,
                            PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkQueue queue,
                            uint32_t queueFamilyIndex, VkFormat format, uint32_t imageCount,
                            GetCommandBufferFunc getCommandBufferFunc);
        ~VulkanImGuiRenderer();
        void Render();
        bool IsInitialized() const { return initialized; }
        VkExtent2D GetFramebufferExtent() const { return framebufferExtent; }
        VkRenderPass GetRenderPass() const { return imGuiRenderPass; }
        VkFramebuffer GetFramebuffer() const { return customFramebuffer; }
        void BlitToSwapchain(VkCommandBuffer cmd, VkImage dstImage) const;

    private:
        void CreateRenderPass(VkFormat format);
        void CreateCommandPool();
        void CreateFramebufferResources();
        VkCommandBuffer BeginSingleTimeCommands(const VkCommandPool& pool) const;
        void EndSingleTimeCommands(VkCommandBuffer cmd, VkCommandPool pool) const;

        ANativeWindow* nativeWindow{};
        VkInstance instance{};
        VkPhysicalDevice physicalDevice{};
        VkDevice device{};
        PFN_vkGetInstanceProcAddr getInstanceProcAddr{};
        VkQueue queue{};
        uint32_t queueFamilyIndex{};
        GetCommandBufferFunc getCommandBufferFunc;
        VmaAllocator vmaAllocator{};
        VkFormat format{};
        bool initialized{};
        VkRenderPass imGuiRenderPass{};
        VkDescriptorPool imGuiDescriptorPool{};
        VkCommandPool imGuiCommandPool{};
        VkImage framebufferImage{};
        VmaAllocation frameBufferImageAllocation{};
        VkImageView framebufferImageView{};
        VkFramebuffer customFramebuffer{};
        VkExtent2D framebufferExtent{};
    };
} // namespace UnityImGui
