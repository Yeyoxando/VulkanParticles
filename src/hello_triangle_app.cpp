/*
 *	Author: Diego Ochando Torres
 *  Date: 25/11/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

// ------------------------------------------------------------------------- // 

#include "hello_triangle_app.h"
#include "vulkan_utils.h"

#include <stdexcept>
#include <cstring>
#include <vector>
#include <set>

// ------------------------------------------------------------------------- // 

HelloTriangleApp::HelloTriangleApp() {

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

  current_frame_ = 0;

}

// ------------------------------------------------------------------------- // 

HelloTriangleApp::~HelloTriangleApp() {

}

// ------------------------------------------------------------------------- // 

void HelloTriangleApp::run() {

  initWindow();

  initVulkan();
  renderLoop();
  close();

}

// ------------------------------------------------------------------------- // 

void HelloTriangleApp::initWindow(int width /*= 800*/, int height /*= 600*/) {

  glfwInit();

  // Tell GLFW to not use OpenGL API
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  // No resize allowed for now
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  window_ = glfwCreateWindow(width, height, "Hello Triangle", nullptr, nullptr);
  window_width_ = width;
  window_height_ = height;

}

// ------------------------------------------------------------------------- // 

void HelloTriangleApp::initVulkan() {

  createInstance();
  setupDebugMessenger();
  createSurface();
  pickPhysicalDevice();
  createLogicalDevice();
  createSwapChain();
  createImageViews();
  createRenderPass();
  createGraphicsPipeline();
  createFramebuffers();
  createCommandPool();
  createCommandBuffers();
  createSyncObjects();

}

// ------------------------------------------------------------------------- // 

void HelloTriangleApp::renderLoop() {

  while (!glfwWindowShouldClose(window_)) {
    
    glfwPollEvents();

    drawFrame(); // All praise the triangle! Hallelujah!!!

  }

  // Wait until the device is on idle so we can clean up resources without being in use
  vkDeviceWaitIdle(logical_device_);

}

// ------------------------------------------------------------------------- // 

void HelloTriangleApp::close() {

  // Vulkan cleanup (reverse creation order)
  for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkDestroySemaphore(logical_device_, available_image_semaphores_[i], nullptr);
    vkDestroySemaphore(logical_device_, finished_render_semaphores_[i], nullptr);
    vkDestroyFence(logical_device_, in_flight_fences_[i], nullptr);
  }

  vkDestroyCommandPool(logical_device_, command_pool_, nullptr);

  for (int i = 0; i < swap_chain_framebuffers_.size(); i++) {
    vkDestroyFramebuffer(logical_device_, swap_chain_framebuffers_[i], nullptr);
  }

  vkDestroyPipeline(logical_device_, graphics_pipeline_, nullptr);

  vkDestroyPipelineLayout(logical_device_, pipeline_layout_, nullptr);

  vkDestroyRenderPass(logical_device_, render_pass_, nullptr);

  for (int i = 0; i < swap_chain_image_views_.size(); i++) {
    vkDestroyImageView(logical_device_, swap_chain_image_views_[i], nullptr);
  }

  vkDestroySwapchainKHR(logical_device_, swap_chain_, nullptr);

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

void HelloTriangleApp::createInstance() {

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
    create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debug_create_info;
  }
  else {
    create_info.enabledLayerCount = 0;
  }

  // Create the instance
  if (vkCreateInstance(&create_info, nullptr, &instance_) != VK_SUCCESS) {
    throw std::runtime_error("Vulkan instance creation failed.");
  }

  // Report available vulkan extensions
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

