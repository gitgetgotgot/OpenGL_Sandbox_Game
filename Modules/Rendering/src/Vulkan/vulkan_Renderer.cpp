#include "Rendering/Vulkan/vulkan_Renderer.h"
#include "Rendering/OpenGL/gl_ShaderPr.h"
#include "Rendering/OpenGL/gl_VL.h"
#include "Rendering/OpenGL/gl_VB.h"
#include "Rendering/OpenGL/gl_IB.h"
#include "Rendering/OpenGL/gl_UB.h"
#include "Rendering/OpenGL/gl_SB.h"
#include "Rendering/OpenGL/gl_Texture.h"
#include "Rendering/OpenGL/gl_TextureArray.h"
#include <Utility/Logger.h>
#include <Base/EngineError.h>

Engine::Graphics::Vulkan_Renderer::Vulkan_Renderer(VkInstance instance, VkSurfaceKHR surface, std::error_code& ec) {
	this->instance = instance;
	this->surface = surface;
	pick_physical_device(ec);
	if (!ec) create_logical_device(ec);
	if (!ec) init_VMA(ec);
	if (!ec) create_Swap_Chain(ec);
	if (!ec) create_render_pass(ec);
	if (!ec) create_frame_buffers(ec);
	if (!ec) create_command_pool_and_buffers(ec);
	if (!ec) create_semaphores_and_fences(ec);
}

Engine::Graphics::Vulkan_Renderer::~Vulkan_Renderer() {
	// wait before GPU finishes all tasks
	if (device != VK_NULL_HANDLE) {
		vkDeviceWaitIdle(device);
	}
	// destroy semaphores and fences
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (image_available_semaphores[i] != VK_NULL_HANDLE) vkDestroySemaphore(device, image_available_semaphores[i], nullptr);
		if (render_finished_semaphores[i] != VK_NULL_HANDLE) vkDestroySemaphore(device, render_finished_semaphores[i], nullptr);
		if (in_flight_fences[i] != VK_NULL_HANDLE) vkDestroyFence(device, in_flight_fences[i], nullptr);
	}
	// destroy command pool (command buffers are destroyed automatically here)
	if (command_pool != VK_NULL_HANDLE) {
		vkDestroyCommandPool(device, command_pool, nullptr);
		command_pool = VK_NULL_HANDLE;
	}
	// destroy swap chain framebuffers
	for (auto framebuffer : swap_chain.framebuffers) {
		if (framebuffer != VK_NULL_HANDLE) {
			vkDestroyFramebuffer(device, framebuffer, nullptr);
		}
	}
	swap_chain.framebuffers.clear();
	// destroy render pass
	if (render_pass != VK_NULL_HANDLE) {
		vkDestroyRenderPass(device, render_pass, nullptr);
		render_pass = VK_NULL_HANDLE;
	}
	// destroy image views (images are destroyed automatically with swap chain destruction)
	for (auto imageView : swap_chain.image_views) {
		if (imageView != VK_NULL_HANDLE) {
			vkDestroyImageView(device, imageView, nullptr);
		}
	}
	swap_chain.image_views.clear();
	// destroy swap chain
	if (swap_chain.chain != VK_NULL_HANDLE) {
		vkDestroySwapchainKHR(device, swap_chain.chain, nullptr);
		swap_chain.chain = VK_NULL_HANDLE;
	}
	// destroy vulkan memory allocator
	if (allocator != VK_NULL_HANDLE) {
		vmaDestroyAllocator(allocator);
		allocator = VK_NULL_HANDLE;
	}
	// destroy logical device
	if (device != VK_NULL_HANDLE) {
		vkDestroyDevice(device, nullptr);
		device = VK_NULL_HANDLE;
	}
	// destroy surface
	if (surface != VK_NULL_HANDLE) {
		vkDestroySurfaceKHR(instance, surface, nullptr);
		surface = VK_NULL_HANDLE;
	}
	// destroy instance
	if (instance != VK_NULL_HANDLE) {
		vkDestroyInstance(instance, nullptr);
		instance = VK_NULL_HANDLE;
	}
}

