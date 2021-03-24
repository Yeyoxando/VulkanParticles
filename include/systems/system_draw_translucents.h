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
#include <glm/glm.hpp>

#include "particle_editor.h"
#include "systems/system.h"

// ------------------------------------------------------------------------- //

class SystemDrawTranslucents : public System {
public:
  SystemDrawTranslucents();
  ~SystemDrawTranslucents();

  // It will create one command and it will add it to the current command buffer 
  // containing all the objects
  void drawObjectsCommand(int cmd_buffer_image, VkCommandBuffer& cmd_buffer,
    std::vector<Entity*>& entities);

  // Updates the uniform buffer where objects are rendered (max objects set somewhere)
  void updateUniformBuffers(int current_image, std::vector<Entity*>& entities);

protected:
  // Return the model matrix for all the objects
  glm::mat4* getObjectModels(std::vector<Entity*>& entities);

  // Return the opaque data for all the objects
  glm::mat4* getObjectTranslucentData(std::vector<Entity*>& entities);
  


  friend struct ParticleEditor::AppData;

};

// ------------------------------------------------------------------------- //

#endif // __SYSTEM_DRAW_OBJECTS_H__