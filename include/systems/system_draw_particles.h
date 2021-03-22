/*
 *	Author: Diego Ochando Torres
 *  Date: 11/01/2021
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

#ifndef __SYSTEM_DRAW_PARTICLES_H__
#define __SYSTEM_DRAW_PARTICLES_H__

// ------------------------------------------------------------------------- //

#include <vector>

#include "systems/system.h"
#include "glm/glm.hpp"
#include "vulkan/vulkan.h"

// ------------------------------------------------------------------------- //

class SystemDrawParticles : public System {
public:
	SystemDrawParticles();
	~SystemDrawParticles();

	// It will create one command and it will add it to the current command buffer containing all the objects
	void drawObjectsCommand(int cmd_buffer_image, VkCommandBuffer& cmd_buffer, std::vector<Entity*>& entities);

	// Updates the uniform buffer where objects are rendered (max objects set somewhere)
	void updateUniformBuffers(int current_image, std::vector<Entity*>& entities);

protected:
	// Return the model matrix for all the objects
	glm::mat4* getObjectModels(std::vector<Entity*>& entities);

	// Return the translucent data for all the objects
	glm::mat4* getObjectTranslucentData(std::vector<Entity*>& entities);

};

// ------------------------------------------------------------------------- //

#endif // __SYSTEM_DRAW_PARTICLES_H__