void Engine::Graphics::Vulkan_Renderer::pick_physical_device(std::error_code& ec) {
	uint32_t device_count = 0;
	vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
	if (device_count == 0) {
		ec = Engine::Errors::EngineError::VulkanDeviceNotFound;
		return;
	}

	std::vector<VkPhysicalDevice> devices(device_count);
	vkEnumeratePhysicalDevices(instance, &device_count, devices.data());

	for (const auto& dev : devices) {
		if (is_device_suitable(dev)) {
			physical_device = dev;
			break;
		}
	}

	if (physical_device == VK_NULL_HANDLE) {
		ec = Engine::Errors::EngineError::VulkanNotSupported;
	}
}

bool Engine::Graphics::Vulkan_Renderer::is_device_suitable(VkPhysicalDevice dev) {
	// Find index of a queue, that supports graphics
	uint32_t queue_family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(dev, &queue_family_count, nullptr);
	std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(dev, &queue_family_count, queue_families.data());

	int graphics_family_index = -1;
	for (int i = 0; i < queue_families.size(); i++) {
		// Find queue with graphics that supports current VkSurfaceKHR
		VkBool32 present_support = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(dev, i, surface, &present_support);

		if ((queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && present_support) {
			graphics_family_index = i;
			break; // universal queue is found
		}
	}
	if (graphics_family_index == -1) return false;

	// Check Swapchain extension support
	uint32_t extension_count = 0;
	vkEnumerateDeviceExtensionProperties(dev, nullptr, &extension_count, nullptr);
	std::vector<VkExtensionProperties> available_extensions(extension_count);
	vkEnumerateDeviceExtensionProperties(dev, nullptr, &extension_count, available_extensions.data());

	bool swapchain_supported = false;
	for (const auto& ext : available_extensions) {
		if (strcmp(ext.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0) {
			swapchain_supported = true;
			break; // swapchain is cupported
		}
	}

	//remember queue_family_index
	graphics_queue_family_index = static_cast<uint32_t>(graphics_family_index);

	return (graphics_family_index != -1) && swapchain_supported;
}

void Engine::Graphics::Vulkan_Renderer::create_logical_device(std::error_code& ec) {
	float queue_priority = 1.0f;
	VkDeviceQueueCreateInfo queue_create_info{};
	queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queue_create_info.queueFamilyIndex = graphics_queue_family_index;
	queue_create_info.queueCount = 1;
	queue_create_info.pQueuePriorities = &queue_priority;

	const std::vector<const char*> device_extensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	VkPhysicalDeviceFeatures device_features{};

	VkDeviceCreateInfo create_info{};
	create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	create_info.queueCreateInfoCount = 1;
	create_info.pQueueCreateInfos = &queue_create_info;
	create_info.pEnabledFeatures = &device_features;
	create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
	create_info.ppEnabledExtensionNames = device_extensions.data();

	if (vkCreateDevice(physical_device, &create_info, nullptr, &device) != VK_SUCCESS) {
		ec = Engine::Errors::EngineError::VulkanLogicalDeviceInitError;
		return;
	}

	vkGetDeviceQueue(device, graphics_queue_family_index, 0, &graphics_queue);
}

void Engine::Graphics::Vulkan_Renderer::init_VMA(std::error_code& ec) {
	VmaAllocatorCreateInfo allocator_info = {};
	allocator_info.physicalDevice = physical_device;
	allocator_info.device = device;
	allocator_info.instance = instance;
	allocator_info.vulkanApiVersion = VK_API_VERSION_1_2; // should be the same as in VkApplicationInfo in VkInstance

	if (vmaCreateAllocator(&allocator_info, &allocator) != VK_SUCCESS) {
		ec = Engine::Errors::EngineError::VulkanOutOfMemory;
	}
}

void Engine::Graphics::Vulkan_Renderer::create_Swap_Chain(std::error_code& ec) {
	// get surface capabilities, foramts and modes
	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &capabilities);

	uint32_t format_count = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, nullptr);
	std::vector<VkSurfaceFormatKHR> formats(format_count);
	vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, formats.data());

	uint32_t present_mode_count = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, nullptr);
	std::vector<VkPresentModeKHR> present_modes(present_mode_count);
	vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, present_modes.data());

	// choose optimal variant (RGBA8 with high color gamma)
	VkSurfaceFormatKHR surface_format = formats[0];
	for (const auto& fmt : formats) {
		if (fmt.format == VK_FORMAT_B8G8R8A8_SRGB && fmt.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			surface_format = fmt;
			break;
		}
	}

	// FIFO used as default -> classic Vsync
	VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
	swap_chain.has_mailbox_present_mode = false;
	for (const auto& mode : present_modes) { // check if MAILBOX_MODE supported
		if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
			swap_chain.has_mailbox_present_mode = true;
			break;
		}
	}
	current_present_mode = PresentMode::Double;

	VkExtent2D extent = capabilities.currentExtent;
	if (capabilities.currentExtent.width == UINT32_MAX || capabilities.currentExtent.height == UINT32_MAX) {
		extent = { 1280, 720 }; //default extent is HD, if window size is not received yet
	}

	// minimum 2 images are used in a swap chain for at least double bufferization
	uint32_t image_count = capabilities.minImageCount + 1;
	if (capabilities.maxImageCount > 0 && image_count > capabilities.maxImageCount) {
		image_count = capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR create_info{};
	create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	create_info.surface = surface;
	create_info.minImageCount = image_count;
	create_info.imageFormat = surface_format.format;
	create_info.imageColorSpace = surface_format.colorSpace;
	create_info.imageExtent = extent;
	create_info.imageArrayLayers = 1; // always 1, if it's not stereoscopic VR rendering
	create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // use images as targets for draw (shaders will output colors)
	create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // only 1 queue is used here for graphic and present, so use exclusive move

	create_info.queueFamilyIndexCount = 0;
	create_info.pQueueFamilyIndices = nullptr;

	create_info.preTransform = capabilities.currentTransform; // use current usual transform (no rotate or reflection)
	create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // no window transparency
	create_info.presentMode = present_mode;
	create_info.clipped = VK_TRUE; // remove overlapping pixels for optimization
	create_info.oldSwapchain = VK_NULL_HANDLE; // can be used when swap chain should be updated, also memory is reused (no fragmentation on GPU)

	if (vkCreateSwapchainKHR(device, &create_info, nullptr, &swap_chain.chain) != VK_SUCCESS) {
		ec = Engine::Errors::EngineError::VulkanSwapChainInitError;
		return;
	}
	surface_transform = capabilities.currentTransform;
	swap_chain.image_count = image_count;
	swap_chain.image_format = surface_format.format;
	swap_chain.image_color_space = surface_format.colorSpace;
	swap_chain.extent = extent;

	vkGetSwapchainImagesKHR(device, swap_chain.chain, &image_count, nullptr);
	swap_chain.images.resize(image_count);
	vkGetSwapchainImagesKHR(device, swap_chain.chain, &image_count, swap_chain.images.data());

	swap_chain.image_views.resize(image_count);
	for (size_t i = 0; i < image_count; i++) {
		VkImageViewCreateInfo view_info{};
		view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		view_info.image = swap_chain.images[i];
		view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		view_info.format = swap_chain.image_format;
		//default color channels
		view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		// ImageView is used for frame color
		view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		view_info.subresourceRange.baseMipLevel = 0;
		view_info.subresourceRange.levelCount = 1;
		view_info.subresourceRange.baseArrayLayer = 0;
		view_info.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device, &view_info, nullptr, &swap_chain.image_views[i]) != VK_SUCCESS) {
			ec = Engine::Errors::EngineError::VulkanImageViewInitError;
			return;
		}
	}
}

