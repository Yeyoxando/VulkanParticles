/*
 *	Author: Diego Ochando Torres
 *  Date: 05/12/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */
 
// ------------------------------------------------------------------------- // 

#include "engine/common_def.h"
#ifdef MAIN_BILLBOARDS

#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include "particle_editor.h"

#include "components/component_mesh.h"
#include "components/component_material.h"
#include "components/component_transform.h"
#include "components/component_particle_system.h"

/*
TODO (ordered):
  - implement the same for particles
  - use blend mode which not requires sorting (additive?)

	- static getDefaultGraphicsPipelineCreateInfo and tweak only what its needed on each one
	- order objects by mesh and call bindVertex and bindIndex only when needed

  - implement radix sort    

  - Extra
    - add texture tiling to opaque mat
	  - add an update component with a callback to customize it from the outside as a scripting method
		- only one bind vertex and idx (packed vertex buffers for normal objects)
		  (not needed for particles as they are all going to be quads)
	  - add a simple directional light and a specular texture to the opaque material
    - node system?

  - clean everything and made a little example
  - finished step 2

  - start step 3 -> minimum to do for the submission (Complete principal objectives, Modular DOD-Vulkan)

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
    //mesh->loadDefaultMesh(ParticleEditor::DefaultMesh::kDefaultMesh_Quad);


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

		scene->addEntity(scenery, opaque_instance_data->getParentID());
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
		//mesh->loadDefaultMesh(ParticleEditor::DefaultMesh::kDefaultMesh_Quad);


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

		scene->addEntity(scenery2, opaque_instance_data->getParentID());
	}


	Entity* scenery3 = new Entity();
	{
		scenery3->initAsArchetype(Entity::kArchetype_3DObject);

		// Get transform component
		ComponentTransform* transform = static_cast<ComponentTransform*>
			(scenery3->getComponent(Component::kComponentKind_Transform));
		// z, x, y
		transform->translate(glm::vec3(0.0f, 0.0f, -0.1f));
		transform->scale(glm::vec3(5.0f, 5.0f, 1.0f));


		// Get mesh component
		ComponentMesh* mesh = static_cast<ComponentMesh*>
			(scenery3->getComponent(Component::kComponentKind_Mesh));

		// Load a model
		//mesh->loadMeshFromFile("../../../resources/models/viking_room.obj");
		mesh->loadDefaultMesh(ParticleEditor::DefaultMesh::kDefaultMesh_Quad);


		// Get material component
		ComponentMaterial* mat = static_cast<ComponentMaterial*>
			(scenery3->getComponent(Component::kComponentKind_Material));
		// Indicate material parent to internally set pipeline, descriptor set...
		mat->setMaterialParent(ParticleEditor::kMaterialParent_Opaque);

		// Create instance data
		ComponentMaterial::OpaqueData* opaque_instance_data = new ComponentMaterial::OpaqueData();
		//opaque_instance_data->loadAlbedoTexture("../../../resources/textures/viking_room.png");
		opaque_instance_data->loadAlbedoTexture("../../../resources/textures/numerical_grid.jpg");
    opaque_instance_data->color_ = glm::vec4(1.0f, 1.0f, 0.8f, 1.0f);
		mat->setInstanceData(opaque_instance_data);

		scene->addEntity(scenery3, opaque_instance_data->getParentID());
	}


	Entity* translucent1 = new Entity();
	{
		translucent1->initAsArchetype(Entity::kArchetype_3DObject);

		// Get transform component
		ComponentTransform* transform = static_cast<ComponentTransform*>
			(translucent1->getComponent(Component::kComponentKind_Transform));
		// z, x, y
		transform->translate(glm::vec3(0.0f, 2.0f, 0.0f));
		transform->rotate(glm::vec3(0.0f, 0.0f, 0.0f));


		// Get mesh component
		ComponentMesh* mesh = static_cast<ComponentMesh*>
			(translucent1->getComponent(Component::kComponentKind_Mesh));
		// Load a model
		mesh->loadDefaultMesh(ParticleEditor::DefaultMesh::kDefaultMesh_Quad);


		// Get material component
		ComponentMaterial* mat = static_cast<ComponentMaterial*>
			(translucent1->getComponent(Component::kComponentKind_Material));
		// Indicate material parent to internally set pipeline, descriptor set...
		mat->setMaterialParent(ParticleEditor::kMaterialParent_Translucent);

		// Create instance data
		ComponentMaterial::TranslucentData* translucent_data = new ComponentMaterial::TranslucentData();
		translucent_data->loadAlbedoTexture("../../../resources/textures/smoke_texture_trasnparency.png");
		//translucent_data->color_ = glm::vec4(1.0f, 1.0f, 0.8f, 1.0f);
		mat->setInstanceData(translucent_data);

		scene->addEntity(translucent1, translucent_data->getParentID());
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

    scene->addEntity(particle_system, 2);
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