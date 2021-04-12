/*
 *	Author: Diego Ochando Torres
 *  Date: 11/01/2021
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

#ifndef __SYSTEM_DRAW_PARTICLES_H__
#define __SYSTEM_DRAW_PARTICLES_H__

 // ------------------------------------------------------------------------- //

#include <vector>

#include <Vulkan/vulkan.h>
#include <glm.hpp>

#include "system.h"
#include "particle_editor.h"

struct Particle;

// ------------------------------------------------------------------------- //

/**
* @brief This system acts on each entity with a particle system archetype.
*        It creates a command buffer to draw all the particles of each particle system.
*        It also updates the uniforms that they are using.
*/
class SystemDrawParticles : public System {
public:
	SystemDrawParticles();
	~SystemDrawParticles();

	/// @brief It adds a draw command for each particle of a particle system archetype in the entities vector to the current draw command buffer.
	void addParticlesDrawCommand(int cmd_buffer_image, VkCommandBuffer& cmd_buffer, std::vector<Entity*>& entities);

	/// @brief Updates the uniform buffer that particles use to be rendered.
	void updateUniformBuffers(int current_image, std::vector<Entity*>& entities);

protected:
	/// @return Model matrix for all the particles.
	glm::mat4* getParticlesModelMatrices(std::vector<Entity*>& entities);

	/// @return Particle materials data for all the particles (Shader uniforms).
	glm::mat4* getParticleMaterialsData(std::vector<Entity*>& entities);

};

// ------------------------------------------------------------------------- //

#endif // __SYSTEM_DRAW_PARTICLES_H__