void Engine::Graphics::Vulkan_Renderer::recreate_Swap_Chain(
	uint32_t new_extent_width, uint32_t new_extent_height, PresentMode mode, std::error_code& ec
) {
	// update image extent
	swap_chain.extent = { new_extent_width, new_extent_height };
	current_present_mode = mode;

	//update present mode
	VkPresentModeKHR new_present_mode = VK_PRESENT_MODE_FIFO_KHR;
	if (mode == PresentMode::Immediate) {
		new_present_mode = VK_PRESENT_MODE_IMMEDIATE_KHR;
	}
	else if (mode == PresentMode::Triple && swap_chain.has_mailbox_present_mode) {
		new_present_mode = VK_PRESENT_MODE_MAILBOX_KHR;
	}

	// firstly wait for GPU tasks to be completed, then destroy old framebuffers and image views
	if (device != VK_NULL_HANDLE) {
		vkDeviceWaitIdle(device);
	}

	for (auto framebuffer : swap_chain.framebuffers) {
		if (framebuffer != VK_NULL_HANDLE) {
			vkDestroyFramebuffer(device, framebuffer, nullptr);
		}
	}
	swap_chain.framebuffers.clear();

	for (auto imageView : swap_chain.image_views) {
		if (imageView != VK_NULL_HANDLE) {
			vkDestroyImageView(device, imageView, nullptr);
		}
	}
	swap_chain.image_views.clear();

	VkSwapchainKHR old_chain = swap_chain.chain;

	// new swap chain create info
	VkSwapchainCreateInfoKHR create_info{};
	create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	create_info.surface = surface;
	create_info.minImageCount = swap_chain.image_count; //cached image count
	create_info.imageFormat = swap_chain.image_format; //cached image format
	create_info.imageColorSpace = swap_chain.image_color_space; //cached image color space
	create_info.imageExtent = swap_chain.extent; //updated extent
	create_info.imageArrayLayers = 1;
	create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	create_info.queueFamilyIndexCount = 0;
	create_info.pQueueFamilyIndices = nullptr;
	create_info.preTransform = surface_transform; //usual cached transform
	create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	create_info.presentMode = new_present_mode; //updated present mode
	create_info.clipped = VK_TRUE;
	create_info.oldSwapchain = old_chain; //reuse old swap chain GPU memory

	// create new swap chain and destroy old swap chain
	if (vkCreateSwapchainKHR(device, &create_info, nullptr, &swap_chain.chain) != VK_SUCCESS) {
		ec = Engine::Errors::EngineError::VulkanSwapChainInitError;
		return;
	}
	if (old_chain != VK_NULL_HANDLE) {
		vkDestroySwapchainKHR(device, old_chain, nullptr);
	}

	// get new images for swap chain
	uint32_t actual_image_count = 0;
	vkGetSwapchainImagesKHR(device, swap_chain.chain, &actual_image_count, nullptr);
	swap_chain.images.resize(actual_image_count);
	vkGetSwapchainImagesKHR(device, swap_chain.chain, &actual_image_count, swap_chain.images.data());
	
	// create new image views
	swap_chain.image_views.resize(actual_image_count);
	for (size_t i = 0; i < actual_image_count; i++) {
		VkImageViewCreateInfo view_info{};
		view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		view_info.image = swap_chain.images[i];
		view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		view_info.format = swap_chain.image_format;
		view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		view_info.subresourceRange.baseMipLevel = 0;
		view_info.subresourceRange.levelCount = 1;
		view_info.subresourceRange.baseArrayLayer = 0;
		view_info.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device, &view_info, nullptr, &swap_chain.image_views[i]) != VK_SUCCESS) {
			ec = Engine::Errors::EngineError::VulkanImageViewInitError;
			return;
		}
	}
	// create new framebuffers
	create_frame_buffers(ec);
}

