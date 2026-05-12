#include "Engine/RHI/Vulkan/VulkanTriangleRenderer.hpp"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define VK_USE_PLATFORM_WIN32_KHR
#include <Windows.h>
#include <vulkan/vulkan.h>

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <limits>
#include <optional>
#include <string>
#include <vector>
#include <iterator>

namespace HFEngine::RHI::Vulkan
{
    namespace
    {
        struct QueueFamilySelection
        {
            std::uint32_t graphicsFamily = 0;
            bool hasGraphicsFamily = false;
        };

        struct SwapchainSupport
        {
            VkSurfaceCapabilitiesKHR capabilities{};
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> presentModes;
        };

        std::string VkMessage(VkResult result, const char* operation)
        {
            return std::string(operation) + " failed with VkResult " + std::to_string(static_cast<int>(result));
        }

        bool VkSucceeded(VkResult result, std::string& message, const char* operation)
        {
            if (result == VK_SUCCESS)
            {
                return true;
            }

            message = VkMessage(result, operation);
            return false;
        }

        std::string ToUtf8(const wchar_t* text)
        {
            if (text == nullptr || text[0] == L'\0')
            {
                return {};
            }

            const int required = WideCharToMultiByte(CP_UTF8, 0, text, -1, nullptr, 0, nullptr, nullptr);
            if (required <= 1)
            {
                return {};
            }

            std::string output(static_cast<std::size_t>(required - 1), '\0');
            WideCharToMultiByte(CP_UTF8, 0, text, -1, output.data(), required, nullptr, nullptr);
            return output;
        }

        std::string ExecutableDirectory()
        {
            wchar_t path[MAX_PATH]{};
            const DWORD size = GetModuleFileNameW(nullptr, path, MAX_PATH);
            if (size == 0 || size == MAX_PATH)
            {
                return ".";
            }

            std::wstring widePath(path, size);
            const std::size_t slash = widePath.find_last_of(L"\\/");
            if (slash != std::wstring::npos)
            {
                widePath.resize(slash);
            }

            return ToUtf8(widePath.c_str());
        }

        std::vector<std::uint32_t> ReadSpirv(const std::string& path)
        {
            std::ifstream file(path, std::ios::binary | std::ios::ate);
            if (!file)
            {
                return {};
            }

            const std::streamsize size = file.tellg();
            if (size <= 0 || (size % 4) != 0)
            {
                return {};
            }

            std::vector<std::uint32_t> data(static_cast<std::size_t>(size) / sizeof(std::uint32_t));
            file.seekg(0, std::ios::beg);
            file.read(reinterpret_cast<char*>(data.data()), size);
            return data;
        }

        QueueFamilySelection FindQueueFamily(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
        {
            std::uint32_t familyCount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &familyCount, nullptr);

            std::vector<VkQueueFamilyProperties> families(familyCount);
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &familyCount, families.data());

            for (std::uint32_t family = 0; family < familyCount; ++family)
            {
                VkBool32 presentSupported = VK_FALSE;
                vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, family, surface, &presentSupported);

                if ((families[family].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0 && presentSupported == VK_TRUE)
                {
                    return { family, true };
                }
            }

            return {};
        }

        SwapchainSupport QuerySwapchainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
        {
            SwapchainSupport support;
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &support.capabilities);

