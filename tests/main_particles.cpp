/*
 *	Author: Diego Ochando Torres
 *  Date: 05/12/2020
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

int main() {

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


		// Get material component
		ComponentMaterial* mat = static_cast<ComponentMaterial*>
			(scenery->getComponent(Component::kComponentKind_Material));
		// Indicate material parent to internally set pipeline, descriptor set...
		mat->setMaterialParent(ParticleEditor::kMaterialParent_Opaque);

		// Create instance data
		ComponentMaterial::OpaqueData* opaque_instance_data = new ComponentMaterial::OpaqueData();
		opaque_instance_data->loadAlbedoTexture("../../../resources/textures/viking_room.png");
		mat->setInstanceData(opaque_instance_data);

		scene->addEntity(scenery, opaque_instance_data->getParentID());
	}


	// Particle system creation and setting
	Entity* particle_system = new Entity();
	{
		particle_system->initAsArchetype(Entity::kArchetype_ParticleSystem);

		// Transform affect to all particles
		ComponentTransform* transform = static_cast<ComponentTransform*>
			(particle_system->getComponent(Component::kComponentKind_Transform));
		transform->translate(glm::vec3(0.0f, 0.17f, -0.1f));
		transform->scale(glm::vec3(0.3f, 0.3f, 0.3f));

		// Get particle system component
		ComponentParticleSystem* psc1 = static_cast<ComponentParticleSystem*>
			(particle_system->getComponent(Component::kComponentKind_ParticleSystem));
		// Initialize particle system with max 100 particles
		psc1->init(300);
		//psc1->setEmissionRate(1.0f);

		psc1->loadTexture("../../../resources/textures/smoke.png");

		scene->addEntity(particle_system, (int)ParticleEditor::MaterialParent::kMaterialParent_Particles);
	}


	// Particle system creation and setting
	Entity* particle_system2 = new Entity();
	{
		particle_system2->initAsArchetype(Entity::kArchetype_ParticleSystem);

		// Transform affect to all particles
		ComponentTransform* transform = static_cast<ComponentTransform*>
			(particle_system2->getComponent(Component::kComponentKind_Transform));
		transform->translate(glm::vec3(0.64f, 0.7f, 0.26f));
		transform->scale(glm::vec3(0.2f, 0.2f, 0.2f));

		// Get particle system component
		ComponentParticleSystem* psc2 = static_cast<ComponentParticleSystem*>
			(particle_system2->getComponent(Component::kComponentKind_ParticleSystem));
		// Initialize particle system with max 60 particles
		psc2->init(150);
		//psc2->setEmissionRate(1.0f);

		psc2->loadTexture("../../../resources/textures/fire.png");

		scene->addEntity(particle_system2, (int)ParticleEditor::MaterialParent::kMaterialParent_Particles);
	}


	// Particle system creation and setting
	Entity* particle_system3 = new Entity();
	{
		particle_system3->initAsArchetype(Entity::kArchetype_ParticleSystem);

		// Transform affect to all particles
		ComponentTransform* transform = static_cast<ComponentTransform*>
			(particle_system3->getComponent(Component::kComponentKind_Transform));
		transform->translate(glm::vec3(-0.56f, 0.68f, 0.26f));
		transform->scale(glm::vec3(0.2f, 0.2f, 0.2f));

		// Get particle system component
		ComponentParticleSystem* psc3 = static_cast<ComponentParticleSystem*>
			(particle_system3->getComponent(Component::kComponentKind_ParticleSystem));
		// Initialize particle system with max 60 particles
		psc3->init(150);
		psc3->setEmissionRate(2.0f);

		psc3->loadTexture("../../../resources/textures/fire.png");

		scene->addEntity(particle_system3, (int)ParticleEditor::MaterialParent::kMaterialParent_Particles);
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