void Engine::Graphics::Vulkan_Renderer::create_render_pass(std::error_code& ec) {
	// description of color attachment in swap chain image
	VkAttachmentDescription color_attachment{};
	color_attachment.format = swap_chain.image_format;
	color_attachment.samples = VK_SAMPLE_COUNT_1_BIT; // no sampling here (for now)
	color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // clear screen in the beginning of rendering with background color
	color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // save image result to present on the screen
	color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // doesn't matter what was on the image before
	color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // final layout used to present image on the screen

	// subpass reference
	VkAttachmentReference color_attachment_ref{};
	color_attachment_ref.attachment = 0; // index in array pAttachments (here only one is used with index = 0)
	color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	// subpass description (just one render subpass is used here for main screen)
	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; // graphics pipeline (no compute)
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_attachment_ref;

	// subpass dependencies (synchronization of memory layouts)
	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL; // external environment for render pass
	dependency.dstSubpass = 0;                   // current first subpass
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo render_pass_info{};
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_info.attachmentCount = 1;
	render_pass_info.pAttachments = &color_attachment;
	render_pass_info.subpassCount = 1;
	render_pass_info.pSubpasses = &subpass;
	render_pass_info.dependencyCount = 1;
	render_pass_info.pDependencies = &dependency;

	if (vkCreateRenderPass(device, &render_pass_info, nullptr, &render_pass) != VK_SUCCESS) {
		ec = Engine::Errors::EngineError::VulkanRenderPassInitError;
		return;
	}
}

