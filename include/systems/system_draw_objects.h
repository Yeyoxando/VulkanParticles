/*
 *	Author: Diego Ochando Torres
 *  Date: 09/01/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

#ifndef __SYSTEM_DRAW_OBJECTS_H__
#define __SYSTEM_DRAW_OBJECTS_H__

// ------------------------------------------------------------------------- //

#include <vector>

#include "systems/system.h"
#include "glm/glm.hpp"
#include "vulkan/vulkan.h"

// ------------------------------------------------------------------------- //

/* 
  - This will need a material and a mesh component to prepare the drawing of the entities
  - get all model matrices in the same piece of data to use dynamic buffers
*/

class SystemDrawObjects : public System{
public:
  SystemDrawObjects();
  ~SystemDrawObjects();

  // It will create one command and it will add it to the current command buffer containing all the objects
	void drawObjectsCommand(int cmd_buffer_image, VkCommandBuffer& cmd_buffer, std::vector<Entity*>& entities);

	// Updates the uniform buffer where objects are rendered (max objects set somewhere)
	void updateDynamicBuffer(int current_image, std::vector<Entity*>& entities);



	// It will delete current uniform buffers of all material components, for swap chain cleanup
	void deleteUniformBuffers(std::vector<Entity*>& entities);

protected:
  // Return the model matrix for all the objects
  std::vector<glm::mat4> getObjectModels(std::vector<Entity*> &entities); // It'll be used for dynamic buffers

};

// ------------------------------------------------------------------------- //

#endif // __SYSTEM_DRAW_OBJECTS_H__