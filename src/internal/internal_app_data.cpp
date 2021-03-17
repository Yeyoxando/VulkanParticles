/*
 *	Author: Diego Ochando Torres
 *  Date: 07/01/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

// ------------------------------------------------------------------------- //

#include "basic_ps_app.h"
#include "../src/internal/internal_app_data.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <tiny_obj_loader.h>

#define VK_USE_PLATFORM_WIN32_KHR

// Provisional
#include "components/component_mesh.h"
#include "components/component_material.h"
#include "components/component_transform.h"

// ----------------------- Functions definition ---------------------------- //

ParticleEditor::AppData::AppData() {

  window_ = nullptr;
  window_width_ = 0;
  window_height_ = 0;

  instance_ = VK_NULL_HANDLE;
  debug_messenger_ = VK_NULL_HANDLE;
  physical_device_ = VK_NULL_HANDLE;
  logical_device_ = VK_NULL_HANDLE;
  graphics_queue_ = VK_NULL_HANDLE;
  present_queue_ = VK_NULL_HANDLE;
  surface_ = VK_NULL_HANDLE;
  swap_chain_ = VK_NULL_HANDLE;
  swap_chain_extent_ = {};
  render_pass_ = VK_NULL_HANDLE;

  vertex_buffers_ = std::vector<Buffer*>(0);
  index_buffers_ = std::vector<Buffer*>(0);

  materials_ = std::vector<Material*>(0);
  texture_images_ = std::vector<Image*>(0);

  depth_image_ = nullptr;
  color_image_ = nullptr;

  msaa_samples_ = VK_SAMPLE_COUNT_1_BIT;

  current_frame_ = 0;
  resized_framebuffer_ = false;
  close_window_ = false;

  system_draw_objects_ = new SystemDrawObjects();

}

// ------------------------------------------------------------------------- //

ParticleEditor::AppData::~AppData(){

  for (int i = 0; i < materials_.size(); i++) {
    delete materials_[i];
  }

  delete system_draw_objects_;

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::initWindow(int width /*= 800*/, int height /*= 600*/) {

  glfwInit();

  // Tell GLFW to not use OpenGL API
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

  window_ = glfwCreateWindow(width, height, "Hello Particles", nullptr, nullptr);
  window_width_ = width;
  window_height_ = height;

  glfwSetWindowUserPointer(window_, this);
  glfwSetFramebufferSizeCallback(window_, framebufferResizeCallback);
  glfwSetInputMode(window_, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::initVulkan() {

  createInstance();
  setupDebugMessenger();
  createSurface();
  pickPhysicalDevice();
  createLogicalDevice();
  createSwapChain();
  createRenderPass();
  setupMaterials();
  createDescriptorSetLayouts();
  createPipelineLayouts();
  createGraphicsPipelines();
  createCommandPool();
  createColorResources();
  createDepthResources();
  createFramebuffers();
  createTextureImages();
  setupVertexBuffers();
	setupIndexBuffers();
	loadModels();
	createDescriptorPools();
	initializeDescriptorSets();
  createCommandBuffers();
  createSyncObjects();

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::renderLoopEnd() {

  // Wait until the device is on idle so we can clean up resources without being in use
  vkDeviceWaitIdle(logical_device_);

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::closeVulkan() {

  // Vulkan cleanup
  cleanupSwapChain();

  for (int i = 0; i < texture_images_.size(); i++) {
    texture_images_[i]->clean(logical_device_);
    delete texture_images_[i];
  }


  vkDestroyDescriptorSetLayout(logical_device_, scene_descriptor_set_layout_, nullptr);
  vkDestroyDescriptorSetLayout(logical_device_, models_descriptor_set_layout_, nullptr);
  vkDestroyDescriptorSetLayout(logical_device_, opaque_descriptor_set_layout_, nullptr);


  for (int i = 0; i < index_buffers_.size(); i++) {
    index_buffers_[i]->clean(logical_device_);
    delete index_buffers_[i];
  }

  for (int i = 0; i < vertex_buffers_.size(); i++) {
    vertex_buffers_[i]->clean(logical_device_);
    delete vertex_buffers_[i];
  }

  for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkDestroySemaphore(logical_device_, available_image_semaphores_[i], nullptr);
    vkDestroySemaphore(logical_device_, finished_render_semaphores_[i], nullptr);
    vkDestroyFence(logical_device_, in_flight_fences_[i], nullptr);
  }

  vkDestroyCommandPool(logical_device_, command_pool_, nullptr);

  vkDestroyDevice(logical_device_, nullptr);

  vkDestroySurfaceKHR(instance_, surface_, nullptr);

  if (kEnableValidationLayers) {
    DestroyDebugUtilsMessengerEXT(instance_, debug_messenger_, nullptr);
  }

  vkDestroyInstance(instance_, nullptr);

  // Window close
  glfwDestroyWindow(window_);
  glfwTerminate();

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::createInstance() {

  // Check for validation layers
  if (kEnableValidationLayers && !checkValidationLayerSupport()) {
    throw std::runtime_error("Requested validation layers are not available.");
  }

  // Custom application info
  VkApplicationInfo app_info{};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = "Hello Triangle";
  app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.pEngineName = "No Engine";
  app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.apiVersion = VK_API_VERSION_1_0;

  auto extensions = getRequiredExtensions();

  // Instance create info
  VkInstanceCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pApplicationInfo = &app_info;
  create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  create_info.ppEnabledExtensionNames = extensions.data();
  // Specific for debug instance creation, destroyed in cleanup
  VkDebugUtilsMessengerCreateInfoEXT debug_create_info;
  if (kEnableValidationLayers) {
    create_info.enabledLayerCount = static_cast<uint32_t>(kVkValidationLayers.size());
    create_info.ppEnabledLayerNames = kVkValidationLayers.data();

    // Enable debug messenger for the instance
    populateDebugMessengerCreateInfo(debug_create_info);
    create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debug_create_info;
  }
  else {
    create_info.enabledLayerCount = 0;
  }

  // Create the instance
  if (vkCreateInstance(&create_info, nullptr, &instance_) != VK_SUCCESS) {
    throw std::runtime_error("Vulkan instance creation failed.");
  }

  // Report available Vulkan extensions
  uint32_t available_extensions = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &available_extensions, nullptr);

  std::vector<VkExtensionProperties> extension_properties(available_extensions);
  vkEnumerateInstanceExtensionProperties(nullptr, &available_extensions, extension_properties.data());

  printf("Available Vulkan extensions:");
  for (uint32_t i = 0; i < available_extensions; i++) {
    printf("\n\t. %s", extension_properties[i].extensionName);
  }

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::createSurface() {

  if (glfwCreateWindowSurface(instance_, window_, nullptr, &surface_) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create window surface.");
  }

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::pickPhysicalDevice() {

  // Request available physical devices (graphics cards)
  uint32_t device_count = 0;
  vkEnumeratePhysicalDevices(instance_, &device_count, nullptr);

  if (device_count == 0) {
    throw std::runtime_error("The device does not have any GPUs supporting Vulkan.");
  }

  std::vector<VkPhysicalDevice> devices(device_count);
  vkEnumeratePhysicalDevices(instance_, &device_count, devices.data());

  for (uint32_t i = 0; i < devices.size(); i++) {
    if (isDeviceSuitable(devices[i])) {
      physical_device_ = devices[i];
      msaa_samples_ = getMaxUsableSampleCount();
      break;
    }
  }

  if (physical_device_ == VK_NULL_HANDLE) {
    throw std::runtime_error("The device's GPUs are not suitable for the requested functionality.");
  }

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::createLogicalDevice() {

  // Create needed queues
  QueueFamilyIndices indices = findQueueFamilies(physical_device_);
  float queue_priority = 1.0f;

  // Store unique queues
  std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
  std::set<uint32_t> unique_queue_families = { indices.graphics_family.value(),
    indices.present_family.value() };

  // Create unique queues create info
  for (uint32_t queue_family : unique_queue_families) {
    VkDeviceQueueCreateInfo queue_create_info{};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = queue_family;
    queue_create_info.queueCount = 1;
    queue_create_info.pQueuePriorities = &queue_priority;
    queue_create_infos.push_back(queue_create_info);
  }

  // Set needed vulkan features
  VkPhysicalDeviceFeatures device_features{};
  device_features.samplerAnisotropy = VK_TRUE;

  // Create the device
  VkDeviceCreateInfo device_create_info{};
  device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  device_create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
  device_create_info.pQueueCreateInfos = queue_create_infos.data();
  device_create_info.pEnabledFeatures = &device_features;
  device_create_info.enabledExtensionCount = static_cast<uint32_t>(kDeviceExtensions.size());
  device_create_info.ppEnabledExtensionNames = kDeviceExtensions.data();

  // Only for older Vulkan apps, layers don't need to be set in the device on actual versions
  if (kEnableValidationLayers) {
    device_create_info.enabledLayerCount = static_cast<uint32_t>(kVkValidationLayers.size());
    device_create_info.ppEnabledLayerNames = kVkValidationLayers.data();
  }
  else {
    device_create_info.enabledLayerCount = 0;
  }

  if (vkCreateDevice(physical_device_, &device_create_info, nullptr, &logical_device_) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create Vulkan logical device.");
  }

  // Store graphics queue handle
  vkGetDeviceQueue(logical_device_, indices.graphics_family.value(), 0, &graphics_queue_);

  // Store present queue handle
  vkGetDeviceQueue(logical_device_, indices.present_family.value(), 0, &present_queue_);

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::createSwapChain() {

  if (window_width_ == 0 || window_height_ == 0) return;

  // Choose preferred details
  SwapChainSupportDetails details = querySwapChainSupportDetails(physical_device_);

  VkSurfaceFormatKHR format = chooseSwapChainSurfaceFormat(details.formats);
  VkPresentModeKHR present_mode = chooseSwapChainPresentMode(details.present_modes);
  VkExtent2D extent = chooseSwapChainExtent(details.capabilities);

  // Request one more image than the min to prevent waiting for them
  uint32_t image_count = details.capabilities.minImageCount + 1;
  // But not exceeding the max number (0 indicates no constraints)
  if (details.capabilities.maxImageCount > 0 && image_count > details.capabilities.maxImageCount) {
    image_count = details.capabilities.maxImageCount;
  }

  // Create info structure
  VkSwapchainCreateInfoKHR create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  create_info.surface = surface_;
  create_info.minImageCount = image_count;
  create_info.imageFormat = format.format;
  create_info.imageColorSpace = format.colorSpace;
  create_info.imageExtent = extent;
  create_info.imageArrayLayers = 1;
  create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;//VK_IMAGE_USAGE_TRANSFER_DST_BIT for rendering to another buffer i.e.

  QueueFamilyIndices indices = findQueueFamilies(physical_device_);
  uint32_t queue_family_indices[] = { indices.graphics_family.value(), indices.present_family.value() };
  if (indices.graphics_family != indices.present_family) {
    // Used in multiple queues
    create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    create_info.queueFamilyIndexCount = 2; // Indicate number of shared families
    create_info.pQueueFamilyIndices = queue_family_indices; // Shared families
  }
  else {
    // Used exclusively in one queue
    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.queueFamilyIndexCount = 0; // Optional
    create_info.pQueueFamilyIndices = nullptr; // Optional
  }

  // No image modification when presenting
  create_info.preTransform = details.capabilities.currentTransform;
  // No alpha with other windows
  create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

  create_info.presentMode = present_mode;
  create_info.clipped = VK_TRUE;
  create_info.oldSwapchain = VK_NULL_HANDLE; // Set later when resizing or something like that

  if (vkCreateSwapchainKHR(logical_device_, &create_info, nullptr, &swap_chain_) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create swap chain.");
  }

  // Store image handles for render operation
  vkGetSwapchainImagesKHR(logical_device_, swap_chain_, &image_count, nullptr);
  swap_chain_images_.resize(image_count);
  std::vector<VkImage> tmp_images;
  tmp_images.resize(image_count);
  vkGetSwapchainImagesKHR(logical_device_, swap_chain_, &image_count, tmp_images.data());
  for (uint32_t i = 0; i < image_count; i++) {
    swap_chain_images_[i] = new Image(Image::kImageType_Framebuffer);
    swap_chain_images_[i]->image_ = tmp_images[i];
  }

  // Store format and extent for future operations
  swap_chain_image_format_ = format.format;
  swap_chain_extent_ = extent;

  // Create the image views
  for (int i = 0; i < swap_chain_images_.size(); i++) {
    swap_chain_images_[i]->createImageView(logical_device_, swap_chain_image_format_, VK_IMAGE_ASPECT_COLOR_BIT);
  }

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::createRenderPass() {

  if (window_width_ == 0 || window_height_ == 0) return;

  // How to load and store the received info for color
  VkAttachmentDescription color_attachment{};
  color_attachment.format = swap_chain_image_format_;
  color_attachment.samples = msaa_samples_;
  // Color and depth data
  color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  // Stencil data
  color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference color_attachment_ref{};
  color_attachment_ref.attachment = 0;
  color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  // How to load and store the received info for depth
  VkAttachmentDescription depth_attachment{};
  depth_attachment.format = findDepthFormat();
  depth_attachment.samples = msaa_samples_;
  depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depth_attachment_ref{};
  depth_attachment_ref.attachment = 1;
  depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  // Color attachment transformation to present after msaa
  VkAttachmentDescription color_attachment_resolve{};
  color_attachment_resolve.format = swap_chain_image_format_;
  color_attachment_resolve.samples = VK_SAMPLE_COUNT_1_BIT;
  color_attachment_resolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment_resolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment_resolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment_resolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment_resolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment_resolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference color_attachment_resolve_ref{};
  color_attachment_resolve_ref.attachment = 2;
  color_attachment_resolve_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  // Sub passes (only 1 for now)
  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &color_attachment_ref; // layout location = 0 in frag shader
  subpass.pDepthStencilAttachment = &depth_attachment_ref;
  subpass.pResolveAttachments = &color_attachment_resolve_ref;

  // Dependency for the render pass to start (Until color attachment is finished)
  VkSubpassDependency subpass_dependency{};
  subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  subpass_dependency.dstSubpass = 0;
  subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
    VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  subpass_dependency.srcAccessMask = 0;
  subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
    VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  subpass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
    VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

  // Create the render pass
  std::array<VkAttachmentDescription, 3> attachments = { color_attachment, depth_attachment, color_attachment_resolve };
  VkRenderPassCreateInfo render_pass_info{};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_info.attachmentCount = static_cast<uint32_t>(attachments.size());
  render_pass_info.pAttachments = attachments.data();
  render_pass_info.subpassCount = 1;
  render_pass_info.pSubpasses = &subpass;
  render_pass_info.dependencyCount = 1;
  render_pass_info.pDependencies = &subpass_dependency;

  if (vkCreateRenderPass(logical_device_, &render_pass_info, nullptr, &render_pass_) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create the render pass");
  }

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::createDescriptorSetLayouts() {

  // COMMON 
  // (Scene descriptor set layout)
	VkDescriptorSetLayoutBinding vp_ubo_layout_binding{};
	vp_ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	vp_ubo_layout_binding.binding = 0;
	vp_ubo_layout_binding.descriptorCount = 1;
	vp_ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	vp_ubo_layout_binding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutCreateInfo dsl_scene_create_info{};
	dsl_scene_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	dsl_scene_create_info.bindingCount = 1;
	dsl_scene_create_info.pBindings = &vp_ubo_layout_binding;

	if (vkCreateDescriptorSetLayout(logical_device_, &dsl_scene_create_info, nullptr, &scene_descriptor_set_layout_) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create scene descriptor set layout.");
	}

  // (Models descriptor set layout)
	VkDescriptorSetLayoutBinding models_ubo_layout_binding{};
	models_ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	models_ubo_layout_binding.binding = 0;
	models_ubo_layout_binding.descriptorCount = 1;
	models_ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	models_ubo_layout_binding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutCreateInfo dsl_models_create_info{};
	dsl_models_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	dsl_models_create_info.bindingCount = 1;
	dsl_models_create_info.pBindings = &models_ubo_layout_binding;

	if (vkCreateDescriptorSetLayout(logical_device_, &dsl_models_create_info, nullptr, &models_descriptor_set_layout_) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create models descriptor set layout.");
	}



  // MATERIAL OPAQUE
  // Create the binding for the vertex shader MVP matrices
  VkDescriptorSetLayoutBinding opaque_ubo_layout_binding{};
  opaque_ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
  opaque_ubo_layout_binding.binding = 0;
  opaque_ubo_layout_binding.descriptorCount = 1;
  opaque_ubo_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  opaque_ubo_layout_binding.pImmutableSamplers = nullptr;

  VkDescriptorSetLayoutBinding sampler_layout_binding{};
  sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  sampler_layout_binding.binding = 1;
  sampler_layout_binding.descriptorCount = loaded_textures_.size();
  sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  sampler_layout_binding.pImmutableSamplers = nullptr;

  // Create the descriptor set layout
  std::array<VkDescriptorSetLayoutBinding, 2> opaque_bindings = { opaque_ubo_layout_binding, sampler_layout_binding };

  VkDescriptorSetLayoutCreateInfo opaque_create_info{};
  opaque_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  opaque_create_info.bindingCount = static_cast<uint32_t>(opaque_bindings.size());
  opaque_create_info.pBindings = opaque_bindings.data();

  if (vkCreateDescriptorSetLayout(logical_device_, &opaque_create_info, nullptr, &opaque_descriptor_set_layout_) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create opaque descriptor set layout.");
  }



  // MATERIAL TRANSLUCENT
	/*// Create the binding for the vertex shader MVP matrices
	VkDescriptorSetLayoutBinding translucent_ubo_layout_binding{};
	translucent_ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	translucent_ubo_layout_binding.binding = 0;
	translucent_ubo_layout_binding.descriptorCount = 1;
	translucent_ubo_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	translucent_ubo_layout_binding.pImmutableSamplers = nullptr;

	// Create the descriptor set layout
	std::array<VkDescriptorSetLayoutBinding, 2> translucent_bindings = { translucent_ubo_layout_binding, sampler_layout_binding };

	VkDescriptorSetLayoutCreateInfo translucent_create_info{};
	translucent_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	translucent_create_info.bindingCount = static_cast<uint32_t>(translucent_bindings.size());
	translucent_create_info.pBindings = translucent_bindings.data();

	if (vkCreateDescriptorSetLayout(logical_device_, &translucent_create_info, nullptr, &materials_[1]->descriptor_set_layout_) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create translucent descriptor set layout.");
	}*/

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::createPipelineLayouts(){

	// OPAQUE MATERIAL PIPELINE LAYOUT
  std::array<VkDescriptorSetLayout, 3> opaque_descriptor_set_layouts{ 
    scene_descriptor_set_layout_, models_descriptor_set_layout_, 
    opaque_descriptor_set_layout_
  };

  VkPipelineLayoutCreateInfo opaque_layout_info{};
	opaque_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	opaque_layout_info.setLayoutCount = static_cast<uint32_t>(opaque_descriptor_set_layouts.size());
	opaque_layout_info.pSetLayouts = opaque_descriptor_set_layouts.data();
	opaque_layout_info.pushConstantRangeCount = 0;
	opaque_layout_info.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(logical_device_, &opaque_layout_info, nullptr, &materials_[0]->pipeline_layout_) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create opaque pipeline layout.");
	}



	// TRANSLUCENT MATERIAL PIPELINE LAYOUT
	std::array<VkDescriptorSetLayout, 3> translucent_descriptor_set_layouts{
		scene_descriptor_set_layout_, models_descriptor_set_layout_,
		opaque_descriptor_set_layout_
	};

	VkPipelineLayoutCreateInfo translucent_layout_info{};
	translucent_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  translucent_layout_info.setLayoutCount = static_cast<uint32_t>(translucent_descriptor_set_layouts.size());
	translucent_layout_info.pSetLayouts = translucent_descriptor_set_layouts.data();
	translucent_layout_info.pushConstantRangeCount = 0;
	translucent_layout_info.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(logical_device_, &translucent_layout_info, nullptr, &materials_[1]->pipeline_layout_) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create translucent pipeline layout.");
	}

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::createGraphicsPipelines() {

  if (window_width_ == 0 || window_height_ == 0) return;

  // OPAQUE MATERIAL PIPELINE

  // Load shaders
  auto vert_shader_code = readFile("../../../resources/shaders/shaders_spirv/v_hello_triangle.spv");
  auto frag_shader_code = readFile("../../../resources/shaders/shaders_spirv/f_hello_triangle.spv");

  VkShaderModule vert_shader_module = createShaderModule(vert_shader_code);
  VkShaderModule frag_shader_module = createShaderModule(frag_shader_code);

  
  VkPipelineShaderStageCreateInfo vert_shader_stage_info{};
  vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vert_shader_stage_info.module = vert_shader_module;
  vert_shader_stage_info.pName = "main";
  vert_shader_stage_info.pSpecializationInfo = nullptr; // Constants can be defined here
  
  // Fragment shader num textures constant
  VkSpecializationMapEntry entry{};
  entry.constantID = 0;
  entry.offset = 0;
  entry.size = sizeof(int32_t);
  VkSpecializationInfo spec_info{};
  spec_info.mapEntryCount = 1;
  spec_info.pMapEntries = &entry;
  spec_info.dataSize = sizeof(uint32_t);
  uint32_t data = loaded_textures_.size();
  spec_info.pData = &data;

  VkPipelineShaderStageCreateInfo frag_shader_stage_info{};
  frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  frag_shader_stage_info.module = frag_shader_module;
  frag_shader_stage_info.pName = "main";
  frag_shader_stage_info.pSpecializationInfo = &spec_info; // Constants can be defined here

  VkPipelineShaderStageCreateInfo shader_stages[] = { vert_shader_stage_info, frag_shader_stage_info };

  // Set vertex input format
  auto binding_desc = Vertex::getBindingDescription();
  auto attribute_desc = Vertex::getAttributeDescription();

  VkPipelineVertexInputStateCreateInfo vertex_input_info{};
  vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertex_input_info.vertexBindingDescriptionCount = 1;
  vertex_input_info.pVertexBindingDescriptions = &binding_desc;
  vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_desc.size());
  vertex_input_info.pVertexAttributeDescriptions = attribute_desc.data();

  // Set input assembly settings
  VkPipelineInputAssemblyStateCreateInfo input_assembly_info{};
  input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  input_assembly_info.primitiveRestartEnable = VK_FALSE; // True to use index buffers with triangle strip

  // Create viewport area to draw
  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)swap_chain_extent_.width;
  viewport.height = (float)swap_chain_extent_.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  // Create scissor rectangle with extent size
  VkRect2D scissor{};
  scissor.offset = { 0, 0 };
  scissor.extent = swap_chain_extent_;

  VkPipelineViewportStateCreateInfo viewport_state_info{};
  viewport_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_state_info.viewportCount = 1;
  viewport_state_info.pViewports = &viewport;
  viewport_state_info.scissorCount = 1;
  viewport_state_info.pScissors = &scissor;

  // Create the rasterizer settings
  VkPipelineRasterizationStateCreateInfo rasterizer_state_info{};
  rasterizer_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer_state_info.depthClampEnable = VK_FALSE;
  rasterizer_state_info.rasterizerDiscardEnable = VK_FALSE;
  rasterizer_state_info.polygonMode = VK_POLYGON_MODE_FILL; // Line to draw wire frame but require a GPU feature
  rasterizer_state_info.lineWidth = 1.0f;
  rasterizer_state_info.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer_state_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; // Turned counter-clockwise due to glm y clip flip on the projection matrix
  rasterizer_state_info.depthBiasEnable = VK_FALSE;
  rasterizer_state_info.depthBiasClamp = 0.0f;
  rasterizer_state_info.depthBiasConstantFactor = 0.0f;
  rasterizer_state_info.depthBiasSlopeFactor = 0.0f;

  // Create multi sampling settings
  VkPipelineMultisampleStateCreateInfo multisample_state_info{};
  multisample_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisample_state_info.sampleShadingEnable = VK_FALSE;
  multisample_state_info.rasterizationSamples = msaa_samples_;
  multisample_state_info.minSampleShading = 1.0f;
  multisample_state_info.pSampleMask = nullptr;
  multisample_state_info.alphaToCoverageEnable = VK_FALSE;
  multisample_state_info.alphaToOneEnable = VK_FALSE;

  // Create depth and stencil settings for the framebuffer
  VkPipelineDepthStencilStateCreateInfo depth_stencil_info{};
  depth_stencil_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depth_stencil_info.depthTestEnable = VK_TRUE;
  depth_stencil_info.depthWriteEnable = VK_TRUE;
  depth_stencil_info.depthCompareOp = VK_COMPARE_OP_LESS;
  depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
  depth_stencil_info.minDepthBounds = 0.0f;
  depth_stencil_info.maxDepthBounds = 1.0f;
  depth_stencil_info.stencilTestEnable = VK_FALSE;
  depth_stencil_info.front = {};
  depth_stencil_info.back = {};

  // Create color blend settings
  VkPipelineColorBlendAttachmentState color_blend_attachment{};
  color_blend_attachment.blendEnable = VK_FALSE;
  color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
  color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
  color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
  // Alpha blending
  color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
  color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
    VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

  VkPipelineColorBlendStateCreateInfo blend_state_info{};
  blend_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  blend_state_info.logicOpEnable = VK_FALSE;
  blend_state_info.logicOp = VK_LOGIC_OP_COPY;
  blend_state_info.attachmentCount = 1;
  blend_state_info.pAttachments = &color_blend_attachment;
  /*blend_state_info.blendConstants[0] = 0.0f;
  blend_state_info.blendConstants[1] = 0.0f;
  blend_state_info.blendConstants[2] = 0.0f;
  blend_state_info.blendConstants[3] = 0.0f;*/

  // Create dynamic settings of the pipeline
  VkDynamicState dynamic_states[] = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_LINE_WIDTH,
  };

  VkPipelineDynamicStateCreateInfo dynamic_state_info{};
  dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamic_state_info.dynamicStateCount = 2;
  dynamic_state_info.pDynamicStates = dynamic_states;

  // Create the actual graphics pipeline
  VkGraphicsPipelineCreateInfo opaque_pipeline_info{};
  opaque_pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  opaque_pipeline_info.stageCount = 2;
  opaque_pipeline_info.pStages = shader_stages;
  opaque_pipeline_info.pVertexInputState = &vertex_input_info;
  opaque_pipeline_info.pInputAssemblyState = &input_assembly_info;
  opaque_pipeline_info.pViewportState = &viewport_state_info;
  opaque_pipeline_info.pRasterizationState = &rasterizer_state_info;
  opaque_pipeline_info.pMultisampleState = &multisample_state_info;
  opaque_pipeline_info.pDepthStencilState = &depth_stencil_info;
  opaque_pipeline_info.pColorBlendState = &blend_state_info;
  opaque_pipeline_info.pDynamicState = nullptr;
  opaque_pipeline_info.layout = materials_[0]->pipeline_layout_;
  opaque_pipeline_info.renderPass = render_pass_;
  opaque_pipeline_info.subpass = 0;
  opaque_pipeline_info.basePipelineHandle = VK_NULL_HANDLE; // derive from other graphics pipeline with flags VK_PIPELINE_CREATE_DERIVATIVE_BIT
  opaque_pipeline_info.basePipelineIndex = -1;

  if (vkCreateGraphicsPipelines(logical_device_, VK_NULL_HANDLE, 1, &opaque_pipeline_info,
    nullptr, &materials_[0]->graphics_pipeline_) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create the opaque graphics pipeline.");
  }


  // TRANSLUCENT MATERIAL PIPELINE
  // Modification of the 1st one

  // VShader
  vert_shader_code = readFile("../../../resources/shaders/shaders_spirv/v_hello_billboard.spv");
  VkShaderModule billboard_vert_shader_module = createShaderModule(vert_shader_code);

	VkPipelineShaderStageCreateInfo billboard_shader_stage_info = vert_shader_stage_info;
	billboard_shader_stage_info.module = billboard_vert_shader_module;

  VkPipelineShaderStageCreateInfo billboard_shader_stages[] = { billboard_shader_stage_info, frag_shader_stage_info };

  // Blend
	VkPipelineColorBlendAttachmentState translucent_color_blend_attachment = color_blend_attachment;
  translucent_color_blend_attachment.blendEnable = VK_TRUE;
	translucent_color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	translucent_color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	translucent_color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
	translucent_color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	translucent_color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	translucent_color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
  translucent_color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

  
	VkPipelineColorBlendStateCreateInfo translucent_blend_state_info = blend_state_info;
  translucent_blend_state_info.pAttachments = &translucent_color_blend_attachment;

  // Depth
	VkPipelineDepthStencilStateCreateInfo translucent_depth_stencil_info = depth_stencil_info;
	translucent_depth_stencil_info.depthTestEnable = VK_FALSE;

	// MSAA
	//VkPipelineMultisampleStateCreateInfo translucent_multisample_state_info = multisample_state_info;
  //translucent_multisample_state_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;


  // Translucent pipeline
	VkGraphicsPipelineCreateInfo translucent_pipeline_info = opaque_pipeline_info;
  translucent_pipeline_info.stageCount = 2;
  translucent_pipeline_info.pStages = billboard_shader_stages;
	translucent_pipeline_info.layout = materials_[1]->pipeline_layout_;
  translucent_pipeline_info.pColorBlendState = &translucent_blend_state_info;
  translucent_pipeline_info.pDepthStencilState = &translucent_depth_stencil_info;
  //translucent_pipeline_info.pMultisampleState = &translucent_multisample_state_info;

	if (vkCreateGraphicsPipelines(logical_device_, VK_NULL_HANDLE, 1, &translucent_pipeline_info,
		nullptr, &materials_[1]->graphics_pipeline_) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create the translucent graphics pipeline.");
	}

  // Destroy shader modules as they're not used anymore
  vkDestroyShaderModule(logical_device_, vert_shader_module, nullptr);
  vkDestroyShaderModule(logical_device_, billboard_vert_shader_module, nullptr);
  vkDestroyShaderModule(logical_device_, frag_shader_module, nullptr);

}

// ------------------------------------------------------------------------- //

VkShaderModule ParticleEditor::AppData::createShaderModule(const std::vector<char>& bytecode) {

  VkShaderModuleCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  create_info.codeSize = bytecode.size();
  create_info.pCode = reinterpret_cast<const uint32_t*>(bytecode.data());

  VkShaderModule shader_module;

  if (vkCreateShaderModule(logical_device_, &create_info, nullptr, &shader_module) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create the shader module.");
  }

  return shader_module;

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::createFramebuffers() {

  if (window_width_ == 0 || window_height_ == 0) return;

  swap_chain_framebuffers_.resize(swap_chain_images_.size());

  for (int i = 0; i < swap_chain_images_.size(); i++) {
    std::array<VkImageView, 3> attachments = {
      color_image_->image_view_,
      depth_image_->image_view_,
      swap_chain_images_[i]->image_view_,
    };

    VkFramebufferCreateInfo framebuffer_info{};
    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_info.renderPass = render_pass_;
    framebuffer_info.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebuffer_info.pAttachments = attachments.data();
    framebuffer_info.width = swap_chain_extent_.width;
    framebuffer_info.height = swap_chain_extent_.height;
    framebuffer_info.layers = 1;

    if (vkCreateFramebuffer(logical_device_, &framebuffer_info, nullptr, &swap_chain_framebuffers_[i]) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create framebuffer.");
    }
  }

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::createCommandPool() {

  QueueFamilyIndices indices = findQueueFamilies(physical_device_);

  VkCommandPoolCreateInfo command_pool_info{};
  command_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  command_pool_info.queueFamilyIndex = indices.graphics_family.value();
  command_pool_info.flags = 0; // VK_COMMAND_POOL_CREATE_TRANSIENT_BIT means that command buffers will be rerecorded with new commands very often

  if (vkCreateCommandPool(logical_device_, &command_pool_info, nullptr, &command_pool_) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create command pool.");
  }

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::createColorResources() {

  VkFormat format = swap_chain_image_format_;

  color_image_ = new Image(Image::kImageType_Framebuffer);
  color_image_->create(physical_device_, logical_device_, swap_chain_extent_.width,
    swap_chain_extent_.height, msaa_samples_, format, VK_IMAGE_TILING_OPTIMAL,
    VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  color_image_->createImageView(logical_device_, format, VK_IMAGE_ASPECT_COLOR_BIT);
  
}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::createDepthResources() {

  VkFormat format = findDepthFormat();

  depth_image_ = new Image(Image::kImageType_Framebuffer);

  depth_image_->create(physical_device_, logical_device_, swap_chain_extent_.width,
    swap_chain_extent_.height, msaa_samples_, format, VK_IMAGE_TILING_OPTIMAL,
    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  depth_image_->createImageView(logical_device_, format, VK_IMAGE_ASPECT_DEPTH_BIT);
  
  transitionImageLayout(depth_image_->image_, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::createTextureImages() {

  int tex_width;
  int tex_height;
  int tex_channels;
  stbi_uc* pixels;
  VkDeviceSize image_size;

	auto it = loaded_textures_.cbegin();
  while (it != loaded_textures_.cend()) {
    Image* texture_image = new Image(Image::kImageType_Texture);

    // Load any texture with 4 channels with STBI_rgb_alpha
    pixels = stbi_load(it->second,
      &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);

    image_size = tex_width * tex_height * 4;

    if (!pixels) {
      throw std::runtime_error("Failed to load texture.");
    }

    // Create a staging buffer to transfer it to the device memory
    Buffer* staging_buffer = new Buffer(Buffer::kBufferType_Image);
    staging_buffer->create(physical_device_, logical_device_, image_size,
      VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    // Copy values to the buffer
    void* data;
    vkMapMemory(logical_device_, staging_buffer->buffer_memory_, 0, image_size, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(image_size));
    vkUnmapMemory(logical_device_, staging_buffer->buffer_memory_);

    stbi_image_free(pixels);

    // Create the texture image
    texture_image->create(physical_device_, logical_device_, tex_width, tex_height, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB,
      VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  
    transitionImageLayout(texture_image->image_, VK_FORMAT_R8G8B8A8_SRGB,
      VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    texture_image->copyFromBuffer(logical_device_, command_pool_, graphics_queue_, *staging_buffer,
      static_cast<uint32_t>(tex_width), static_cast<uint32_t>(tex_height));
    transitionImageLayout(texture_image->image_, VK_FORMAT_R8G8B8A8_SRGB,
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    staging_buffer->clean(logical_device_);
    delete staging_buffer;

    texture_image->createImageView(logical_device_, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
    texture_image->createSampler(physical_device_, logical_device_);

    texture_images_.push_back(texture_image);

    ++it;
	}

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::loadModels() {

	// Load model from file
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

  auto it = loaded_models_.cbegin();
	while (it != loaded_models_.cend()) {

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

    const char* model_path = it->second;
		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, model_path)) {
			throw std::runtime_error(warn + err);
		}

		// Create the model with indices
		std::unordered_map<Vertex, uint32_t> unique_vertices{};

		for (const auto& shape : shapes) {
			for (const auto& index : shape.mesh.indices) {
				Vertex vertex{};

				vertex.position = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};

				vertex.tex_coord = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
				};

				vertex.color = { 1.0f, 1.0f, 1.0f };

				if (unique_vertices.count(vertex) == 0) {
					unique_vertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}

				indices.push_back(unique_vertices[vertex]);
			}
		}

		createVertexBuffer(vertices);
		createIndexBuffer(indices);

    ++it;
  }

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::createVertexBuffer(std::vector<Vertex> &vertices) {

  // Driver developers says that buffers should be created in only one VkBuffer
  // and use them with the offset properties that are in the functions
  // this will allow cache optimizations known as aliasing
	vertex_buffers_.push_back(new Buffer(Buffer::kBufferType_Vertex));

  // Create a staging buffer and allocate its memory
  VkDeviceSize buffer_size = sizeof(vertices[0]) * vertices.size();
  Buffer* staging_buffer = new Buffer(Buffer::kBufferType_Vertex);
  staging_buffer->create(physical_device_, logical_device_, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  // Map the memory from the CPU to GPU
  void* data;
  vkMapMemory(logical_device_, staging_buffer->buffer_memory_, 0, buffer_size, 0, &data);
  memcpy(data, vertices.data(), (size_t)buffer_size);
  vkUnmapMemory(logical_device_, staging_buffer->buffer_memory_);

  // The memory is not copied instantly, but it should be before the next call to vkQueueSubmit
  // This could cause performance decreases and it could be done with a host coherent memory heap
  // or using vkFlushMappedMemoryRanges after writing to mapped memory and vkInvalidateMappedMemoryRanges before reading


  // Create the actual vertex buffer
  vertex_buffers_[vertex_buffers_.size() - 1]->create(physical_device_, logical_device_, buffer_size,
    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  // Copy the content from the staging buffer to the vertex buffer (allocated in gpu memory)
  vertex_buffers_[vertex_buffers_.size() - 1]->copy(logical_device_, command_pool_, graphics_queue_, *staging_buffer, buffer_size);

  // Delete and free the staging buffer
  staging_buffer->clean(logical_device_);
  delete staging_buffer;

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::createIndexBuffer(std::vector<uint32_t>& indices) {

	index_buffers_.push_back(new Buffer(Buffer::kBufferType_Index));

  // Create a staging buffer and allocate its memory
  VkDeviceSize buffer_size = sizeof(indices[0]) * indices.size();
  
  Buffer* staging_buffer = new Buffer(Buffer::kBufferType_Index);
  staging_buffer->create(physical_device_, logical_device_,
    buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, indices.size());

  // Map the memory from the CPU to GPU
  void* data;
  vkMapMemory(logical_device_, staging_buffer->buffer_memory_, 0, buffer_size, 0, &data);
  memcpy(data, indices.data(), (size_t)buffer_size);
  vkUnmapMemory(logical_device_, staging_buffer->buffer_memory_);

  // Create the actual index buffer
  index_buffers_[index_buffers_.size() - 1]->create(physical_device_, logical_device_, buffer_size, 
    VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indices.size());
  
  // Copy the content from the staging buffer to the vertex buffer (allocated in gpu memory)
  index_buffers_[index_buffers_.size() - 1]->copy(logical_device_, command_pool_, graphics_queue_, *staging_buffer, buffer_size);
  
  // Delete and free the staging buffer
  staging_buffer->clean(logical_device_);
  delete staging_buffer;
  
}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::createDescriptorPools() {

  // COMMON DESCRIPTOR POOLS 
  // Scene
  VkDescriptorPoolSize scene_pool_size{};
  scene_pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  scene_pool_size.descriptorCount = static_cast<uint32_t>(swap_chain_images_.size());

	VkDescriptorPoolCreateInfo scene_dp_create_info{};
	scene_dp_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	scene_dp_create_info.poolSizeCount = 1;
	scene_dp_create_info.pPoolSizes = &scene_pool_size;
	scene_dp_create_info.maxSets = static_cast<uint32_t>(swap_chain_images_.size());
	
  if (vkCreateDescriptorPool(logical_device_, &scene_dp_create_info, nullptr, &scene_descriptor_pool_) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create scene descriptor pool.");
	}

  // Models
	VkDescriptorPoolSize models_pool_size{};
  models_pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
  models_pool_size.descriptorCount = static_cast<uint32_t>(swap_chain_images_.size());

	VkDescriptorPoolCreateInfo models_dp_create_info{};
	models_dp_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	models_dp_create_info.poolSizeCount = 1;
	models_dp_create_info.pPoolSizes = &models_pool_size;
	models_dp_create_info.maxSets = static_cast<uint32_t>(swap_chain_images_.size());

	if (vkCreateDescriptorPool(logical_device_, &models_dp_create_info, nullptr, &models_descriptor_pool_) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create models descriptor pool.");
	}



  // OPAQUE MATERIAL DESCRIPTOR POOL
  // Create descriptor pool info
  int num_textures = loaded_textures_.size();
  std::vector<VkDescriptorPoolSize> pool_sizes = std::vector<VkDescriptorPoolSize>(1 + num_textures);
  pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	pool_sizes[0].descriptorCount = static_cast<uint32_t>(swap_chain_images_.size());
  for (int i = 1; i < num_textures + 1; ++i) {
    pool_sizes[i].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    pool_sizes[i].descriptorCount = static_cast<uint32_t>(swap_chain_images_.size());
  }

  VkDescriptorPoolCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  create_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
  create_info.pPoolSizes = pool_sizes.data();
  create_info.maxSets = static_cast<uint32_t>(swap_chain_images_.size());

  if (vkCreateDescriptorPool(logical_device_, &create_info, nullptr, &opaque_descriptor_pool_) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create opaque descriptor pool.");
  }



	// TRANSLUCENT MATERIAL DESCRIPTOR POOL
	// Create descriptor pool info
	/*std::array<VkDescriptorPoolSize, 2> pool_sizes{};
	pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	pool_sizes[0].descriptorCount = static_cast<uint32_t>(swap_chain_images_.size());
	pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	pool_sizes[1].descriptorCount = static_cast<uint32_t>(swap_chain_images_.size());

	VkDescriptorPoolCreateInfo create_info{};
	create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	create_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
	create_info.pPoolSizes = pool_sizes.data();
	create_info.maxSets = static_cast<uint32_t>(swap_chain_images_.size());*/

	/*if (vkCreateDescriptorPool(logical_device_, &create_info, nullptr, &materials_[1]->descriptor_pool_) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create translucent descriptor pool.");
	}*/

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::initializeDescriptorSets(){

	createUniformBuffers(sizeof(SceneUBO), scene_uniform_buffers_);
	createDynamicUniformBuffers(models_uniform_buffers_);
	createOpaqueDynamicUniformBuffers(opaque_uniform_buffers_);
	populateSceneDescriptorSets();
	populateModelsDescriptorSets();
	populateOpaqueDescriptorSets();

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::createCommandBuffers() {

  if (window_width_ == 0 || window_height_ == 0) return;

  command_buffers_.resize(swap_chain_framebuffers_.size());

  // Allocate command buffers
  VkCommandBufferAllocateInfo allocate_info{};
  allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocate_info.commandPool = command_pool_;
  allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocate_info.commandBufferCount = (uint32_t)command_buffers_.size();

  if (vkAllocateCommandBuffers(logical_device_, &allocate_info, command_buffers_.data()) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create command buffers.");
  }

  // Record the command buffers
  for (int i = 0; i < command_buffers_.size(); i++) {
    // Begin command buffers recording
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = 0;
    begin_info.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(command_buffers_[i], &begin_info) != VK_SUCCESS) {
      throw std::runtime_error("Failed to begin command buffer recording.");
    }

    // Set the render pass begin info
    VkRenderPassBeginInfo render_pass_begin{};
    render_pass_begin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_begin.renderPass = render_pass_;
    render_pass_begin.framebuffer = swap_chain_framebuffers_[i];
    render_pass_begin.renderArea.offset = { 0, 0 };
    render_pass_begin.renderArea.extent = swap_chain_extent_;
    std::array<VkClearValue, 2> clear_values{};
    clear_values[0].color = { 0.0f, 0.0f, 0.0f, 1.0f }; // Black
    clear_values[1].depthStencil = { 1.0f, 0 };
    render_pass_begin.clearValueCount = static_cast<uint32_t>(clear_values.size());
    render_pass_begin.pClearValues = clear_values.data();

    // Begin recording the commands on the command buffer
    vkCmdBeginRenderPass(command_buffers_[i], &render_pass_begin, VK_SUBPASS_CONTENTS_INLINE);

    // Call draw system to prepare the commands for all the entities
    system_draw_objects_->drawObjectsCommand(i, command_buffers_[i], 
      ParticleEditor::instance().active_scene_->getEntities());

    // Finish recording commands
    vkCmdEndRenderPass(command_buffers_[i]);

    if (vkEndCommandBuffer(command_buffers_[i]) != VK_SUCCESS) {
      throw std::runtime_error("Failed to end command buffer recording.");
    }

  }

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::createSyncObjects() {

  available_image_semaphores_.resize(MAX_FRAMES_IN_FLIGHT);
  finished_render_semaphores_.resize(MAX_FRAMES_IN_FLIGHT);
  in_flight_fences_.resize(MAX_FRAMES_IN_FLIGHT);
  images_in_flight_.resize(swap_chain_images_.size(), VK_NULL_HANDLE);

  VkSemaphoreCreateInfo semaphore_info{};
  semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fence_info{};
  fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    if ((vkCreateSemaphore(logical_device_, &semaphore_info, nullptr, &available_image_semaphores_[i]) ||
      vkCreateSemaphore(logical_device_, &semaphore_info, nullptr, &finished_render_semaphores_[i]) ||
      vkCreateFence(logical_device_, &fence_info, nullptr, &in_flight_fences_[i]))
      != VK_SUCCESS) {
      throw std::runtime_error("Failed to create synchronization objects for a frame.");
    }
  }

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::updateFrame() {

  // Calculate time since rendering started
  static auto start_time = std::chrono::high_resolution_clock::now();

  auto current_time = std::chrono::high_resolution_clock::now();

  float time = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();  
  

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::updateUniformBuffers(uint32_t current_image) {

  // THIS SHOULD BE ON THE SCENE
  // Calculate time since rendering started
  static auto start_time = std::chrono::high_resolution_clock::now();

  auto current_time = std::chrono::high_resolution_clock::now();

  float time = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();
  
  // Get model matrix (this will be on the system somehow
  //auto transform = static_cast<ComponentTransform*>(BasicPSApp::instance().active_scene_->getEntities()[0]->
  //  getComponent(Component::ComponentKind::kComponentKind_Transform));
  // test to see transform comp working
  //transform->rotate(glm::vec3(0.0f, 0.0f, 5.0f * time));
  

  // Update the dynamic buffer using the draw system
  system_draw_objects_->updateUniformBuffers(current_image,
    ParticleEditor::instance().active_scene_->getEntities());

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::drawFrame() {

  if (window_width_ == 0 || window_height_ == 0) return;

  // Wait until a frame is available
  vkWaitForFences(logical_device_, 1, &in_flight_fences_[current_frame_], VK_TRUE, UINT64_MAX);

  // Acquire an image from the swap chain
  uint32_t image_index;
  VkResult result = vkAcquireNextImageKHR(logical_device_, swap_chain_, UINT64_MAX, available_image_semaphores_[current_frame_],
    VK_NULL_HANDLE, &image_index); // UINT64_MAX stands for no timeout

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    recreateSwapChain();
  }
  else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("Failed to acquire swap chain image.");
  }

  // Check if a previous frame is using the image
  if (images_in_flight_[image_index] != VK_NULL_HANDLE) {
    vkWaitForFences(logical_device_, 1, &images_in_flight_[image_index], VK_TRUE, UINT64_MAX);
  }

  // Mark the image as used in this frame
  images_in_flight_[image_index] = in_flight_fences_[current_frame_];

  // Update the uniform buffers
  updateUniformBuffers(image_index);

  // Execute the command buffer with that image as attachment in the framebuffer
  VkSubmitInfo submit_info{};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  VkSemaphore wait_semaphores[] = { available_image_semaphores_[current_frame_] };
  VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
  submit_info.waitSemaphoreCount = 1;
  submit_info.pWaitSemaphores = wait_semaphores;
  submit_info.pWaitDstStageMask = wait_stages;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &command_buffers_[image_index];
  VkSemaphore signal_semaphores[] = { finished_render_semaphores_[current_frame_] };
  submit_info.signalSemaphoreCount = 1;
  submit_info.pSignalSemaphores = signal_semaphores;

  vkResetFences(logical_device_, 1, &in_flight_fences_[current_frame_]);

  if (vkQueueSubmit(graphics_queue_, 1, &submit_info, in_flight_fences_[current_frame_]) != VK_SUCCESS) {
    throw std::runtime_error("Failed to submit draw command buffer.");
  }

  // Return the image to the swap chain for presentation
  VkPresentInfoKHR present_info{};
  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present_info.waitSemaphoreCount = 1;
  present_info.pWaitSemaphores = signal_semaphores;
  VkSwapchainKHR swap_chains[] = { swap_chain_ };
  present_info.swapchainCount = 1;
  present_info.pSwapchains = swap_chains;
  present_info.pImageIndices = &image_index;
  present_info.pResults = nullptr;

  result = vkQueuePresentKHR(graphics_queue_, &present_info);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || resized_framebuffer_) {
    resized_framebuffer_ = false;
    recreateSwapChain();
  }
  else if (result != VK_SUCCESS) {
    throw std::runtime_error("Failed to present swap chain image.");
  }

  current_frame_ = (current_frame_ + 1) % MAX_FRAMES_IN_FLIGHT;

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::recreateSwapChain() {

  int width = 0;
  int height = 0;
  glfwGetFramebufferSize(window_, &width, &height);
  if (width == 0 || height == 0) {
    glfwGetFramebufferSize(window_, &width, &height);
    glfwWaitEvents();
  }

  vkDeviceWaitIdle(logical_device_);

  cleanupSwapChain();

  createSwapChain();
	createRenderPass();
	createPipelineLayouts();
  createGraphicsPipelines();
  createColorResources();
  createDepthResources();
  createFramebuffers();
  createDescriptorPools();
  initializeDescriptorSets();
  createCommandBuffers();

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::cleanupSwapChain() {

  // Render images
  color_image_->clean(logical_device_);
  depth_image_->clean(logical_device_);
  delete color_image_;
  delete depth_image_;

  // Swap chain frame buffers
  for (int i = 0; i < swap_chain_framebuffers_.size(); i++) {
    vkDestroyFramebuffer(logical_device_, swap_chain_framebuffers_[i], nullptr);
  }

  // Command buffer
  vkFreeCommandBuffers(logical_device_, command_pool_,
    static_cast<uint32_t>(command_buffers_.size()), command_buffers_.data());

  
  // Pipelines and all related to them
  for (int i = 0; i < materials_.size(); i++) {
    vkDestroyPipeline(logical_device_, materials_[i]->graphics_pipeline_, nullptr);
    vkDestroyPipelineLayout(logical_device_, materials_[i]->pipeline_layout_, nullptr);
  }
  vkDestroyDescriptorPool(logical_device_, scene_descriptor_pool_, nullptr);
  vkDestroyDescriptorPool(logical_device_, models_descriptor_pool_, nullptr);
  vkDestroyDescriptorPool(logical_device_, opaque_descriptor_pool_, nullptr);

  // Render pass
  vkDestroyRenderPass(logical_device_, render_pass_, nullptr);

  // Buffers
	cleanUniformBuffers(scene_uniform_buffers_);
	scene_descriptor_sets_.clear();

	cleanUniformBuffers(models_uniform_buffers_);
	models_descriptor_sets_.clear();
	
  cleanUniformBuffers(opaque_uniform_buffers_);
	opaque_descriptor_sets_.clear();

  // Swap chain image views
  for (int i = 0; i < swap_chain_images_.size(); i++) {
    vkDestroyImageView(logical_device_, swap_chain_images_[i]->image_view_, nullptr);
  }

  // Swap chain
  vkDestroySwapchainKHR(logical_device_, swap_chain_, nullptr);

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::framebufferResizeCallback(GLFWwindow* window, int width, int height) {

  auto app = reinterpret_cast<AppData*>(glfwGetWindowUserPointer(window));
  app->resized_framebuffer_ = true;
  app->window_width_ = width;
  app->window_height_ = height;

  if (width != 0) {
    ParticleEditor::instance().getCamera()->setupProjection(90.0f, (float)app->window_width_ / (float)app->window_height_, 0.1f, 10.0f);
  }

}

// ------------------------------------------------------------------------- //

bool ParticleEditor::AppData::isDeviceSuitable(VkPhysicalDevice device) {

  // Basic device properties
  VkPhysicalDeviceProperties device_properties;
  vkGetPhysicalDeviceProperties(device, &device_properties);

  // Tex compression, 64-bit floats and multi-viewport rendering
  VkPhysicalDeviceFeatures device_features;
  vkGetPhysicalDeviceFeatures(device, &device_features);

  // Return false if a requested feature is not supported
  if (!device_features.samplerAnisotropy) {
    return false;
  }

  // Check if device supports graphic and present queues
  QueueFamilyIndices indices = findQueueFamilies(device);

  // Check if the required extensions are supported
  bool extensions_supported = checkDeviceExtensionSupport(device);

  // Check if the swap chain details are adequate on the device
  bool adequate_swap_chain = false;
  if (extensions_supported) {
    SwapChainSupportDetails details = querySwapChainSupportDetails(device);
    adequate_swap_chain = !details.formats.empty() && !details.present_modes.empty();
  }

  // Expand this function with necessary features

  return indices.isComplete() && extensions_supported && adequate_swap_chain;

}

// ------------------------------------------------------------------------- //

bool ParticleEditor::AppData::hasStencilComponent(VkFormat format) {

  return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;

}

// ------------------------------------------------------------------------- //

SwapChainSupportDetails ParticleEditor::AppData::querySwapChainSupportDetails(VkPhysicalDevice device) {

  SwapChainSupportDetails details;

  // Request device supported capabilities
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface_, &details.capabilities);

  // Request device supported formats
  uint32_t formats_count;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formats_count, nullptr);
  if (formats_count != 0) {
    details.formats.resize(formats_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formats_count, details.formats.data());
  }

  // Request device supported present modes
  uint32_t present_modes_count;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &present_modes_count, nullptr);
  if (present_modes_count != 0) {
    details.present_modes.resize(present_modes_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &present_modes_count, details.present_modes.data());
  }

  return details;

}

// ------------------------------------------------------------------------- //

VkSurfaceFormatKHR ParticleEditor::AppData::chooseSwapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats) {

  // Search if any of the available formats has the desired features
  for (int i = 0; i < available_formats.size(); i++) {
    if (available_formats[i].format == VK_FORMAT_B8G8R8A8_SRGB &&
      available_formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {

      return available_formats[i];

    }
  }

  // If not fits we could tank the available formats to choose the better one
  // Just pick the first one in this case
  return available_formats[0];

}

// ------------------------------------------------------------------------- //

VkPresentModeKHR ParticleEditor::AppData::chooseSwapChainPresentMode(const std::vector<VkPresentModeKHR>& available_present_modes) {

  // Search if any of the available present modes are mailbox (triple buffering support)
  for (int i = 0; i < available_present_modes.size(); i++) {
    if (available_present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
      return available_present_modes[i];
    }
  }

  // Always available (double buffering)
  return VK_PRESENT_MODE_FIFO_KHR;

}

// ------------------------------------------------------------------------- //

VkExtent2D ParticleEditor::AppData::chooseSwapChainExtent(const VkSurfaceCapabilitiesKHR& capabilities) {

  if (capabilities.currentExtent.width != UINT32_MAX) {
    // Some window managers set this by default, if is not equal to this it can be modified
    return capabilities.currentExtent;
  }
  else {
    // Choose best window resolution fit between the current and min/max image extent
    int width;
    int height;
    glfwGetFramebufferSize(window_, &width, &height);

    VkExtent2D window_extent = {
      static_cast<uint32_t>(width),
      static_cast<uint32_t>(height)
    };

    window_extent.width = std::max(capabilities.minImageExtent.width,
      std::min(capabilities.maxImageExtent.width, window_extent.width));
    window_extent.height = std::max(capabilities.minImageExtent.height,
      std::min(capabilities.maxImageExtent.height, window_extent.height));

    return window_extent;
  }

}

// ------------------------------------------------------------------------- //

std::vector<const char*> ParticleEditor::AppData::getRequiredExtensions() {

  // Get supported extensions
  uint32_t glfw_extension_count = 0;
  const char** glfw_extensions;
  glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

  std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

  // Be sure to add EXT_DEBUG_UTILS if validation layers are enabled
  if (kEnableValidationLayers) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  return extensions;

}

// ------------------------------------------------------------------------- //

bool ParticleEditor::AppData::checkValidationLayerSupport() {

  // Count available layers
  uint32_t layers_count = 0;
  vkEnumerateInstanceLayerProperties(&layers_count, nullptr);

  // Request available layers data
  std::vector<VkLayerProperties> layer_properties(layers_count);
  vkEnumerateInstanceLayerProperties(&layers_count, layer_properties.data());

  // Iterate over requested layers and check if they are available
  for (uint32_t i = 0; i < kVkValidationLayers.size(); ++i) {
    bool found_layer = false;

    for (uint32_t l = 0; l < layers_count; ++l) {
      if (strcmp(kVkValidationLayers[i], layer_properties[l].layerName) == 0) {
        found_layer = true;
        break;
      }
    }

    if (!found_layer) {
      return false;
    }
  }

  return true;

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::setupDebugMessenger() {

  if (!kEnableValidationLayers) return;

  VkDebugUtilsMessengerCreateInfoEXT messenger_info;
  populateDebugMessengerCreateInfo(messenger_info);

  if (CreateDebugUtilsMessengerEXT(instance_, &messenger_info, nullptr, &debug_messenger_) != VK_SUCCESS) {
    throw std::runtime_error("Debug Messenger creation has failed.");
  }

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& create_info) {

  create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  create_info.pfnUserCallback = debugCallback;
  create_info.pUserData = nullptr;

}

// ------------------------------------------------------------------------- //

VKAPI_ATTR VkBool32 VKAPI_CALL ParticleEditor::AppData::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {

  // Not finished, it could show the information of other lot of things
  // it could even show it trough a log in the app, which can be divided in different message sections

  if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
    //printf("\nValidation layer (VERBOSE): %s.", pCallbackData->pMessage);
  }
  if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
    //printf("\nValidation layer (INFO): %s.", pCallbackData->pMessage);
  }
  if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
    printf("\nValidation layer (WARNING): %s.", pCallbackData->pMessage);
  }
  if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
    printf("\nValidation layer (ERROR): %s.", pCallbackData->pMessage);
  }

  return VK_FALSE;

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout) {

  VkCommandBuffer cmd_buffer = beginSingleTimeCommands(logical_device_, command_pool_);

  VkImageMemoryBarrier barrier{};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.oldLayout = old_layout;
  barrier.newLayout = new_layout;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = image;
  barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = 1;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;

  VkPipelineStageFlags source_stage;
  VkPipelineStageFlags destination_stage;

  if (new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    if (hasStencilComponent(format)) {
      barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }
  }
  else {
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  }


  if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  }
  else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  }
  else if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
      VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destination_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  }
  else {
    throw std::runtime_error("Unsupported layout transition.");
  }

  vkCmdPipelineBarrier(cmd_buffer, source_stage, destination_stage,
    0, 0, nullptr,
    0, nullptr, 1, &barrier);

  endSingleTimeCommands(logical_device_, command_pool_, cmd_buffer, graphics_queue_);

}

// ------------------------------------------------------------------------- //

uint32_t ParticleEditor::AppData::findMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties) {

  // Request supported memory properties from the graphics card
  VkPhysicalDeviceMemoryProperties mem_properties;
  vkGetPhysicalDeviceMemoryProperties(physical_device_, &mem_properties);

  // Find a suitable memory type (Mask memory type bit and property flags)
  for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++) {
    if (type_filter & (1 << i) &&
      (mem_properties.memoryTypes[i].propertyFlags & properties) == properties) {
      return i;
    }
  }

  throw std::runtime_error("Failed to find a suitable memory type.");

}

// ------------------------------------------------------------------------- //

VkFormat ParticleEditor::AppData::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {

  for (uint32_t i = 0; i < candidates.size(); i++) {
    VkFormatProperties properties;
    vkGetPhysicalDeviceFormatProperties(physical_device_, candidates[i], &properties);

    if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features) {
      return candidates[i];
    }
    else if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features) {
      return candidates[i];
    }
  }

  throw std::runtime_error("Failed to find a supported a format.");

}

// ------------------------------------------------------------------------- //

bool ParticleEditor::AppData::checkDeviceExtensionSupport(VkPhysicalDevice device) {

  // Get supported device properties
  uint32_t extension_count;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);

  std::vector<VkExtensionProperties> available_extensions(extension_count);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());

  std::set<std::string> required_extensions(kDeviceExtensions.begin(), kDeviceExtensions.end());

  // Delete the extension from the required extension vector if they are available
  for (uint32_t i = 0; i < available_extensions.size(); i++) {
    required_extensions.erase(available_extensions[i].extensionName);
  }

  // If the required extensions vector was emptied it means that they are supported
  return required_extensions.empty();

}

// ------------------------------------------------------------------------- //

QueueFamilyIndices ParticleEditor::AppData::findQueueFamilies(VkPhysicalDevice device) {

  QueueFamilyIndices indices;

  // Get device family queue properties
  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

  std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

  for (uint32_t i = 0; i < queue_families.size(); i++) {
    // Check if graphics queue is supported
    if (queue_families[i].queueFlags && VK_QUEUE_GRAPHICS_BIT) {
      indices.graphics_family = i;
    }

    // Check if present queue is supported
    VkBool32 present_support = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface_, &present_support);
    if (present_support) {
      indices.present_family = i;
    }

    // Break if both are found
    if (indices.isComplete()) {
      break;
    }
  }

  return indices;

}

// ------------------------------------------------------------------------- //

VkFormat ParticleEditor::AppData::findDepthFormat() {

  return findSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
    VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

}

// ------------------------------------------------------------------------- //

VkSampleCountFlagBits ParticleEditor::AppData::getMaxUsableSampleCount() {

	VkPhysicalDeviceProperties phys_device_properties;
	vkGetPhysicalDeviceProperties(physical_device_, &phys_device_properties);

	VkSampleCountFlags counts = phys_device_properties.limits.framebufferColorSampleCounts &
		phys_device_properties.limits.framebufferDepthSampleCounts;

	if (!ENABLE_MSAA) return VK_SAMPLE_COUNT_1_BIT;

	if (counts & VK_SAMPLE_COUNT_64_BIT) return VK_SAMPLE_COUNT_64_BIT;
	if (counts & VK_SAMPLE_COUNT_32_BIT) return VK_SAMPLE_COUNT_32_BIT;
	if (counts & VK_SAMPLE_COUNT_16_BIT) return VK_SAMPLE_COUNT_16_BIT;
	if (counts & VK_SAMPLE_COUNT_8_BIT)  return VK_SAMPLE_COUNT_8_BIT;
	if (counts & VK_SAMPLE_COUNT_4_BIT)  return VK_SAMPLE_COUNT_4_BIT;
	if (counts & VK_SAMPLE_COUNT_2_BIT)  return VK_SAMPLE_COUNT_2_BIT;

	return VK_SAMPLE_COUNT_1_BIT;

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::createUniformBuffers(int size, std::vector<Buffer*>& buffers_) {

	if (window_width_ == 0 || window_height_ == 0) return;

	buffers_.resize(swap_chain_images_.size());

	VkDeviceSize buffer_size = size;

	for (int i = 0; i < swap_chain_images_.size(); i++) {
		buffers_[i] = new Buffer(Buffer::kBufferType_Uniform);
		buffers_[i]->create(physical_device_, logical_device_, buffer_size,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	}

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::createDynamicUniformBuffers(std::vector<Buffer*>& buffers_){

	if (window_width_ == 0 || window_height_ == 0) return;

	buffers_.resize(swap_chain_images_.size());

  // Return false if a requested feature is not supported
	VkPhysicalDeviceProperties device_properties;
	vkGetPhysicalDeviceProperties(physical_device_, &device_properties);

  // Calculate required alignment  for the ubo based on minimum device offset alignment
	size_t min_ubo_alignment = device_properties.limits.minUniformBufferOffsetAlignment;
  system_draw_objects_->dynamic_alignment_ = sizeof(glm::mat4);
	if (min_ubo_alignment > 0) {
    system_draw_objects_->dynamic_alignment_ = (system_draw_objects_->dynamic_alignment_ +
      min_ubo_alignment - 1) & ~(min_ubo_alignment - 1);
	}

  // Number of 3D objects * dynamic alignment
	size_t buffer_size = system_draw_objects_->getNumberOfObjects(ParticleEditor::instance().getScene()->getEntities()) *
    system_draw_objects_->dynamic_alignment_;

  // Allocate the memory for the ubo
  models_ubo_.models = (glm::mat4*)alignedAlloc(buffer_size, 
    system_draw_objects_->dynamic_alignment_);
  if (models_ubo_.models == nullptr) {
    throw std::runtime_error("\nFailed to allocate dynamic uniform buffer.");
  }

	// Uniform buffer object with per-object matrices
	for (int i = 0; i < swap_chain_images_.size(); i++) {
		buffers_[i] = new Buffer(Buffer::kBufferType_Uniform);
		buffers_[i]->create(physical_device_, logical_device_, buffer_size,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	}

  // Update for the first frame
	/*system_draw_objects_->updateUniformBuffers(current_frame_,
		BasicPSApp::instance().getScene()->getEntities());*/

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::createOpaqueDynamicUniformBuffers(std::vector<Buffer*>& buffers_){

	if (window_width_ == 0 || window_height_ == 0) return;

	buffers_.resize(swap_chain_images_.size());

	// Return false if a requested feature is not supported
	VkPhysicalDeviceProperties device_properties;
	vkGetPhysicalDeviceProperties(physical_device_, &device_properties);

	// Calculate required alignment  for the ubo based on minimum device offset alignment
	size_t min_ubo_alignment = device_properties.limits.minUniformBufferOffsetAlignment;
	system_draw_objects_->opaque_dynamic_alignment_ = sizeof(glm::mat4);
	if (min_ubo_alignment > 0) {
		system_draw_objects_->opaque_dynamic_alignment_ = (system_draw_objects_->opaque_dynamic_alignment_ +
			min_ubo_alignment - 1) & ~(min_ubo_alignment - 1);
	}

	// Number of 3D objects * dynamic alignment
	size_t buffer_size = system_draw_objects_->getNumberOfObjects(ParticleEditor::instance().getScene()->getEntities()) *
		system_draw_objects_->opaque_dynamic_alignment_;

	// Allocate the memory for the ubo (First half of the ubo)
	opaque_ubo_.packed_uniforms = (glm::mat4*)alignedAlloc(buffer_size,
		system_draw_objects_->opaque_dynamic_alignment_);
	if (opaque_ubo_.packed_uniforms == nullptr) {
		throw std::runtime_error("\nFailed to allocate color dynamic uniform buffer.");
	}

	// Uniform buffer object with per-object matrices
	for (int i = 0; i < swap_chain_images_.size(); i++) {
		buffers_[i] = new Buffer(Buffer::kBufferType_Uniform);
		buffers_[i]->create(physical_device_, logical_device_, buffer_size,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	}

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::updateUniformBuffer(SceneUBO ubo, Buffer* buffer) {

	void* data;
	vkMapMemory(logical_device_, buffer->buffer_memory_, 0,
		sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(logical_device_, buffer->buffer_memory_);

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::updateUniformBuffer(ModelsUBO ubo, int objects, Buffer* buffer) {

  void* data;
  uint32_t size = objects * system_draw_objects_->dynamic_alignment_;
  vkMapMemory(logical_device_, buffer->buffer_memory_, 0, size, 0, &data);

	for (int i = 0; i < objects; ++i) {
		// Do the dynamic offset things
		uint32_t offset = (i * system_draw_objects_->dynamic_alignment_);
    glm::mat4* mem_offset = (glm::mat4*)(((uint64_t)data + offset));
		glm::mat4* model = (glm::mat4*)(((uint64_t)ubo.models + offset));
		memcpy((void*)mem_offset, *(&model), system_draw_objects_->dynamic_alignment_);
	}

  // Flush to make changes visible to the host if visible host flag is not set
  /*VkMappedMemoryRange memoryRange = vks::initializers::mappedMemoryRange();
  memoryRange.memory = uniformBuffers.dynamic.memory;
  memoryRange.size = uniformBuffers.dynamic.size;
  vkFlushMappedMemoryRanges(device, 1, &memoryRange);*/

  vkUnmapMemory(logical_device_, buffer->buffer_memory_);

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::updateUniformBuffer(OpaqueUBO ubo, int objects, Buffer* buffer){

	void* data;
  uint32_t size = objects * system_draw_objects_->opaque_dynamic_alignment_;
	vkMapMemory(logical_device_, buffer->buffer_memory_, 0, size, 0, &data);

  for (int i = 0; i < objects; ++i) {
    // Do the dynamic offset things
    uint64_t offset = (i * system_draw_objects_->opaque_dynamic_alignment_);
    glm::mat4* mem_offset = (glm::mat4*)(((uint64_t)data + offset));
    glm::mat4* packed_uniform = (glm::mat4*)(((uint64_t)ubo.packed_uniforms + offset));
	  memcpy((void*)mem_offset, *(&packed_uniform), system_draw_objects_->opaque_dynamic_alignment_);
  }


	vkUnmapMemory(logical_device_, buffer->buffer_memory_);

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::cleanUniformBuffers(std::vector<Buffer*>& buffers_){

	for (int i = 0; i < swap_chain_images_.size(); i++) {
    buffers_[i]->clean(logical_device_);
		delete buffers_[i];
	}

  buffers_.clear();

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::populateSceneDescriptorSets() {

	std::vector<VkDescriptorSetLayout> descriptor_set_layouts(swap_chain_images_.size(), scene_descriptor_set_layout_);
	VkDescriptorSetAllocateInfo allocate_info{};
	allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocate_info.descriptorPool = scene_descriptor_pool_;
	allocate_info.descriptorSetCount = static_cast<uint32_t>(swap_chain_images_.size());
	allocate_info.pSetLayouts = descriptor_set_layouts.data();

	scene_descriptor_sets_.resize(swap_chain_images_.size());
	if (vkAllocateDescriptorSets(logical_device_, &allocate_info, scene_descriptor_sets_.data()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create scene descriptor sets.");
	}


	for (int i = 0; i < scene_descriptor_sets_.size(); i++) {
		VkDescriptorBufferInfo buffer_info{};
		buffer_info.buffer = scene_uniform_buffers_[i]->buffer_;
		buffer_info.offset = 0;
		buffer_info.range = sizeof(SceneUBO);

		VkWriteDescriptorSet write_descriptor{};
		write_descriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write_descriptor.dstSet = scene_descriptor_sets_[i];
		write_descriptor.dstBinding = 0;
		write_descriptor.dstArrayElement = 0;
		write_descriptor.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		write_descriptor.descriptorCount = 1;
		write_descriptor.pBufferInfo = &buffer_info;
		write_descriptor.pImageInfo = nullptr; // Image data
		write_descriptor.pTexelBufferView = nullptr; // Buffer views

		vkUpdateDescriptorSets(logical_device_, 1, &write_descriptor, 0, nullptr);
	}

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::populateModelsDescriptorSets() {

	std::vector<VkDescriptorSetLayout> descriptor_set_layouts(swap_chain_images_.size(), models_descriptor_set_layout_);
	VkDescriptorSetAllocateInfo allocate_info{};
	allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocate_info.descriptorPool = models_descriptor_pool_;
	allocate_info.descriptorSetCount = static_cast<uint32_t>(swap_chain_images_.size());
	allocate_info.pSetLayouts = descriptor_set_layouts.data();

	models_descriptor_sets_.resize(swap_chain_images_.size());
	if (vkAllocateDescriptorSets(logical_device_, &allocate_info, models_descriptor_sets_.data()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create models descriptor sets.");
	}


	for (int i = 0; i < models_descriptor_sets_.size(); i++) {
		VkDescriptorBufferInfo buffer_info{};
		buffer_info.buffer = models_uniform_buffers_[i]->buffer_;
		buffer_info.offset = 0;
    buffer_info.range = system_draw_objects_->dynamic_alignment_;

		VkWriteDescriptorSet write_descriptor{};
		write_descriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write_descriptor.dstSet = models_descriptor_sets_[i];
		write_descriptor.dstBinding = 0;
		write_descriptor.dstArrayElement = 0;
		write_descriptor.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		write_descriptor.descriptorCount = 1;
		write_descriptor.pBufferInfo = &buffer_info;
		write_descriptor.pImageInfo = nullptr; // Image data
		write_descriptor.pTexelBufferView = nullptr; // Buffer views

		vkUpdateDescriptorSets(logical_device_, 1, &write_descriptor, 0, nullptr);
	}

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::populateOpaqueDescriptorSets() {

	std::vector<VkDescriptorSetLayout> descriptor_set_layouts(swap_chain_images_.size(), opaque_descriptor_set_layout_);
	VkDescriptorSetAllocateInfo allocate_info{};
	allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocate_info.descriptorPool = opaque_descriptor_pool_;
	allocate_info.descriptorSetCount = static_cast<uint32_t>(swap_chain_images_.size());
	allocate_info.pSetLayouts = descriptor_set_layouts.data();

	opaque_descriptor_sets_.resize(swap_chain_images_.size());
	if (vkAllocateDescriptorSets(logical_device_, &allocate_info, opaque_descriptor_sets_.data()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create opaque descriptor sets.");
	}


	for (int i = 0; i < opaque_descriptor_sets_.size(); i++) {
		VkDescriptorBufferInfo buffer_info{};
		buffer_info.buffer = opaque_uniform_buffers_[i]->buffer_;
		buffer_info.offset = 0;
		buffer_info.range = system_draw_objects_->opaque_dynamic_alignment_;

    const int num_textures = loaded_textures_.size();
    std::vector<VkDescriptorImageInfo> image_info = std::vector<VkDescriptorImageInfo>(num_textures);
    for (int j = 0; j < num_textures; ++j) {
      image_info[j] = {};
			image_info[j].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			image_info[j].imageView = ParticleEditor::instance().app_data_->
				texture_images_[j]->image_view_;
			image_info[j].sampler = ParticleEditor::instance().app_data_->
				texture_images_[j]->texture_sampler_;
    }

    std::array<VkWriteDescriptorSet, 2> write_descriptors{};
		write_descriptors[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write_descriptors[0].dstSet = opaque_descriptor_sets_[i];
		write_descriptors[0].dstBinding = 0;
		write_descriptors[0].dstArrayElement = 0;
		write_descriptors[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		write_descriptors[0].descriptorCount = 1;
		write_descriptors[0].pBufferInfo = &buffer_info;
		write_descriptors[0].pImageInfo = nullptr; // Image data
		write_descriptors[0].pTexelBufferView = nullptr; // Buffer views

		write_descriptors[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write_descriptors[1].dstSet = opaque_descriptor_sets_[i];
		write_descriptors[1].dstBinding = 1;
		write_descriptors[1].dstArrayElement = 0;
		write_descriptors[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		write_descriptors[1].descriptorCount = num_textures;
		write_descriptors[1].pBufferInfo = nullptr; // Buffer data
		write_descriptors[1].pImageInfo = image_info.data();
		write_descriptors[1].pTexelBufferView = nullptr; // Buffer views

		vkUpdateDescriptorSets(logical_device_, static_cast<uint32_t>(write_descriptors.size()),
			write_descriptors.data(), 0, nullptr);
	}

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::setupVertexBuffers(){

	std::vector<Vertex> quad_vertices;
  // Quad
  quad_vertices = {
		{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
		{{ 0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
		{{ 0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
		{{-0.5f,  0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
	};

  createVertexBuffer(quad_vertices);


  // Cube
	//vertices = {
    //{{-0.5f, ... };
	//createVertexBuffer(vertices);


}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::setupIndexBuffers(){

	std::vector<uint32_t> quad_indices;
  // Quad
	quad_indices = { 0, 1, 2, 2, 3, 0 };
  
  createIndexBuffer(quad_indices);

	// Cube
	//indices = {
		//{{-0.5f, ... };
	//createIndexBuffer(indices);

}

// ------------------------------------------------------------------------- //

void ParticleEditor::AppData::setupMaterials(){

  Material* opaque_material = new Material();
  opaque_material->material_id_ = 0;

  materials_.push_back(opaque_material);
  
  
  Material* translucent_material = new Material();
  translucent_material->material_id_ = 1;

  materials_.push_back(translucent_material);

}

// ------------------------------------------------------------------------- //