void Engine::Graphics::Vulkan_Renderer::create_frame_buffers(std::error_code& ec) {
	swap_chain.framebuffers.resize(swap_chain.image_views.size());

	for (size_t i = 0; i < swap_chain.image_views.size(); i++) {
		VkImageView attachments[] = {
			swap_chain.image_views[i]
		};

		VkFramebufferCreateInfo framebuffer_info{};
		framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuffer_info.renderPass = render_pass;
		framebuffer_info.attachmentCount = 1;
		framebuffer_info.pAttachments = attachments;
		framebuffer_info.width = swap_chain.extent.width;
		framebuffer_info.height = swap_chain.extent.height;
		framebuffer_info.layers = 1; // layers count (for VR this will be 2)

		if (vkCreateFramebuffer(device, &framebuffer_info, nullptr, &swap_chain.framebuffers[i]) != VK_SUCCESS) {
			ec = Engine::Errors::EngineError::VulkanFramebufferInitError;
			return;
		}
	}
}

void Engine::Graphics::Vulkan_Renderer::create_command_pool_and_buffers(std::error_code& ec) {
	VkCommandPoolCreateInfo pool_info{};
	pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // allow reset/rewrite buffers
	pool_info.queueFamilyIndex = graphics_queue_family_index; //index of graphics queue, that was cached before while finding physical device

	if (vkCreateCommandPool(device, &pool_info, nullptr, &command_pool) != VK_SUCCESS) {
		ec = Engine::Errors::EngineError::VulkanCommandPoolInitError;
		return;
	}

	// allocate command buffers for each framebuffer/image in swapchain
	command_buffers.resize(MAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo alloc_info{};
	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.commandPool = command_pool;
	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // primary buffer, can be sent directly in queue
	alloc_info.commandBufferCount = static_cast<uint32_t>(command_buffers.size());

	if (vkAllocateCommandBuffers(device, &alloc_info, command_buffers.data()) != VK_SUCCESS) {
		ec = Engine::Errors::EngineError::VulkanCommandBufferAllocError;
		return;
	}
}

void Engine::Graphics::Vulkan_Renderer::create_semaphores_and_fences(std::error_code& ec) {
	image_available_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
	render_finished_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
	in_flight_fences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphore_info{};
	semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fence_info{};
	fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT; // create fence in a "signaled" state for the first frame

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(device, &semaphore_info, nullptr, &image_available_semaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(device, &semaphore_info, nullptr, &render_finished_semaphores[i]) != VK_SUCCESS ||
			vkCreateFence(device, &fence_info, nullptr, &in_flight_fences[i]) != VK_SUCCESS) {

			ec = Engine::Errors::EngineError::VulkanSyncObjectsInitError;
			return;
		}
	}
}


