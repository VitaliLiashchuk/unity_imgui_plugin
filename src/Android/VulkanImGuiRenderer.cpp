#include "VulkanImGuiRenderer.hpp"
#include "PluginInternal.h"
#include <android/log.h>
#include <android/native_window.h>
#include <cassert>
#include <imgui.h>
#include <imgui_impl_android.h>
#include <imgui_impl_vulkan.h>
#include <stdexcept>

#define VK_CHECK(func)                                                                             \
    {                                                                                              \
        VkResult r = (func);                                                                       \
        if (r != VK_SUCCESS)                                                                       \
        {                                                                                          \
            __android_log_print(ANDROID_LOG_ERROR, "UnityImGui", "Vulkan error %d at %s:%d",      \
                                static_cast<int>(r), __FILE__, __LINE__);                          \
            assert(false);                                                                         \
        }                                                                                          \
    }

namespace UnityImGui
{
    VulkanImGuiRenderer::VulkanImGuiRenderer(ANativeWindow* nativeWindow, VkInstance instance,
                                             VkPhysicalDevice physicalDevice, VkDevice device,
                                             PFN_vkGetInstanceProcAddr getInstanceProcAddr,
                                             VkQueue queue, uint32_t queueFamilyIndex,
                                             VkFormat format, uint32_t imageCount,
                                             GetCommandBufferFunc getCommandBufferFunc)
        : nativeWindow(nativeWindow), instance(instance), physicalDevice(physicalDevice),
          device(device), getInstanceProcAddr(getInstanceProcAddr), queue(queue),
          queueFamilyIndex(queueFamilyIndex), getCommandBufferFunc(getCommandBufferFunc),
          format(format)
    {
        VmaAllocatorCreateInfo allocInfo{};
        allocInfo.physicalDevice = physicalDevice;
        allocInfo.device = device;
        allocInfo.instance = instance;
        allocInfo.vulkanApiVersion = VK_API_VERSION_1_0;
        VmaVulkanFunctions vkFns{};
        vkFns.vkGetInstanceProcAddr = getInstanceProcAddr;
        vkFns.vkGetDeviceProcAddr = vkGetDeviceProcAddr;
        allocInfo.pVulkanFunctions = &vkFns;
        vmaCreateAllocator(&allocInfo, &vmaAllocator);

        CreateRenderPass(format);
        CreateCommandPool();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigWindowsMoveFromTitleBarOnly = true;
        io.DisplaySize = ImVec2(static_cast<float>(ANativeWindow_getWidth(nativeWindow)),
                                static_cast<float>(ANativeWindow_getHeight(nativeWindow)));
        io.FontGlobalScale = 2.0f;
        ImGui::StyleColorsDark();
        ImGui::GetStyle().ScaleAllSizes(2.0f);

        ImGui_ImplAndroid_Init(nativeWindow);

        ImGui_ImplVulkan_LoadFunctions(
            VK_API_VERSION_1_0,
            [](const char* name, void* userData) -> PFN_vkVoidFunction
            {
                auto* self = static_cast<VulkanImGuiRenderer*>(userData);
                return self->getInstanceProcAddr(self->instance, name);
            },
            this);

        ImGui_ImplVulkan_PipelineInfo pipelineInfo{};
        pipelineInfo.RenderPass = imGuiRenderPass;
        pipelineInfo.Subpass = 0;
        pipelineInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

        ImGui_ImplVulkan_InitInfo initInfo{};
        initInfo.Instance = instance;
        initInfo.PhysicalDevice = physicalDevice;
        initInfo.Device = device;
        initInfo.QueueFamily = queueFamilyIndex;
        initInfo.Queue = queue;
        initInfo.DescriptorPool = imGuiDescriptorPool;
        initInfo.MinImageCount = imageCount;
        initInfo.ImageCount = imageCount;
        initInfo.PipelineInfoMain = pipelineInfo;
        ImGui_ImplVulkan_Init(&initInfo);

        framebufferExtent = {static_cast<uint32_t>(ANativeWindow_getWidth(nativeWindow)),
                             static_cast<uint32_t>(ANativeWindow_getHeight(nativeWindow))};
        CreateFramebufferResources();
        initialized = true;
    }

    VulkanImGuiRenderer::~VulkanImGuiRenderer()
    {
        if (!initialized)
        {
            if (vmaAllocator) { vmaDestroyAllocator(vmaAllocator); vmaAllocator = VK_NULL_HANDLE; }
            return;
        }
        ImGui_ImplVulkan_Shutdown();
        ImGui::DestroyContext();
        if (customFramebuffer) vkDestroyFramebuffer(device, customFramebuffer, nullptr);
        if (framebufferImageView) vkDestroyImageView(device, framebufferImageView, nullptr);
        if (framebufferImage) vmaDestroyImage(vmaAllocator, framebufferImage, frameBufferImageAllocation);
        if (imGuiRenderPass) vkDestroyRenderPass(device, imGuiRenderPass, nullptr);
        if (imGuiDescriptorPool) vkDestroyDescriptorPool(device, imGuiDescriptorPool, nullptr);
        if (imGuiCommandPool) vkDestroyCommandPool(device, imGuiCommandPool, nullptr);
        if (vmaAllocator) vmaDestroyAllocator(vmaAllocator);
    }

