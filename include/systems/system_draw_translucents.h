/*
 *	Author: Diego Ochando Torres
 *  Date: 22/03/2021
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

#ifndef __SYSTEM_DRAW_TRANSLUCENTS_H__
#define __SYSTEM_DRAW_TRANSLUCENTS_H__

 // ------------------------------------------------------------------------- //

#include <vector>

#include <Vulkan/vulkan.h>
#include <glm.hpp>

#include "particle_editor.h"
#include "systems/system.h"

// ------------------------------------------------------------------------- //

/**
* @brief This system acts on each entity with a 3D object archetype and a translucent material.
*        It creates a command buffer to draw all these entities.
*        It also updates the uniforms that they are using.
*/
class SystemDrawTranslucents : public System {
public:
	SystemDrawTranslucents();
	~SystemDrawTranslucents();

	/// @brief It adds a draw command for each translucent 3D object archetype in the entities vector to the current draw command buffer.
	void addDrawCommands(int cmd_buffer_image, VkCommandBuffer& cmd_buffer,
		std::vector<Entity*>& entities);

	/// @brief Updates the uniform buffer that translucent 3D objects use to be rendered.
	void updateUniformBuffers(int current_image, std::vector<Entity*>& entities);

protected:
	/// @return Model matrix for all the translucent 3D objects.
	glm::mat4* getModelMatrices(std::vector<Entity*>& entities);

	/// @return Translucent materials data for all the 3D objects (Shader uniforms).
	glm::mat4* getTranslucentMaterialsData(std::vector<Entity*>& entities);



	friend struct ParticleEditor::AppData;

};

// ------------------------------------------------------------------------- //

#endif // __SYSTEM_DRAW_OBJECTS_H__