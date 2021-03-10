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

int main(){

  // 3D object creation and components setting
  Entity* scenery = new Entity();
  {
    scenery->initAsArchetype(Entity::kArchetype_3DObject);

    // Get mesh component
    ComponentMesh* mesh = static_cast<ComponentMesh*>
      (scenery->getComponent(Component::kComponentKind_Mesh));
    // Load a model
    mesh->loadMeshFromFile("./mesh filename");


    // Get material component
    ComponentMaterial* mat = static_cast<ComponentMaterial*>
      (scenery->getComponent(Component::kComponentKind_Material));
    // Indicate material parent to internally set pipeline, descriptor set...
    mat->setMaterialParent(BasicPSApp::kMaterialParent_Opaque);
    // Create initial instance data
    ComponentMaterial::Mat1 opaque_instance_data;
    mat->setInstanceData(&opaque_instance_data);
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


  // Components get deleted automatically, only created entities and scene must be deleted

  // User cleanup
  delete scenery;
  delete particle_system;
  delete scene;


  // This is to check that memory gets cleaned
  //while (1) {}



  return EXIT_SUCCESS;
  
}

#endif