std::vector<const char*> HelloTriangleApp::getRequiredExtensions() {

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

bool HelloTriangleApp::checkValidationLayerSupport() {

  // Count available layers
  uint32_t layers_count = 0;
  vkEnumerateInstanceLayerProperties(&layers_count, nullptr);

  // Request available layers data
  std::vector<VkLayerProperties> layer_properties(layers_count);
  vkEnumerateInstanceLayerProperties(&layers_count, layer_properties.data());

  // Iterate over requested layers and check if they are available
  for (uint32_t i = 0; i < kVkValidationLayers.size(); ++i){
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

void HelloTriangleApp::setupDebugMessenger() {

  if (!kEnableValidationLayers) return;

  VkDebugUtilsMessengerCreateInfoEXT messenger_info;
  populateDebugMessengerCreateInfo(messenger_info);

  if (CreateDebugUtilsMessengerEXT(instance_, &messenger_info, nullptr, &debug_messenger_) != VK_SUCCESS) {
    throw std::runtime_error("Debug Messenger creation has failed.");
  }

}

// ------------------------------------------------------------------------- // 

void HelloTriangleApp::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& create_info) {
  
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

VKAPI_ATTR VkBool32 VKAPI_CALL HelloTriangleApp::debugCallback(
  VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, 
  VkDebugUtilsMessageTypeFlagsEXT messageType, 
  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, 
  void* pUserData) {

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

void HelloTriangleApp::createSurface() {

  if (glfwCreateWindowSurface(instance_, window_, nullptr, &surface_) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create window surface.");
  }

}

// ------------------------------------------------------------------------- // 

void HelloTriangleApp::pickPhysicalDevice() {

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
      break;
    }
  }

  if (physical_device_ == VK_NULL_HANDLE) {
    throw std::runtime_error("The device's GPUs are not suitable for the requested functionality.");
  }

}

// ------------------------------------------------------------------------- // 

bool HelloTriangleApp::isDeviceSuitable(VkPhysicalDevice device) {

  // Basic device properties
  VkPhysicalDeviceProperties device_properties;
  vkGetPhysicalDeviceProperties(device, &device_properties);

  // Tex compression, 64-bit floats and multi-viewport rendering
  VkPhysicalDeviceFeatures device_features;
  vkGetPhysicalDeviceFeatures(device, &device_features);

  // Example if card don't support geometry shader
  /*if (!device_features.geometryShader) {
    return false;
  }*/

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

bool HelloTriangleApp::checkDeviceExtensionSupport(VkPhysicalDevice device) {

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

QueueFamilyIndices HelloTriangleApp::findQueueFamilies(VkPhysicalDevice device) {
  
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

SwapChainSupportDetails HelloTriangleApp::querySwapChainSupportDetails(VkPhysicalDevice device) {

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

VkSurfaceFormatKHR HelloTriangleApp::chooseSwapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats) {

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

VkPresentModeKHR HelloTriangleApp::chooseSwapChainPresentMode(const std::vector<VkPresentModeKHR>& available_present_modes) {

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

VkExtent2D HelloTriangleApp::chooseSwapChainExtent(const VkSurfaceCapabilitiesKHR& capabilities) {

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

void HelloTriangleApp::createLogicalDevice() {

  // Create needed queues
  QueueFamilyIndices indices = findQueueFamilies(physical_device_);
  float queue_priority = 1.0f;

  // Store unique queues
  std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
  std::set<uint32_t> unique_queue_families = {indices.graphics_family.value(), 
    indices.present_family.value()};

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
  //device_features.geometryShader = true; // For example

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

void HelloTriangleApp::createSwapChain() {

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
  else{
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
  vkGetSwapchainImagesKHR(logical_device_, swap_chain_, &image_count, swap_chain_images_.data());

  // Store format and extent for future operations
  swap_chain_image_format_ = format.format;
  swap_chain_extent_ = extent;

}

// ------------------------------------------------------------------------- // 

void HelloTriangleApp::createImageViews() {

  swap_chain_image_views_.resize(swap_chain_images_.size());

  for (int i = 0; i < swap_chain_images_.size(); i++) {
    VkImageViewCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    create_info.image = swap_chain_images_[i];
    create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    create_info.format = swap_chain_image_format_;
    
    // Remapping components (Set to default)
    create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    
    // Image usage (color bit for those)
    create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    create_info.subresourceRange.baseMipLevel = 0;
    create_info.subresourceRange.levelCount = 1;
    create_info.subresourceRange.baseArrayLayer = 0;
    create_info.subresourceRange.layerCount = 1;

    if (vkCreateImageView(logical_device_, &create_info, nullptr, &swap_chain_image_views_[i]) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create image views.");
    }

  }

}

// ------------------------------------------------------------------------- // 

void HelloTriangleApp::createRenderPass() {

  // How to load and store the received info
  VkAttachmentDescription color_attachment{};
  color_attachment.format = swap_chain_image_format_;
  color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
  // Color and depth data
  color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  // Stencil data
  color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  // Subpasses (only 1 for now)
  VkAttachmentReference color_attachment_ref{};
  color_attachment_ref.attachment = 0;
  color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &color_attachment_ref; // layout location = 0 in frag shader

  // Dependency for the render pass to start (Until color attachment is finished)
  VkSubpassDependency subpass_dependency{};
  subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  subpass_dependency.dstSubpass = 0;
  subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  subpass_dependency.srcAccessMask = 0;
  subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  subpass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  // Create the render pass
  VkRenderPassCreateInfo render_pass_info{};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_info.attachmentCount = 1;
  render_pass_info.pAttachments = &color_attachment;
  render_pass_info.subpassCount = 1;
  render_pass_info.pSubpasses = &subpass;
  render_pass_info.dependencyCount = 1;
  render_pass_info.pDependencies = &subpass_dependency;

  if (vkCreateRenderPass(logical_device_, &render_pass_info, nullptr, &render_pass_) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create the render pass");
  }

}

// ------------------------------------------------------------------------- // 

void HelloTriangleApp::createGraphicsPipeline() {

  // Different pipelines has to be created for different render options
  // Not like in OpenGL where you can change glBlendFunc between objects
  // Whole pipeline has to be created and changed to do this

  // Load shaders
  auto vert_shader_code = readFile("../../../resources/shaders_spirv/v_hello_triangle.spv");
  auto frag_shader_code = readFile("../../../resources/shaders_spirv/f_hello_triangle.spv");

  VkShaderModule vert_shader_module = createShaderModule(vert_shader_code);
  VkShaderModule frag_shader_module = createShaderModule(frag_shader_code);

  VkPipelineShaderStageCreateInfo vert_shader_stage_info{};
  vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vert_shader_stage_info.module = vert_shader_module;
  vert_shader_stage_info.pName = "main";
  vert_shader_stage_info.pSpecializationInfo = nullptr; // Constants can be defined here

  VkPipelineShaderStageCreateInfo frag_shader_stage_info{};
  frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  frag_shader_stage_info.module = frag_shader_module;
  frag_shader_stage_info.pName = "main";
  frag_shader_stage_info.pSpecializationInfo = nullptr; // Constants can be defined here

  VkPipelineShaderStageCreateInfo shader_stages[] = { vert_shader_stage_info, frag_shader_stage_info };

  // Set vertex input format (Empty for now)
  VkPipelineVertexInputStateCreateInfo vertex_input_info{};
  vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertex_input_info.vertexBindingDescriptionCount = 0;
  vertex_input_info.pVertexBindingDescriptions = nullptr;
  vertex_input_info.vertexAttributeDescriptionCount = 0;
  vertex_input_info.pVertexAttributeDescriptions = nullptr;

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
  rasterizer_state_info.polygonMode = VK_POLYGON_MODE_FILL; // Line to draw wireframe but require a GPU feature
  rasterizer_state_info.lineWidth = 1.0f;
  rasterizer_state_info.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer_state_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizer_state_info.depthBiasEnable = VK_FALSE;
  rasterizer_state_info.depthBiasClamp = 0.0f;
  rasterizer_state_info.depthBiasConstantFactor = 0.0f;
  rasterizer_state_info.depthBiasSlopeFactor = 0.0f;

  // Create multi sampling settings
  VkPipelineMultisampleStateCreateInfo multisample_state_info{};
  multisample_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisample_state_info.sampleShadingEnable = VK_FALSE;
  multisample_state_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisample_state_info.minSampleShading = 1.0f;
  multisample_state_info.pSampleMask = nullptr;
  multisample_state_info.alphaToCoverageEnable = VK_FALSE;
  multisample_state_info.alphaToOneEnable = VK_FALSE;

  // Create depth and stencil settings for the framebuffer (Null for now)


  // Create color blend settings
  VkPipelineColorBlendAttachmentState color_blend_attachment{};
  color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
    VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  color_blend_attachment.blendEnable = VK_FALSE;
  color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
  color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
  // Alpha blending
  //color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  //color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
  color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

  VkPipelineColorBlendStateCreateInfo blend_state_info{};
  blend_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  blend_state_info.logicOpEnable = VK_FALSE;
  blend_state_info.logicOp = VK_LOGIC_OP_COPY;
  blend_state_info.attachmentCount = 1;
  blend_state_info.pAttachments = &color_blend_attachment;
  blend_state_info.blendConstants[0] = 0.0f;
  blend_state_info.blendConstants[1] = 0.0f;
  blend_state_info.blendConstants[2] = 0.0f;
  blend_state_info.blendConstants[3] = 0.0f;

  // Create dynamic settings of the pipeline
  VkDynamicState dynamic_states[] = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_LINE_WIDTH,
  };

  VkPipelineDynamicStateCreateInfo dynamic_state_info{};
  dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamic_state_info.dynamicStateCount = 2;
  dynamic_state_info.pDynamicStates = dynamic_states;

  // Create pipeline state layout (Uniforms)
  VkPipelineLayoutCreateInfo layout_info{};
  layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  layout_info.setLayoutCount = 0;
  layout_info.pSetLayouts = nullptr;
  layout_info.pushConstantRangeCount = 0;
  layout_info.pPushConstantRanges = nullptr;

  if (vkCreatePipelineLayout(logical_device_, &layout_info, nullptr, &pipeline_layout_) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create pipeline layout.");
  }

  // Create the actual graphics pipeline
  VkGraphicsPipelineCreateInfo graphics_pipeline_info{};
  graphics_pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  graphics_pipeline_info.stageCount = 2;
  graphics_pipeline_info.pStages = shader_stages;
  graphics_pipeline_info.pVertexInputState = &vertex_input_info;
  graphics_pipeline_info.pInputAssemblyState = &input_assembly_info;
  graphics_pipeline_info.pViewportState = &viewport_state_info;
  graphics_pipeline_info.pRasterizationState = &rasterizer_state_info;
  graphics_pipeline_info.pMultisampleState = &multisample_state_info;
  graphics_pipeline_info.pDepthStencilState = nullptr;
  graphics_pipeline_info.pColorBlendState = &blend_state_info;
  graphics_pipeline_info.pDynamicState = nullptr;
  graphics_pipeline_info.layout = pipeline_layout_;
  graphics_pipeline_info.renderPass = render_pass_;
  graphics_pipeline_info.subpass = 0;
  graphics_pipeline_info.basePipelineHandle = VK_NULL_HANDLE; // derive from other graphics pipeline with flags VK_PIPELINE_CREATE_DERIVATIVE_BIT
  graphics_pipeline_info.basePipelineIndex = -1;

  if (vkCreateGraphicsPipelines(logical_device_, VK_NULL_HANDLE, 1, &graphics_pipeline_info,
    nullptr, &graphics_pipeline_) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create the graphics pipeline.");
  }

  vkDestroyShaderModule(logical_device_, vert_shader_module, nullptr);
  vkDestroyShaderModule(logical_device_, frag_shader_module, nullptr);

}

// ------------------------------------------------------------------------- // 

VkShaderModule HelloTriangleApp::createShaderModule(const std::vector<char>& bytecode) {
  
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

void HelloTriangleApp::createFramebuffers() {

  swap_chain_framebuffers_.resize(swap_chain_image_views_.size());

  for (int i = 0; i < swap_chain_image_views_.size(); i++) {
    VkImageView attachments[] = {
      swap_chain_image_views_[i]
    };
  
    VkFramebufferCreateInfo framebuffer_info{};
    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_info.renderPass = render_pass_;
    framebuffer_info.attachmentCount = 1;
    framebuffer_info.pAttachments = attachments;
    framebuffer_info.width = swap_chain_extent_.width;
    framebuffer_info.height = swap_chain_extent_.height;
    framebuffer_info.layers = 1;

    if (vkCreateFramebuffer(logical_device_, &framebuffer_info, nullptr, &swap_chain_framebuffers_[i]) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create framebuffer.");
    }
  }

}

// ------------------------------------------------------------------------- // 

void HelloTriangleApp::createCommandPool() {

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

void HelloTriangleApp::createCommandBuffers() {

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
    VkClearValue clear_color = { 0.0f, 0.0f, 0.0f, 1.0f }; // Black
    render_pass_begin.clearValueCount = 1;
    render_pass_begin.pClearValues = &clear_color;

    // Record the commands on the command buffer
    vkCmdBeginRenderPass(command_buffers_[i], &render_pass_begin, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(command_buffers_[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_);

    vkCmdDraw(command_buffers_[i], 3, 1, 0, 0);

    vkCmdEndRenderPass(command_buffers_[i]);

    if (vkEndCommandBuffer(command_buffers_[i]) != VK_SUCCESS) {
      throw std::runtime_error("Failed to end command buffer recording.");
    }
  }

}

// ------------------------------------------------------------------------- // 

void HelloTriangleApp::createSyncObjects() {

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

void HelloTriangleApp::drawFrame() {

  // Wait until a frame is available
  vkWaitForFences(logical_device_, 1, &in_flight_fences_[current_frame_], VK_TRUE, UINT64_MAX);

  // Acquire an image from the swap chain
  uint32_t image_index;
  vkAcquireNextImageKHR(logical_device_, swap_chain_, UINT64_MAX, available_image_semaphores_[current_frame_],
    VK_NULL_HANDLE, &image_index); // UINT64_MAX stands for no timeout

  // Check if a previous frame is using the image
  if (images_in_flight_[image_index] != VK_NULL_HANDLE) {
    vkWaitForFences(logical_device_, 1, &images_in_flight_[image_index], VK_TRUE, UINT64_MAX);
  }

  // Mark the image as used in this frame
  images_in_flight_[image_index] = in_flight_fences_[current_frame_];


  // Execute the command buffer with that image as attachment in the framebuffer
  VkSubmitInfo submit_info{};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  VkSemaphore wait_semaphores[] = { available_image_semaphores_[current_frame_] };
  VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
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

  vkQueuePresentKHR(graphics_queue_, &present_info);

  current_frame_ = (current_frame_ + 1) % MAX_FRAMES_IN_FLIGHT;

}

// ------------------------------------------------------------------------- // 