void Engine::Graphics::Vulkan_Renderer::start_frame(std::error_code& ec) {
	// if window is hidden, then just skip render
	if (swap_chain.extent.width == 0 || swap_chain.extent.height == 0) {
		return;
	}

	// wait before GPU finishes processing last frame
	if (vkWaitForFences(device, 1, &in_flight_fences[current_frame_index], VK_TRUE, UINT64_MAX) != VK_SUCCESS) {
		ec = Engine::Errors::EngineError::VulkanFenceWaitError;
		return;
	}
	// get index of free image from swap chain
	VkResult result = vkAcquireNextImageKHR(
		device,
		swap_chain.chain,
		UINT64_MAX,
		image_available_semaphores[current_frame_index],
		VK_NULL_HANDLE,
		&current_image_index // real image index (0, 1 or 2)
	);

	//recreate swap chain and just return, so old frame will be rendered
	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreate_Swap_Chain(swap_chain.extent.width, swap_chain.extent.height, current_present_mode, ec);
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		ec = Engine::Errors::EngineError::VulkanAcquireImageError;
		return;
	}

	// reset fence of current frame
	if (vkResetFences(device, 1, &in_flight_fences[current_frame_index]) != VK_SUCCESS) {
		ec = Engine::Errors::EngineError::VulkanFenceResetError;
		return;
	}

	// reset and begin writing to command buffer, connected to the current frame
	vkResetCommandBuffer(command_buffers[current_frame_index], 0);

	VkCommandBufferBeginInfo begin_info{};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(command_buffers[current_frame_index], &begin_info) != VK_SUCCESS) {
		ec = Engine::Errors::EngineError::VulkanCommandBufferBeginError;
		return;
	}
}

void Engine::Graphics::Vulkan_Renderer::clear(float r, float g, float b) {
	VkClearValue clear_color{ { { r, g, b, 1.0f } } };

	VkRenderPassBeginInfo render_pass_info{};
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	render_pass_info.renderPass = render_pass;
	render_pass_info.framebuffer = swap_chain.framebuffers[current_image_index];
	render_pass_info.renderArea.offset = { 0, 0 };
	render_pass_info.renderArea.extent = swap_chain.extent;
	render_pass_info.clearValueCount = 1;
	render_pass_info.pClearValues = &clear_color;

	// write command to open render pass and clear
	vkCmdBeginRenderPass(command_buffers[current_frame_index], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
}

void Engine::Graphics::Vulkan_Renderer::present_frame(std::error_code& ec) {
	// finish render pass and buffer
	vkCmdEndRenderPass(command_buffers[current_frame_index]);
	if (vkEndCommandBuffer(command_buffers[current_frame_index]) != VK_SUCCESS) {
		ec = Engine::Errors::EngineError::VulkanCommandBufferEndError;
		return;
	}

	// submit to queue with synchronization through semaphore of current frame
	VkSubmitInfo submit_info{};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore wait_semaphores[] = { image_available_semaphores[current_frame_index] };
	VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = wait_semaphores;
	submit_info.pWaitDstStageMask = wait_stages;

	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &command_buffers[current_frame_index];

	VkSemaphore signal_semaphores[] = { render_finished_semaphores[current_frame_index] };
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = signal_semaphores;

	if (vkQueueSubmit(graphics_queue, 1, &submit_info, in_flight_fences[current_frame_index]) != VK_SUCCESS) {
		ec = Engine::Errors::EngineError::VulkanQueueSubmitError;
		return;
	}

	// presentation of image to the screen, using current_image_index
	VkPresentInfoKHR present_info{};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = signal_semaphores;

	VkSwapchainKHR swapchains[] = { swap_chain.chain };
	present_info.swapchainCount = 1;
	present_info.pSwapchains = swapchains;
	present_info.pImageIndices = &current_image_index;

	VkResult result = vkQueuePresentKHR(present_queue, &present_info);

	//recreate swap chain if something in the display was changed
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		recreate_Swap_Chain(swap_chain.extent.width, swap_chain.extent.height, current_present_mode, ec);
	}
	else if (result != VK_SUCCESS) {
		ec = Engine::Errors::EngineError::VulkanPresentError;
		return;
	}

	// now current frame is the next one (0 or 1)
	current_frame_index = (current_frame_index + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Engine::Graphics::Vulkan_Renderer::on_window_resize(uint32_t window_width, uint32_t window_height, PresentMode mode) {
	std::error_code ec;
	recreate_Swap_Chain(window_width, window_height, mode, ec);
}

void Engine::Graphics::Vulkan_Renderer::set_viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
	
}

void Engine::Graphics::Vulkan_Renderer::use_depth_test(bool use) {
	
}

void Engine::Graphics::Vulkan_Renderer::use_depth_write(bool use) {
	
}

void Engine::Graphics::Vulkan_Renderer::use_depth_func(DepthFunc func) {
	
}

void Engine::Graphics::Vulkan_Renderer::use_scissor_test(bool use) {
	
}

void Engine::Graphics::Vulkan_Renderer::set_scissor_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
	
}

