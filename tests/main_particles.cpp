/*
 *	Author: Diego Ochando Torres
 *  Date: 05/12/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */
 
// ------------------------------------------------------------------------- // 

#include "common_def.h"
#ifdef MAIN_BILLBOARDS

#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include "basic_ps_app.h"

#include "components/component_mesh.h"
#include "components/component_material.h"
#include "components/component_particle_system.h"

/*
TODO (ordered):
	- separate vp and model matrices
  - use a color variable only for now to make an example of the different uniforms
  - draw two objects with dynamic uniforms        
  - draw lots of objects with dynamic uniforms     
  - implement the same for particles
  - use blend mode which not requires sorting (additive?)
  - implement radix sort    
  - add a callback for the scene update to customize it from the outside as a scripting method
  - clean everything and made a little example    
  - finished step 2

  - start step 3 -> min to do for the submission (Complete principal objectives, Modular DOD-Vulkan)

*/

int main(){
  //while (1) {} 2MB start

  // 3D object creation and components setting
  Entity* scenery = new Entity();
  {
    scenery->initAsArchetype(Entity::kArchetype_3DObject);

    // Get mesh component
    ComponentMesh* mesh = static_cast<ComponentMesh*>
      (scenery->getComponent(Component::kComponentKind_Mesh));

    // Load a model
    mesh->loadMeshFromFile("../../../resources/models/viking_room.obj");
    //mesh->loadDefaultMesh(BasicPSApp::DefaultMesh::kDefaultMesh_Quad);


    // Get material component
    ComponentMaterial* mat = static_cast<ComponentMaterial*>
      (scenery->getComponent(Component::kComponentKind_Material));
    // Indicate material parent to internally set pipeline, descriptor set...
    mat->setMaterialParent(BasicPSApp::kMaterialParent_Opaque);

    // Create instance data
    ComponentMaterial::OpaqueData* opaque_instance_data = new ComponentMaterial::OpaqueData();
    opaque_instance_data->loadAlbedoTexture("../../../resources/textures/viking_room.png");
    //opaque_instance_data->loadAlbedoTexture("../../../resources/textures/smoke_texture_trasnparency.png");
    mat->setInstanceData(opaque_instance_data);
  }


  // Particle system creation and setting
  Entity* particle_system = new Entity();
  {
    particle_system->initAsArchetype(Entity::kArchetype_ParticleSystem);
    
    // Get particle system component
    ComponentParticleSystem* ps = static_cast<ComponentParticleSystem*>
      (particle_system->getComponent(Component::kComponentKind_ParticleSystem));
    // Initialize particle system with max 500 particles
    ps->init(500);
  }
  
  
  // Scene creation, settings and entities adding
  Scene* scene = new Scene();
  {
    scene->setName("Particles test");

    scene->addEntity(scenery);
    scene->addEntity(particle_system);

    // Set up scene to the app!!
    BasicPSApp::instance().loadScene(scene);
  }


  // Run particle editor
  try {
    BasicPSApp::instance().run();
  }
  catch(const std::exception& e) {
    printf(e.what());
    return EXIT_FAILURE;
  }


  // This is to check that memory gets cleaned
  //while (1) {} //7MB at the end -> 5MB??



  return EXIT_SUCCESS;
  
}

#endif