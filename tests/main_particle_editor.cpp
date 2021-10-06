/*
 *	Author: Diego Ochando Torres
 *  Date: 14/04/2021
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

// ------------------------------------------------------------------------- // 

//#include "engine/common_def.h"

#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include "particle_editor.h"

#include "components/component_mesh.h"
#include "components/component_material.h"
#include "components/component_transform.h"
#include "components/component_particle_system.h"

// ------------------------------------------------------------------------- //

/**
* @brief
*	THE PLAYGROUND!
*
*	Scene to test different particle effect settings.
*	This scene will be used to work in the editor's flexibility.
*/
int main() {

	// Scene creation and settings, entities are added after them settings
	Scene* scene = new Scene();
	scene->setName("Particles editor test");

	Entity* particle_systems[16] = {};
	ComponentParticleSystem* ps_components[16];

	// 3D object creation and components setting
	Entity* floor = new Entity();
	{
		floor->initAsArchetype(Entity::kArchetype_3DObject);

		// Get transform component
		ComponentTransform* transform = static_cast<ComponentTransform*>
			(floor->getComponent(Component::kComponentKind_Transform));
		transform->scale(glm::vec3(8.0f, 0.0f, 8.0f));


		// Get mesh component
		ComponentMesh* mesh = static_cast<ComponentMesh*>
			(floor->getComponent(Component::kComponentKind_Mesh));

		// Load a model
		mesh->loadDefaultMesh(ParticleEditor::DefaultMesh::kDefaultMesh_Quad);


		// Get material component
		ComponentMaterial* mat = static_cast<ComponentMaterial*>
			(floor->getComponent(Component::kComponentKind_Material));
		// Indicate material parent to internally set pipeline, descriptor set...
		mat->setMaterialParent(ParticleEditor::kMaterialParent_Opaque);

		// Create instance data
		ComponentMaterial::OpaqueData* opaque_instance_data = new ComponentMaterial::OpaqueData();
		opaque_instance_data->loadAlbedoTexture("../../resources/textures/chessboard.jpg");
		opaque_instance_data->color_ = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f);
		mat->setInstanceData(opaque_instance_data);

		scene->addEntity(floor, opaque_instance_data->getParentID());
	}


	for (int x = 0; x < 4; ++x) {
		for (int z = 0; z < 4; ++z) {
			Entity* particle_system = particle_systems[z + (x * 4)];
			particle_system = new Entity();
			particle_system->initAsArchetype(Entity::kArchetype_ParticleSystem);

			// Transform affect to all particles
			ComponentTransform* transform = static_cast<ComponentTransform*>
				(particle_system->getComponent(Component::kComponentKind_Transform));
			transform->translate(glm::vec3(x * 2.0f - 3.0f, 0.0f, z * 2.0f - 3.0f));

			//ComponentParticleSystem* psc = ps_components[z + (x * 4)];
			ps_components[z + (x * 4)] = static_cast<ComponentParticleSystem*>(particle_system->getComponent
				(Component::kComponentKind_ParticleSystem));

			// Initialize particle system with max particles
			ps_components[z + (x * 4)]->init(150);
			ps_components[z + (x * 4)]->loadTexture("../../resources/textures/fire.png");

			scene->addEntity(particle_system, (int)ParticleEditor::MaterialParent::kMaterialParent_Particles);
		}
	}
	

	ps_components[0]->setBurst();
	ps_components[10]->setBurst();



	// Set up scene to the app!!
	ParticleEditor::instance().loadScene(scene);


	// Run particle editor
	try {
		ParticleEditor::instance().run();
	}
	catch (const std::exception& e) {
		printf(e.what());
		return EXIT_FAILURE;
	}



	return EXIT_SUCCESS;

}