std::unique_ptr<Engine::Graphics::ShaderProgram> Engine::Graphics::Vulkan_Renderer::create_ShaderPr(
	const std::filesystem::path& vertex_shader_path,
	const std::filesystem::path& fragment_shader_path,
	std::error_code& ec
) const {
	return std::make_unique<gl_ShaderPr>(vertex_shader_path, fragment_shader_path);
}

std::unique_ptr<Engine::Graphics::VertexLayout> Engine::Graphics::Vulkan_Renderer::create_VertexLayout(std::error_code& ec) const {
	return std::make_unique<gl_VL>();
}

std::unique_ptr<Engine::Graphics::VertexBuffer> Engine::Graphics::Vulkan_Renderer::create_VertexBuffer(std::error_code& ec) const {
	return std::make_unique<gl_VB>();
}

std::unique_ptr<Engine::Graphics::IndexBuffer> Engine::Graphics::Vulkan_Renderer::create_IndexBuffer(std::error_code& ec) const {
	return std::make_unique<gl_IB>();
}

std::unique_ptr<Engine::Graphics::UniformBuffer> Engine::Graphics::Vulkan_Renderer::create_UniformBuffer(std::error_code& ec) const {
	return std::make_unique<gl_UB>();
}

std::unique_ptr<Engine::Graphics::StorageBuffer> Engine::Graphics::Vulkan_Renderer::create_StorageBuffer(std::error_code& ec) const {
	return std::make_unique<gl_SB>();
}

std::unique_ptr<Engine::Graphics::Texture> Engine::Graphics::Vulkan_Renderer::create_Texture(std::error_code& ec) const {
	return std::make_unique<gl_Texture>();
}

std::unique_ptr<Engine::Graphics::TextureArray> Engine::Graphics::Vulkan_Renderer::create_TextureArray(std::error_code& ec) const {
	return std::make_unique<gl_TextureArray>();
}

void Engine::Graphics::Vulkan_Renderer::renderData(
	const ShaderProgram& shader,
	const VertexLayout& va,
	const VertexBuffer& vb
) {
	
}

void Engine::Graphics::Vulkan_Renderer::renderIndexedData(
	const ShaderProgram& shader,
	const VertexLayout& va,
	const VertexBuffer& vb,
	const IndexBuffer& ib,
	uint32_t indexCount,
	uint32_t baseIndex
) {
	
}

void Engine::Graphics::Vulkan_Renderer::renderIndexedDataVAOSpecific(
	const VertexLayout& va,
	uint32_t indexCount
) {
	
}

void Engine::Graphics::Vulkan_Renderer::renderInstancedData(
	const ShaderProgram& shader,
	const VertexLayout& va,
	const VertexBuffer& vb,
	const IndexBuffer& ib,
	uint32_t indexCount,
	uint32_t instanceCount
) {
	
}

void Engine::Graphics::Vulkan_Renderer::renderBasedInstancedData(
	const ShaderProgram& shader,
	const VertexLayout& va,
	const VertexBuffer& vb,
	const IndexBuffer& ib,
	uint32_t indexCount,
	uint32_t instanceCount,
	uint32_t baseInstance
) {
	
}