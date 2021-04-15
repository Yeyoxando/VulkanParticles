/*
 *	Author: Diego Ochando Torres
 *  Date: 14/04/2021
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

 // ------------------------------------------------------------------------- // 

#include "engine/common_def.h"

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
*	THE BIG BANG EFFECT!!!
*
*	And the big optimization challenge!!!
*	This scene will be used to work in the editor's performance.
*/
int main() {

	// Scene creation and settings, entities are added after them settings
	Scene* scene = new Scene();
	scene->setName("Particles performance test");


	/*Entity* floor = new Entity();
	{
		floor->initAsArchetype(Entity::kArchetype_3DObject);

		// Get transform component
		ComponentTransform* transform = static_cast<ComponentTransform*>
			(floor->getComponent(Component::kComponentKind_Transform));
		transform->translate(glm::vec3(0.0f, -0.1f, 0.0f));
		transform->scale(glm::vec3(5.0f, 0.0f, 5.0f));


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
		opaque_instance_data->loadAlbedoTexture("../../../resources/textures/numerical_grid.jpg");
		opaque_instance_data->color_ = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		mat->setInstanceData(opaque_instance_data);

		scene->addEntity(floor, opaque_instance_data->getParentID());
	}*/


	// Particle system creation and setting
	Entity* particle_system = new Entity();
	{
		particle_system->initAsArchetype(Entity::kArchetype_ParticleSystem);

		// Transform affect to all particles
		ComponentTransform* transform = static_cast<ComponentTransform*>
			(particle_system->getComponent(Component::kComponentKind_Transform));

		// Get particle system component
		ComponentParticleSystem* psc1 = static_cast<ComponentParticleSystem*>
			(particle_system->getComponent(Component::kComponentKind_ParticleSystem));
		// Initialize particle system with max particles
		psc1->init(10000);
		psc1->setBurst();
		psc1->setLifetime(0.0f);
		//psc1->setConstantVelocity(glm::vec3(0.0f, 0.0f, 0.1f));
		psc1->setInitialVelocity(glm::vec3(-0.4f, -0.4f, -0.4f), glm::vec3(0.4f, 0.4f, 0.4f));
		psc1->setParticleColor(glm::vec4(1.0f, 1.0f, 1.0f, 0.33f));

		psc1->loadTexture("../../../resources/textures/dot.png");

		scene->addEntity(particle_system, (int)ParticleEditor::MaterialParent::kMaterialParent_Particles);
	}



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