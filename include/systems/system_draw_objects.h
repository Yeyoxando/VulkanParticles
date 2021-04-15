/*
 *	Author: Diego Ochando Torres
 *  Date: 09/01/2021
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

#ifndef __SYSTEM_DRAW_OBJECTS_H__
#define __SYSTEM_DRAW_OBJECTS_H__

// ------------------------------------------------------------------------- //

#include <vector>

#include <Vulkan/vulkan.h>
#include <glm.hpp>

#include "system.h"
#include "particle_editor.h"

// ------------------------------------------------------------------------- //

/**
* @brief This system acts on each entity with a 3D object archetype and a opaque material.
*        It creates a command buffer to draw all these entities.
*        It also updates the uniforms that they are using.
*/
class SystemDrawObjects : public System {
public:
	SystemDrawObjects();
	~SystemDrawObjects();

	/// @brief It adds a draw command for each opaque 3D object archetype in the entities vector to the current draw command buffer.
	void addDrawCommands(int cmd_buffer_image, VkCommandBuffer& cmd_buffer,
		std::vector<Entity*>& entities);

	/// @brief Updates the uniform buffer that opaque 3D objects use to be rendered.
	void updateUniformBuffers(int current_image, std::vector<Entity*>& entities);

protected:
	/// @return Model matrix for all the opaque 3D objects.
	glm::mat4* getModelMatrices(std::vector<Entity*>& entities);

	/// @return Opaque materials data for all the 3D objects (Shader uniforms).
	glm::mat4* getOpaqueMaterialsData(std::vector<Entity*>& entities);



	friend struct ParticleEditor::AppData;

};

// ------------------------------------------------------------------------- //

#endif // __SYSTEM_DRAW_OBJECTS_H__