    void VulkanImGuiRenderer::CreateRenderPass(VkFormat fmt)
    {
        VkAttachmentDescription attachment{0, fmt, VK_SAMPLE_COUNT_1_BIT,
                                           VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
                                           VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
                                           VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR};
        VkAttachmentReference colRef{0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
        VkSubpassDescription subpass{0, VK_PIPELINE_BIND_POINT_GRAPHICS, 0, nullptr, 1, &colRef,
                                     nullptr, nullptr, 0, nullptr};
        VkRenderPassCreateInfo rpInfo{VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO, nullptr, 0,
                                      1, &attachment, 1, &subpass, 0, nullptr};
        VK_CHECK(vkCreateRenderPass(device, &rpInfo, nullptr, &imGuiRenderPass));

        VkDescriptorPoolSize poolSizes[] = {
            {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
            {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
            {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
            {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};
        VkDescriptorPoolCreateInfo poolInfo{VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO, nullptr,
                                            VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
                                            1000 * IM_ARRAYSIZE(poolSizes),
                                            static_cast<uint32_t>(IM_ARRAYSIZE(poolSizes)), poolSizes};
        if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &imGuiDescriptorPool) != VK_SUCCESS)
            throw std::runtime_error("Failed to create ImGui descriptor pool");
    }

    void VulkanImGuiRenderer::CreateCommandPool()
    {
        VkCommandPoolCreateInfo info{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO, nullptr,
                                     VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, queueFamilyIndex};
        VK_CHECK(vkCreateCommandPool(device, &info, nullptr, &imGuiCommandPool));
    }

    void VulkanImGuiRenderer::CreateFramebufferResources()
    {
        VkImageCreateInfo imgInfo{};
        imgInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imgInfo.imageType = VK_IMAGE_TYPE_2D;
        imgInfo.format = format;
        imgInfo.extent = {framebufferExtent.width, framebufferExtent.height, 1};
        imgInfo.mipLevels = 1;
        imgInfo.arrayLayers = 1;
        imgInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imgInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imgInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
        VK_CHECK(vmaCreateImage(vmaAllocator, &imgInfo, &allocInfo, &framebufferImage,
                                &frameBufferImageAllocation, nullptr));

        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = framebufferImage;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;
        viewInfo.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
        VK_CHECK(vkCreateImageView(device, &viewInfo, nullptr, &framebufferImageView));

        VkFramebufferCreateInfo fbInfo{};
        fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fbInfo.renderPass = imGuiRenderPass;
        fbInfo.attachmentCount = 1;
        fbInfo.pAttachments = &framebufferImageView;
        fbInfo.width = framebufferExtent.width;
        fbInfo.height = framebufferExtent.height;
        fbInfo.layers = 1;
        VK_CHECK(vkCreateFramebuffer(device, &fbInfo, nullptr, &customFramebuffer));
    }

    VkCommandBuffer VulkanImGuiRenderer::BeginSingleTimeCommands(const VkCommandPool& pool) const
    {
        VkCommandBufferAllocateInfo allocInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, nullptr,
                                              pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1};
        VkCommandBuffer cmd;
        vkAllocateCommandBuffers(device, &allocInfo, &cmd);
        VkCommandBufferBeginInfo beginInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, nullptr,
                                           VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr};
        vkBeginCommandBuffer(cmd, &beginInfo);
        return cmd;
    }

    void VulkanImGuiRenderer::EndSingleTimeCommands(VkCommandBuffer cmd, VkCommandPool pool) const
    {
        vkEndCommandBuffer(cmd);
        VkSubmitInfo submitInfo{VK_STRUCTURE_TYPE_SUBMIT_INFO, nullptr, 0, nullptr, nullptr, 1, &cmd};
        VK_CHECK(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE));
        vkQueueWaitIdle(queue);
        vkFreeCommandBuffers(device, pool, 1, &cmd);
    }

    void VulkanImGuiRenderer::BlitToSwapchain(VkCommandBuffer cmd, VkImage dstImage) const
    {
        VkImageMemoryBarrier barrierSrc{};
        barrierSrc.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrierSrc.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        barrierSrc.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrierSrc.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        barrierSrc.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrierSrc.image = framebufferImage;
        barrierSrc.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

        VkImageMemoryBarrier barrierDst = barrierSrc;
        barrierDst.image = dstImage;
        barrierDst.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barrierDst.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrierDst.srcAccessMask = 0;
        barrierDst.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        VkImageMemoryBarrier barriers[] = {barrierSrc, barrierDst};
        vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                             VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 2, barriers);

        VkImageBlit blitRegion{};
        blitRegion.srcSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
        blitRegion.srcOffsets[1] = {static_cast<int32_t>(framebufferExtent.width),
                                    static_cast<int32_t>(framebufferExtent.height), 1};
        blitRegion.dstSubresource = blitRegion.srcSubresource;
        blitRegion.dstOffsets[1] = blitRegion.srcOffsets[1];
        vkCmdBlitImage(cmd, framebufferImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstImage,
                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blitRegion, VK_FILTER_NEAREST);

        barrierDst.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrierDst.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        barrierDst.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrierDst.dstAccessMask = 0;
        vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT,
                             VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrierDst);
    }

    void VulkanImGuiRenderer::Render()
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplAndroid_NewFrame();
        if (ImGui::GetIO().DisplaySize.x > 0.0f && ImGui::GetIO().DisplaySize.y > 0.0f)
        {
            ImGui::NewFrame();
            if (gImGuiRenderCallback) gImGuiRenderCallback();
            ImGui::EndFrame();
            ImGui::Render();
            ImDrawData* drawData = ImGui::GetDrawData();
            if (drawData)
            {
                VkCommandBuffer cmd = getCommandBufferFunc();
                if (cmd != VK_NULL_HANDLE)
                    ImGui_ImplVulkan_RenderDrawData(drawData, cmd);
            }
        }
    }
} // namespace UnityImGui
