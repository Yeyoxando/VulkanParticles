/*
 *	Author: Diego Ochando Torres
 *  Date: 08/01/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

#ifndef __INTERNAL_GPU_RESOURCES_H__
#define __INTERNAL_GPU_RESOURCES_H__

// ------------------------------------------------------------------------- //

#include "vulkan/vulkan.h"
#include "../src/internal/internal_commands.h"

#include <stdexcept>

// ------------------------------------------------------------------------- //

static uint32_t findMemoryType(VkPhysicalDevice physical_device, uint32_t type_filter, VkMemoryPropertyFlags properties);

class GPUResource {
public:
  GPUResource() { id_ = -1; }
  ~GPUResource() {}

  uint32_t id_;

};

// ------------------------------------------------------------------------- //

class Buffer : public GPUResource {
public:
  enum BufferType {
    kBufferType_Invalid = -1,
    kBufferType_Vertex = 0,
    kBufferType_Index = 1,
    kBufferType_Uniform = 2,
    kBufferType_Image = 3,
  };

  explicit Buffer(BufferType type) : buffer_type_(type) {
    buffer_ = VK_NULL_HANDLE;
    buffer_memory_ = VK_NULL_HANDLE;
  };

  ~Buffer() {}

  // Creates a buffer and allocate its memory
  void create(VkPhysicalDevice phys_device, VkDevice logical_device, VkDeviceSize size, VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties, uint32_t data_count = 0) {

    // Create the  buffer
    VkBufferCreateInfo buffer_info{};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.flags = 0;
    buffer_info.size = size;
    buffer_info.usage = usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(logical_device, &buffer_info, nullptr, &buffer_) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create buffer.");
    }

    // Find memory requirements and allocate memory
    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(logical_device, buffer_, &mem_requirements);

    VkMemoryAllocateInfo allocate_info{};
    allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocate_info.allocationSize = mem_requirements.size;
    allocate_info.memoryTypeIndex = findMemoryType(phys_device, mem_requirements.memoryTypeBits, properties);

    if (vkAllocateMemory(logical_device, &allocate_info, nullptr, &buffer_memory_) != VK_SUCCESS) {
      throw std::runtime_error("Failed to allocate device memory for buffer.");
    }

    // Bind buffer and memory together
    vkBindBufferMemory(logical_device, buffer_, buffer_memory_, 0);
    
    data_count_ = data_count;

  }

  // Copy a buffer from the cpu to the device local memory through a staging buffer
  void copy(VkDevice logical_device, VkCommandPool cmd_pool, VkQueue submit_queue,
    Buffer& src_buffer, VkDeviceSize size) {
    VkCommandBuffer command_buffer = beginSingleTimeCommands(logical_device, cmd_pool);

    VkBufferCopy copy_region{};
    copy_region.srcOffset = 0;
    copy_region.dstOffset = 0;
    copy_region.size = size;
    vkCmdCopyBuffer(command_buffer, src_buffer.buffer_, buffer_, 1, &copy_region);

    endSingleTimeCommands(logical_device, cmd_pool, command_buffer, submit_queue);
  }

  // Clean all the buffer used resources
  void clean(VkDevice logical_device) {
    vkDestroyBuffer(logical_device, buffer_, nullptr);
    vkFreeMemory(logical_device, buffer_memory_, nullptr);
  }


  BufferType buffer_type_;

  VkBuffer buffer_;
  VkDeviceMemory buffer_memory_;

  uint32_t data_count_;

};

// ------------------------------------------------------------------------- //

class Image : public GPUResource {
public:
  enum ImageType {
    kImageType_Invalid = -1,
    kImageType_Framebuffer = 0,
    kImageType_Texture = 1,
  };

  explicit Image(ImageType type) : image_type_(type) {
    image_ = VK_NULL_HANDLE;
    image_memory_ = VK_NULL_HANDLE;
    image_view_ = VK_NULL_HANDLE;
    texture_sampler_ = VK_NULL_HANDLE;
  };

  ~Image() {}

  // Creates a vulkan image
  void create(VkPhysicalDevice phys_device, VkDevice logical_device, uint32_t width, uint32_t height, VkSampleCountFlagBits samples_count, VkFormat format, VkImageTiling tiling,
    VkImageUsageFlags usage, VkMemoryPropertyFlags properties) {

    VkImageCreateInfo image_info{};
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.extent.width = width;
    image_info.extent.height = height;
    image_info.extent.depth = 1;
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1;
    image_info.format = format;
    image_info.tiling = tiling;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.usage = usage;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_info.samples = samples_count;
    image_info.flags = 0;

    if (vkCreateImage(logical_device, &image_info, nullptr, &image_) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create texture image.");
    }

    // Request memory requirements
    VkMemoryRequirements requirements;
    vkGetImageMemoryRequirements(logical_device, image_, &requirements);

    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = requirements.size;
    alloc_info.memoryTypeIndex = findMemoryType(phys_device, requirements.memoryTypeBits, properties);

    if (vkAllocateMemory(logical_device, &alloc_info, nullptr, &image_memory_) != VK_SUCCESS) {
      throw std::runtime_error("Failed to allocate image memory.");
    }

    vkBindImageMemory(logical_device, image_, image_memory_, 0);
  
  }

  // Copies an image from a buffer
  void copyFromBuffer(VkDevice logical_device, VkCommandPool cmd_pool, VkQueue submit_queue, 
    Buffer& src_buffer, uint32_t width, uint32_t height){

    VkCommandBuffer cmd_buffer = beginSingleTimeCommands(logical_device, cmd_pool);

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = { width, height, 1 };

    vkCmdCopyBufferToImage(cmd_buffer, src_buffer.buffer_, image_,
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    endSingleTimeCommands(logical_device, cmd_pool, cmd_buffer, submit_queue);

  }

  // Clean all the image used resources
  void clean(VkDevice logical_device) {
    if (texture_sampler_ != VK_NULL_HANDLE) {
      vkDestroySampler(logical_device, texture_sampler_, nullptr);
    }

    vkDestroyImageView(logical_device, image_view_, nullptr);
    vkDestroyImage(logical_device, image_, nullptr);

    if (image_memory_ != VK_NULL_HANDLE) {
      vkFreeMemory(logical_device, image_memory_, nullptr);
    }
  }

  // Creates a image view resource
  void createImageView(VkDevice logical_device, VkFormat format, VkImageAspectFlags aspect_flags){

    VkImageViewCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    create_info.image = image_;
    create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    create_info.format = format;

    // Remapping components (Set to default)
    create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    // Image usage (color bit for those)
    create_info.subresourceRange.aspectMask = aspect_flags;
    create_info.subresourceRange.baseMipLevel = 0;
    create_info.subresourceRange.levelCount = 1;
    create_info.subresourceRange.baseArrayLayer = 0;
    create_info.subresourceRange.layerCount = 1;

    if (vkCreateImageView(logical_device, &create_info, nullptr, &image_view_) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create texture image view.");
    }

  }
  
  // Creates the sampler for texture filtering and shader reading
  void createSampler(VkPhysicalDevice phys_device, VkDevice logical_device) {

    VkSamplerCreateInfo sampler_info{};
    sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_info.minFilter = VK_FILTER_LINEAR;
    sampler_info.magFilter = VK_FILTER_LINEAR;

    sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

    sampler_info.anisotropyEnable = VK_TRUE;
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(phys_device, &properties);
    sampler_info.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

    sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    sampler_info.unnormalizedCoordinates = VK_FALSE;
    sampler_info.compareEnable = VK_FALSE;
    sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
    sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_info.mipLodBias = 0.0f;
    sampler_info.minLod = 0.0f;
    sampler_info.maxLod = 0.0f;

    if (vkCreateSampler(logical_device, &sampler_info, nullptr, &texture_sampler_) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create texture sampler");
    }
  
  }



  ImageType image_type_;

  VkImage image_;
  VkDeviceMemory image_memory_;
  VkImageView image_view_;
  VkSampler texture_sampler_;

  uint32_t width_;
  uint32_t height_;

};

// ------------------------------------------------------------------------- //

class Material : public GPUResource {
public:
  Material() { material_id_ = -1; }
  ~Material() {}


  // maybe it would be better with ids, as these things will be created in the internal file
  // so they could be in an array or something like the buffers

	VkPipeline graphics_pipeline_; // uses the descSet layout
	VkPipelineLayout pipeline_layout_; // defines the layout of the descriptor set layouts on the pipeline
	
  int material_id_;

};

// ------------------------------------------------------------------------- //

// ------------------------------------------------------------------------- //

// ------------------------------------------------------------------------- //

static uint32_t findMemoryType(VkPhysicalDevice physical_device, uint32_t type_filter, VkMemoryPropertyFlags properties) {

  // Request supported memory properties from the graphics card
  VkPhysicalDeviceMemoryProperties mem_properties;
  vkGetPhysicalDeviceMemoryProperties(physical_device, &mem_properties);

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

#endif // __INTERNAL_GPU_RESOURCES_H__