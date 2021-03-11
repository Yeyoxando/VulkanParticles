/*
 *	Author: Diego Ochando Torres
 *  Date: 11/01/2020
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

 /*
   - This will need a particle system component to prepare the drawing of the particles

 */

class SystemDrawParticles : public System {
public:
	SystemDrawParticles();

	// It will create one command and it will add it to the current command buffer containing all the particles
	void drawObjectsCommand(VkCommandBuffer cmd_buffer);

	// Updates the uniform buffer where particles are rendered (max particles set somewhere)
	void updateDynamicBuffer();

protected:
	// Return the model matrix for all the particles
	std::vector<glm::mat4> getObjectModels(); // It'll be used for dynamic buffers

	~SystemDrawParticles();

};

// ------------------------------------------------------------------------- //

#endif // __SYSTEM_DRAW_PARTICLES_H__