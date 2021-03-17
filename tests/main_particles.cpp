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
#include "components/component_transform.h"
#include "components/component_particle_system.h"

/*
TODO (ordered):
  - draw lots of objects with dynamic uniforms
      - Move all the opaque things into another dynamic buffer instead of having lots of descriptor sets
      - update the dynamic buffer being careful with the offset and alignment
      - memory mapping and unmapping should only be done 1 time
  - implement the same for particles
  - use blend mode which not requires sorting (additive?)
  - implement radix sort    
  - add a callback for the scene update to customize it from the outside as a scripting method
  - clean everything and made a little example (add node system?)
  - Move Vulkan things to internal app (material, etc)
  - finished step 2

  - start step 3 -> min to do for the submission (Complete principal objectives, Modular DOD-Vulkan)

*/

int main(){
  
  //while (1) {} // 2MB start



	// Scene creation and settings, entities are added after them settings
	Scene* scene = new Scene();
	scene->setName("Particles test");


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
    mat->setMaterialParent(ParticleEditor::kMaterialParent_Opaque);

    // Create instance data
    ComponentMaterial::OpaqueData* opaque_instance_data = new ComponentMaterial::OpaqueData();
    opaque_instance_data->loadAlbedoTexture("../../../resources/textures/viking_room.png");
    //opaque_instance_data->loadAlbedoTexture("../../../resources/textures/smoke_texture_trasnparency.png");
    mat->setInstanceData(opaque_instance_data);

		scene->addEntity(scenery);
  }


	Entity* scenery2 = new Entity();
	{
		scenery2->initAsArchetype(Entity::kArchetype_3DObject);

		// Get transform component
		ComponentTransform* transform = static_cast<ComponentTransform*>
			(scenery2->getComponent(Component::kComponentKind_Transform));
    // z, x, y
    transform->translate(glm::vec3(0.0f, 0.0f, 1.6f));


		// Get mesh component
		ComponentMesh* mesh = static_cast<ComponentMesh*>
			(scenery2->getComponent(Component::kComponentKind_Mesh));

		// Load a model
		mesh->loadMeshFromFile("../../../resources/models/viking_room.obj");
		//mesh->loadDefaultMesh(BasicPSApp::DefaultMesh::kDefaultMesh_Quad);


		// Get material component
		ComponentMaterial* mat = static_cast<ComponentMaterial*>
			(scenery2->getComponent(Component::kComponentKind_Material));
		// Indicate material parent to internally set pipeline, descriptor set...
		mat->setMaterialParent(ParticleEditor::kMaterialParent_Opaque);

		// Create instance data
		ComponentMaterial::OpaqueData* opaque_instance_data = new ComponentMaterial::OpaqueData();
		opaque_instance_data->loadAlbedoTexture("../../../resources/textures/viking_room.png");
    opaque_instance_data->color_ = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
    mat->setInstanceData(opaque_instance_data);

		scene->addEntity(scenery2);
	}


	Entity* scenery3 = new Entity();
	{
		scenery3->initAsArchetype(Entity::kArchetype_3DObject);

		// Get transform component
		ComponentTransform* transform = static_cast<ComponentTransform*>
			(scenery3->getComponent(Component::kComponentKind_Transform));
		// z, x, y
		transform->translate(glm::vec3(0.0f, 0.0f, -1.6f));


		// Get mesh component
		ComponentMesh* mesh = static_cast<ComponentMesh*>
			(scenery3->getComponent(Component::kComponentKind_Mesh));

		// Load a model
		mesh->loadMeshFromFile("../../../resources/models/viking_room.obj");
		//mesh->loadDefaultMesh(BasicPSApp::DefaultMesh::kDefaultMesh_Quad);


		// Get material component
		ComponentMaterial* mat = static_cast<ComponentMaterial*>
			(scenery3->getComponent(Component::kComponentKind_Material));
		// Indicate material parent to internally set pipeline, descriptor set...
		mat->setMaterialParent(ParticleEditor::kMaterialParent_Opaque);

		// Create instance data
		ComponentMaterial::OpaqueData* opaque_instance_data = new ComponentMaterial::OpaqueData();
		//opaque_instance_data->loadAlbedoTexture("../../../resources/textures/viking_room.png");
		opaque_instance_data->loadAlbedoTexture("../../../resources/textures/smoke_texture_trasnparency.png");
		mat->setInstanceData(opaque_instance_data);

		scene->addEntity(scenery3);
	}


  // Particle system creation and setting
  /*Entity* particle_system = new Entity();
  {
    particle_system->initAsArchetype(Entity::kArchetype_ParticleSystem);
    
    // Get particle system component
    ComponentParticleSystem* ps = static_cast<ComponentParticleSystem*>
      (particle_system->getComponent(Component::kComponentKind_ParticleSystem));
    // Initialize particle system with max 500 particles
    ps->init(500);

    scene->addEntity(particle_system);
  }*/
  
  

  // Set up scene to the app!!
  ParticleEditor::instance().loadScene(scene);


  // Run particle editor
  try {
    ParticleEditor::instance().run();
  }
  catch(const std::exception& e) {
    printf(e.what());
    return EXIT_FAILURE;
  }


  // This is to check that memory gets cleaned
  //while (1) {} //5MB at the end -> 3MB??



  return EXIT_SUCCESS;
  
}

#endif