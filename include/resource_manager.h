/*
 *	Author: Diego Ochando Torres
 *  Date: 23/12/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

/*
 - Class to manage all graphic resources from Vulkan API
 - The test has to include this class and do other things on its update
 - Different graphics pipelines
 - Different uniform buffer objects for the different pipelines
 - Draw the model and a big transparent quad to achieve the first steps
 */
 
#ifndef __RESOURCE_MANAGER_H__
#define __RESOURCE_MANAGER_H__

// ------------------------------------------------------------------------- // 

// Includes

// ------------------------------------------------------------------------- // 

class ResourceManager {
public:
  // Empty
  ResourceManager();
  // Empty
  ~ResourceManager();

  // Initialize here all the things to call them when needed and not in object construction
  void init();
  // Delete allocated resources and finishes here to control the destruction order of the managers
  void shutDown();

  // Create all the resource structures with an id of when it was created and to get it after from other sides
  /*
  
    // Used for textures and framebuffers
    struct Image{
      uint32_t data_;
      VkImage texture_image_;
      VkDeviceMemory texture_image_memory_;
      VkImageView texture_image_view_;
      ...
      ...
      uint32_t id_;
    };



    enum BufferType{
      k_BufferType_Vertex = 0,
      k_BufferType_Index = 1,
      ...
    };

    struct Buffer{
      VkBuffer buffer_;
      VkDeviceMemory buffer_memory_;
      ...
      uint32_t id_;
    };



    struct UniformBuffers????



    // Graphic pipelines
    struct PSO{
      VkPipeline graphics_pipeline_;
      ...
      ...
      uint32_t id_;
    }

  */

private:
  // Arrays or vectors of created resources
  // textures
  // framebuffers
  // psos
  // vertex buffers
  // index buffers

};

// ------------------------------------------------------------------------- // 

#endif // __RESOURCE_MANAGER_H__