            std::uint32_t formatCount = 0;
            vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
            support.formats.resize(formatCount);
            if (formatCount > 0)
            {
                vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, support.formats.data());
            }

            std::uint32_t presentModeCount = 0;
            vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
            support.presentModes.resize(presentModeCount);
            if (presentModeCount > 0)
            {
                vkGetPhysicalDeviceSurfacePresentModesKHR(
                    physicalDevice, surface, &presentModeCount, support.presentModes.data());
            }

            return support;
        }

        VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats)
        {
            for (const VkSurfaceFormatKHR& format : formats)
            {
                if (format.format == VK_FORMAT_B8G8R8A8_UNORM &&
                    format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                {
                    return format;
                }
            }

            return formats.front();
        }

        VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities, Platform::Win32Window& window)
        {
            if (capabilities.currentExtent.width != std::numeric_limits<std::uint32_t>::max())
            {
                return capabilities.currentExtent;
            }

            VkExtent2D extent{ window.Width(), window.Height() };
            extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
            return extent;
        }

        struct VulkanState
        {
            VkInstance instance = VK_NULL_HANDLE;
            VkSurfaceKHR surface = VK_NULL_HANDLE;
            VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
            VkDevice device = VK_NULL_HANDLE;
            VkQueue graphicsQueue = VK_NULL_HANDLE;
            VkSwapchainKHR swapchain = VK_NULL_HANDLE;
            std::vector<VkImage> swapchainImages;
            std::vector<VkImageView> swapchainImageViews;
            std::vector<VkFramebuffer> framebuffers;
            VkFormat swapchainFormat = VK_FORMAT_UNDEFINED;
            VkExtent2D swapchainExtent{};
            VkRenderPass renderPass = VK_NULL_HANDLE;
            VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
            VkPipeline pipeline = VK_NULL_HANDLE;
            VkCommandPool commandPool = VK_NULL_HANDLE;
            VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
            VkSemaphore imageAvailable = VK_NULL_HANDLE;
            VkSemaphore renderFinished = VK_NULL_HANDLE;
            VkFence inFlight = VK_NULL_HANDLE;
            std::uint32_t queueFamily = 0;
            std::string adapterName;

            ~VulkanState()
            {
                if (device != VK_NULL_HANDLE)
                {
                    vkDeviceWaitIdle(device);
                    if (inFlight != VK_NULL_HANDLE)
                        vkDestroyFence(device, inFlight, nullptr);
                    if (renderFinished != VK_NULL_HANDLE)
                        vkDestroySemaphore(device, renderFinished, nullptr);
                    if (imageAvailable != VK_NULL_HANDLE)
                        vkDestroySemaphore(device, imageAvailable, nullptr);
                    if (commandPool != VK_NULL_HANDLE)
                        vkDestroyCommandPool(device, commandPool, nullptr);
                    if (pipeline != VK_NULL_HANDLE)
                        vkDestroyPipeline(device, pipeline, nullptr);
                    if (pipelineLayout != VK_NULL_HANDLE)
                        vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
                    if (renderPass != VK_NULL_HANDLE)
                        vkDestroyRenderPass(device, renderPass, nullptr);
                    for (VkFramebuffer framebuffer : framebuffers)
                        vkDestroyFramebuffer(device, framebuffer, nullptr);
                    for (VkImageView imageView : swapchainImageViews)
                        vkDestroyImageView(device, imageView, nullptr);
                    if (swapchain != VK_NULL_HANDLE)
                        vkDestroySwapchainKHR(device, swapchain, nullptr);
                    vkDestroyDevice(device, nullptr);
                }

                if (surface != VK_NULL_HANDLE)
                    vkDestroySurfaceKHR(instance, surface, nullptr);
                if (instance != VK_NULL_HANDLE)
                    vkDestroyInstance(instance, nullptr);
            }
        };

        bool CreateInstance(VulkanState& state, std::string& message)
        {
            VkApplicationInfo appInfo{};
            appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            appInfo.pApplicationName = "HFEngine Sandbox";
            appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
            appInfo.pEngineName = "HFEngine";
            appInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
            appInfo.apiVersion = VK_API_VERSION_1_2;

            const char* extensions[] = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME };

            VkInstanceCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            createInfo.pApplicationInfo = &appInfo;
            createInfo.enabledExtensionCount = static_cast<std::uint32_t>(std::size(extensions));
            createInfo.ppEnabledExtensionNames = extensions;

            return VkSucceeded(vkCreateInstance(&createInfo, nullptr, &state.instance), message, "vkCreateInstance");
        }

        bool CreateSurface(VulkanState& state, Platform::Win32Window& window, std::string& message)
        {
            VkWin32SurfaceCreateInfoKHR createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
            createInfo.hinstance = GetModuleHandleW(nullptr);
            createInfo.hwnd = static_cast<HWND>(window.NativeHandle());

            return VkSucceeded(
                vkCreateWin32SurfaceKHR(state.instance, &createInfo, nullptr, &state.surface),
                message,
                "vkCreateWin32SurfaceKHR");
        }

        bool PickPhysicalDevice(VulkanState& state, std::string& message)
        {
            std::uint32_t deviceCount = 0;
            vkEnumeratePhysicalDevices(state.instance, &deviceCount, nullptr);
            if (deviceCount == 0)
            {
                message = "No Vulkan physical devices found";
                return false;
            }

            std::vector<VkPhysicalDevice> devices(deviceCount);
            vkEnumeratePhysicalDevices(state.instance, &deviceCount, devices.data());

            for (VkPhysicalDevice device : devices)
            {
                const QueueFamilySelection family = FindQueueFamily(device, state.surface);
                const SwapchainSupport support = QuerySwapchainSupport(device, state.surface);
                if (!family.hasGraphicsFamily || support.formats.empty() || support.presentModes.empty())
                {
                    continue;
                }

                state.physicalDevice = device;
                state.queueFamily = family.graphicsFamily;

                VkPhysicalDeviceProperties properties{};
                vkGetPhysicalDeviceProperties(device, &properties);
                state.adapterName = properties.deviceName;
                return true;
            }

            message = "No Vulkan device supports graphics and presentation for this window";
            return false;
        }

        bool CreateDevice(VulkanState& state, std::string& message)
        {
            float priority = 1.0f;
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = state.queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &priority;

            const char* extensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

            VkDeviceCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            createInfo.queueCreateInfoCount = 1;
            createInfo.pQueueCreateInfos = &queueCreateInfo;
            createInfo.enabledExtensionCount = static_cast<std::uint32_t>(std::size(extensions));
            createInfo.ppEnabledExtensionNames = extensions;

            if (!VkSucceeded(vkCreateDevice(state.physicalDevice, &createInfo, nullptr, &state.device), message, "vkCreateDevice"))
            {
                return false;
            }

            vkGetDeviceQueue(state.device, state.queueFamily, 0, &state.graphicsQueue);
            return true;
        }

        bool CreateSwapchain(VulkanState& state, Platform::Win32Window& window, std::string& message)
        {
            const SwapchainSupport support = QuerySwapchainSupport(state.physicalDevice, state.surface);
            const VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(support.formats);
            const VkExtent2D extent = ChooseExtent(support.capabilities, window);

            std::uint32_t imageCount = support.capabilities.minImageCount + 1;
            if (support.capabilities.maxImageCount > 0 && imageCount > support.capabilities.maxImageCount)
            {
                imageCount = support.capabilities.maxImageCount;
            }

            VkSwapchainCreateInfoKHR createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            createInfo.surface = state.surface;
            createInfo.minImageCount = imageCount;
            createInfo.imageFormat = surfaceFormat.format;
            createInfo.imageColorSpace = surfaceFormat.colorSpace;
            createInfo.imageExtent = extent;
            createInfo.imageArrayLayers = 1;
            createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.preTransform = support.capabilities.currentTransform;
            createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
            createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
            createInfo.clipped = VK_TRUE;

            if (!VkSucceeded(vkCreateSwapchainKHR(state.device, &createInfo, nullptr, &state.swapchain), message, "vkCreateSwapchainKHR"))
            {
                return false;
            }

            state.swapchainFormat = surfaceFormat.format;
            state.swapchainExtent = extent;

            vkGetSwapchainImagesKHR(state.device, state.swapchain, &imageCount, nullptr);
            state.swapchainImages.resize(imageCount);
            vkGetSwapchainImagesKHR(state.device, state.swapchain, &imageCount, state.swapchainImages.data());

            for (VkImage image : state.swapchainImages)
            {
                VkImageViewCreateInfo imageViewInfo{};
                imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                imageViewInfo.image = image;
                imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
                imageViewInfo.format = state.swapchainFormat;
                imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                imageViewInfo.subresourceRange.levelCount = 1;
                imageViewInfo.subresourceRange.layerCount = 1;

                VkImageView imageView = VK_NULL_HANDLE;
                if (!VkSucceeded(vkCreateImageView(state.device, &imageViewInfo, nullptr, &imageView), message, "vkCreateImageView"))
                {
                    return false;
                }
                state.swapchainImageViews.push_back(imageView);
            }

            return true;
        }

        bool CreateRenderPass(VulkanState& state, std::string& message)
        {
            VkAttachmentDescription colorAttachment{};
            colorAttachment.format = state.swapchainFormat;
            colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
            colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

            VkAttachmentReference colorReference{};
            colorReference.attachment = 0;
            colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            VkSubpassDescription subpass{};
            subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass.colorAttachmentCount = 1;
            subpass.pColorAttachments = &colorReference;

            VkSubpassDependency dependency{};
            dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            dependency.dstSubpass = 0;
            dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

            VkRenderPassCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            createInfo.attachmentCount = 1;
            createInfo.pAttachments = &colorAttachment;
            createInfo.subpassCount = 1;
            createInfo.pSubpasses = &subpass;
            createInfo.dependencyCount = 1;
            createInfo.pDependencies = &dependency;

            return VkSucceeded(vkCreateRenderPass(state.device, &createInfo, nullptr, &state.renderPass), message, "vkCreateRenderPass");
        }

        bool CreateShaderModule(VulkanState& state, const std::vector<std::uint32_t>& spirv, VkShaderModule& outShader, std::string& message)
        {
            if (spirv.empty())
            {
                message = "SPIR-V shader file is missing or empty";
                return false;
            }

            VkShaderModuleCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            createInfo.codeSize = spirv.size() * sizeof(std::uint32_t);
            createInfo.pCode = spirv.data();
            return VkSucceeded(vkCreateShaderModule(state.device, &createInfo, nullptr, &outShader), message, "vkCreateShaderModule");
        }

        bool CreatePipeline(VulkanState& state, std::string& message)
        {
            const std::string shaderDir = ExecutableDirectory();
            const std::vector<std::uint32_t> vertexSpirv = ReadSpirv(shaderDir + "\\Triangle.vert.spv");
            const std::vector<std::uint32_t> fragmentSpirv = ReadSpirv(shaderDir + "\\Triangle.frag.spv");

            VkShaderModule vertexShader = VK_NULL_HANDLE;
            VkShaderModule fragmentShader = VK_NULL_HANDLE;
            if (!CreateShaderModule(state, vertexSpirv, vertexShader, message) ||
                !CreateShaderModule(state, fragmentSpirv, fragmentShader, message))
            {
                if (vertexShader != VK_NULL_HANDLE)
                    vkDestroyShaderModule(state.device, vertexShader, nullptr);
                if (fragmentShader != VK_NULL_HANDLE)
                    vkDestroyShaderModule(state.device, fragmentShader, nullptr);
                return false;
            }

            VkPipelineShaderStageCreateInfo shaderStages[2]{};
            shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
            shaderStages[0].module = vertexShader;
            shaderStages[0].pName = "main";
            shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            shaderStages[1].module = fragmentShader;
            shaderStages[1].pName = "main";

            VkPipelineVertexInputStateCreateInfo vertexInput{};
            vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

            VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
            inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

            VkViewport viewport{};
            viewport.width = static_cast<float>(state.swapchainExtent.width);
            viewport.height = static_cast<float>(state.swapchainExtent.height);
            viewport.maxDepth = 1.0f;

            VkRect2D scissor{};
            scissor.extent = state.swapchainExtent;

            VkPipelineViewportStateCreateInfo viewportState{};
            viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            viewportState.viewportCount = 1;
            viewportState.pViewports = &viewport;
            viewportState.scissorCount = 1;
            viewportState.pScissors = &scissor;

            VkPipelineRasterizationStateCreateInfo rasterizer{};
            rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
            rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
            rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
            rasterizer.lineWidth = 1.0f;

            VkPipelineMultisampleStateCreateInfo multisample{};
            multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

            VkPipelineColorBlendAttachmentState colorBlendAttachment{};
            colorBlendAttachment.colorWriteMask =
                VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

            VkPipelineColorBlendStateCreateInfo colorBlend{};
            colorBlend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            colorBlend.attachmentCount = 1;
            colorBlend.pAttachments = &colorBlendAttachment;

            VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
            pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            if (!VkSucceeded(
                    vkCreatePipelineLayout(state.device, &pipelineLayoutInfo, nullptr, &state.pipelineLayout),
                    message,
                    "vkCreatePipelineLayout"))
            {
                vkDestroyShaderModule(state.device, vertexShader, nullptr);
                vkDestroyShaderModule(state.device, fragmentShader, nullptr);
                return false;
            }

            VkGraphicsPipelineCreateInfo pipelineInfo{};
            pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            pipelineInfo.stageCount = 2;
            pipelineInfo.pStages = shaderStages;
            pipelineInfo.pVertexInputState = &vertexInput;
            pipelineInfo.pInputAssemblyState = &inputAssembly;
            pipelineInfo.pViewportState = &viewportState;
            pipelineInfo.pRasterizationState = &rasterizer;
            pipelineInfo.pMultisampleState = &multisample;
            pipelineInfo.pColorBlendState = &colorBlend;
            pipelineInfo.layout = state.pipelineLayout;
            pipelineInfo.renderPass = state.renderPass;

            const bool created = VkSucceeded(
                vkCreateGraphicsPipelines(state.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &state.pipeline),
                message,
                "vkCreateGraphicsPipelines");

            vkDestroyShaderModule(state.device, vertexShader, nullptr);
            vkDestroyShaderModule(state.device, fragmentShader, nullptr);
            return created;
        }

        bool CreateFramebuffers(VulkanState& state, std::string& message)
        {
            for (VkImageView imageView : state.swapchainImageViews)
            {
                VkFramebufferCreateInfo createInfo{};
                createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                createInfo.renderPass = state.renderPass;
                createInfo.attachmentCount = 1;
                createInfo.pAttachments = &imageView;
                createInfo.width = state.swapchainExtent.width;
                createInfo.height = state.swapchainExtent.height;
                createInfo.layers = 1;

                VkFramebuffer framebuffer = VK_NULL_HANDLE;
                if (!VkSucceeded(vkCreateFramebuffer(state.device, &createInfo, nullptr, &framebuffer), message, "vkCreateFramebuffer"))
                {
                    return false;
                }
                state.framebuffers.push_back(framebuffer);
            }

            return true;
        }

        bool CreateCommandsAndSync(VulkanState& state, std::string& message)
        {
            VkCommandPoolCreateInfo poolInfo{};
            poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            poolInfo.queueFamilyIndex = state.queueFamily;
            if (!VkSucceeded(vkCreateCommandPool(state.device, &poolInfo, nullptr, &state.commandPool), message, "vkCreateCommandPool"))
            {
                return false;
            }

            VkCommandBufferAllocateInfo allocateInfo{};
            allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocateInfo.commandPool = state.commandPool;
            allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocateInfo.commandBufferCount = 1;
            if (!VkSucceeded(vkAllocateCommandBuffers(state.device, &allocateInfo, &state.commandBuffer), message, "vkAllocateCommandBuffers"))
            {
                return false;
            }

            VkSemaphoreCreateInfo semaphoreInfo{};
            semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            if (!VkSucceeded(vkCreateSemaphore(state.device, &semaphoreInfo, nullptr, &state.imageAvailable), message, "vkCreateSemaphore(imageAvailable)") ||
                !VkSucceeded(vkCreateSemaphore(state.device, &semaphoreInfo, nullptr, &state.renderFinished), message, "vkCreateSemaphore(renderFinished)"))
            {
                return false;
            }

            VkFenceCreateInfo fenceInfo{};
            fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
            return VkSucceeded(vkCreateFence(state.device, &fenceInfo, nullptr, &state.inFlight), message, "vkCreateFence");
        }

        bool Initialize(VulkanState& state, Platform::Win32Window& window, std::string& message)
        {
            return CreateInstance(state, message) &&
                   CreateSurface(state, window, message) &&
                   PickPhysicalDevice(state, message) &&
                   CreateDevice(state, message) &&
                   CreateSwapchain(state, window, message) &&
                   CreateRenderPass(state, message) &&
                   CreatePipeline(state, message) &&
                   CreateFramebuffers(state, message) &&
                   CreateCommandsAndSync(state, message);
        }

        bool RecordCommandBuffer(VulkanState& state, std::uint32_t imageIndex, std::string& message)
        {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            if (!VkSucceeded(vkBeginCommandBuffer(state.commandBuffer, &beginInfo), message, "vkBeginCommandBuffer"))
            {
                return false;
            }

            VkClearValue clear{};
            clear.color = { { 0.025f, 0.035f, 0.055f, 1.0f } };

            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = state.renderPass;
            renderPassInfo.framebuffer = state.framebuffers[imageIndex];
            renderPassInfo.renderArea.extent = state.swapchainExtent;
            renderPassInfo.clearValueCount = 1;
            renderPassInfo.pClearValues = &clear;

            vkCmdBeginRenderPass(state.commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
            vkCmdBindPipeline(state.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, state.pipeline);
            vkCmdDraw(state.commandBuffer, 3, 1, 0, 0);
            vkCmdEndRenderPass(state.commandBuffer);

            return VkSucceeded(vkEndCommandBuffer(state.commandBuffer), message, "vkEndCommandBuffer");
        }

        bool RenderFrame(VulkanState& state, std::string& message)
        {
            vkWaitForFences(state.device, 1, &state.inFlight, VK_TRUE, UINT64_MAX);
            vkResetFences(state.device, 1, &state.inFlight);

            std::uint32_t imageIndex = 0;
            VkResult acquire = vkAcquireNextImageKHR(
                state.device, state.swapchain, UINT64_MAX, state.imageAvailable, VK_NULL_HANDLE, &imageIndex);
            if (acquire != VK_SUCCESS && acquire != VK_SUBOPTIMAL_KHR)
            {
                message = VkMessage(acquire, "vkAcquireNextImageKHR");
                return false;
            }

            vkResetCommandBuffer(state.commandBuffer, 0);
            if (!RecordCommandBuffer(state, imageIndex, message))
            {
                return false;
            }

            VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            VkSubmitInfo submitInfo{};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = &state.imageAvailable;
            submitInfo.pWaitDstStageMask = &waitStage;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &state.commandBuffer;
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = &state.renderFinished;

            if (!VkSucceeded(vkQueueSubmit(state.graphicsQueue, 1, &submitInfo, state.inFlight), message, "vkQueueSubmit"))
            {
                return false;
            }

            VkPresentInfoKHR presentInfo{};
            presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores = &state.renderFinished;
            presentInfo.swapchainCount = 1;
            presentInfo.pSwapchains = &state.swapchain;
            presentInfo.pImageIndices = &imageIndex;

            VkResult present = vkQueuePresentKHR(state.graphicsQueue, &presentInfo);
            if (present != VK_SUCCESS && present != VK_SUBOPTIMAL_KHR)
            {
                message = VkMessage(present, "vkQueuePresentKHR");
                return false;
            }

            return true;
        }
    }

    TriangleRunResult RunTriangleSandbox(const Core::EngineConfig& config, Platform::Win32Window& window)
    {
        TriangleRunResult result;

        VulkanState state;
        if (!Initialize(state, window, result.message))
        {
            return result;
        }

        result.adapterName = state.adapterName;

        while (window.PumpMessages())
        {
            if (!RenderFrame(state, result.message))
            {
                return result;
            }

            ++result.framesRendered;
            if (config.maxFrames != 0 && result.framesRendered >= config.maxFrames)
            {
                break;
            }
        }

        vkDeviceWaitIdle(state.device);
        result.success = true;
        result.message = "ok";
        return result;
    }
}
