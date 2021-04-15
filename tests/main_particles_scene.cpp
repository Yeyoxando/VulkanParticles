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

/**
* @brief
*	THE SCENE EXAMPLE!
*
*	Scene to test different particle effects combined with another environment objects using the ECS.
*	This scene will be used to work in the editor's usability.
*/
int main() {

	// Scene creation and settings, entities are added after them settings
	Scene* scene = new Scene();
	scene->setName("Particles scene test");


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

		// Add it to the scene
		scene->addEntity(scenery, opaque_instance_data->getParentID());
	}


	// Particle system creation and setting
	Entity* particle_system_smoke_puff = new Entity();
	{
		particle_system_smoke_puff->initAsArchetype(Entity::kArchetype_ParticleSystem);

		// Transform affect to all particles
		ComponentTransform* transform = static_cast<ComponentTransform*>
			(particle_system_smoke_puff->getComponent(Component::kComponentKind_Transform));
		transform->translate(glm::vec3(0.0f, 0.17f, -0.1f));
		transform->scale(glm::vec3(0.3f, 0.3f, 0.3f));

		// Get particle system component
		ComponentParticleSystem* ps = static_cast<ComponentParticleSystem*>
			(particle_system_smoke_puff->getComponent(Component::kComponentKind_ParticleSystem));
		// Initialize particle system with max particles
		ps->init(20);
		ps->setBurst();
		ps->setLifetime(5.14f);
		ps->setParticleColor(glm::vec4(1.0f, 1.0f, 1.0f, 0.33f));
		ps->setAlphaColorOverTime(0.0f);

		ps->loadTexture("../../../resources/textures/smoke.png");

		scene->addEntity(particle_system_smoke_puff, (int)ParticleEditor::MaterialParent::kMaterialParent_Particles);
	}

	// Particle system creation and setting
	Entity* particle_system_smoke_puff_two = new Entity();
	{
		particle_system_smoke_puff_two->initAsArchetype(Entity::kArchetype_ParticleSystem);

		// Transform affect to all particles
		ComponentTransform* transform = static_cast<ComponentTransform*>
			(particle_system_smoke_puff_two->getComponent(Component::kComponentKind_Transform));
		transform->translate(glm::vec3(0.0f, 0.17f, -0.1f));
		transform->scale(glm::vec3(0.2f, 0.2f, 0.3f));

		// Get particle system component
		ComponentParticleSystem* ps = static_cast<ComponentParticleSystem*>
			(particle_system_smoke_puff_two->getComponent(Component::kComponentKind_ParticleSystem));
		// Initialize particle system with max particles
		ps->init(20);
		ps->setBurst();
		ps->setLifetime(6.35f);
		ps->setParticleColor(glm::vec4(1.0f, 1.0f, 1.0f, 0.22f));
		ps->setAlphaColorOverTime(0.0f);

		ps->loadTexture("../../../resources/textures/smoke.png");

		scene->addEntity(particle_system_smoke_puff_two, (int)ParticleEditor::MaterialParent::kMaterialParent_Particles);
	}


	// Particle system creation and setting
	Entity* particle_system_fire_torch_right = new Entity();
	{
		particle_system_fire_torch_right->initAsArchetype(Entity::kArchetype_ParticleSystem);

		// Transform affect to all particles
		ComponentTransform* transform = static_cast<ComponentTransform*>
			(particle_system_fire_torch_right->getComponent(Component::kComponentKind_Transform));
		transform->translate(glm::vec3(0.63f, 0.69f, 0.26f));
		transform->scale(glm::vec3(0.2f, 0.2f, 0.2f));

		// Get particle system component
		ComponentParticleSystem* ps = static_cast<ComponentParticleSystem*>
			(particle_system_fire_torch_right->getComponent(Component::kComponentKind_ParticleSystem));
		// Initialize particle system with max particles
		ps->init(100);
		ps->setLifetime(2.0f);
		ps->setInitialVelocity(glm::vec3(-0.06f, -0.06f, 0.02f), glm::vec3(0.06f, 0.06f, 0.06f));
		ps->setAlphaColorOverTime(0.0f);

		ps->loadTexture("../../../resources/textures/fire.png");

		scene->addEntity(particle_system_fire_torch_right, (int)ParticleEditor::MaterialParent::kMaterialParent_Particles);
	}

	// Particle system creation and setting
	Entity* particle_system_smoke_torch_right = new Entity();
	{
		particle_system_smoke_torch_right->initAsArchetype(Entity::kArchetype_ParticleSystem);

		// Transform affect to all particles
		ComponentTransform* transform = static_cast<ComponentTransform*>
			(particle_system_smoke_torch_right->getComponent(Component::kComponentKind_Transform));
		transform->translate(glm::vec3(0.63f, 0.71f, 0.26f));
		transform->scale(glm::vec3(0.4f, 0.4f, 0.4f));

		// Get particle system component
		ComponentParticleSystem* ps = static_cast<ComponentParticleSystem*>
			(particle_system_smoke_torch_right->getComponent(Component::kComponentKind_ParticleSystem));
		// Initialize particle system with max particles
		ps->init(100);
		ps->setLifetime(6.0f);
		ps->setParticleColor(glm::vec4(1.0f, 1.0f, 1.0f, 0.17f));
		ps->setInitialVelocity(glm::vec3(-0.06f, -0.06f, 0.08f), glm::vec3(0.06f, 0.06f, 0.12f));
		ps->setAlphaColorOverTime(0.0f);

		ps->loadTexture("../../../resources/textures/smoke.png");

		scene->addEntity(particle_system_smoke_torch_right, (int)ParticleEditor::MaterialParent::kMaterialParent_Particles);
	}


	// Particle system creation and setting
	Entity* particle_system_fire_torch_left = new Entity();
	{
		particle_system_fire_torch_left->initAsArchetype(Entity::kArchetype_ParticleSystem);

		// Transform affect to all particles
		ComponentTransform* transform = static_cast<ComponentTransform*>
			(particle_system_fire_torch_left->getComponent(Component::kComponentKind_Transform));
		transform->translate(glm::vec3(-0.56f, 0.67f, 0.25f));
		transform->scale(glm::vec3(0.2f, 0.2f, 0.2f));

		// Get particle system component
		ComponentParticleSystem* ps = static_cast<ComponentParticleSystem*>
			(particle_system_fire_torch_left->getComponent(Component::kComponentKind_ParticleSystem));
		// Initialize particle system with max particles
		ps->init(100);
		ps->setLifetime(2.0f);
		ps->setInitialVelocity(glm::vec3(-0.06f, -0.06f, 0.02f), glm::vec3(0.06f, 0.06f, 0.06f));
		ps->setAlphaColorOverTime(0.0f);

		ps->loadTexture("../../../resources/textures/fire.png");

		scene->addEntity(particle_system_fire_torch_left, (int)ParticleEditor::MaterialParent::kMaterialParent_Particles);
	}

	// Particle system creation and setting
	Entity* particle_system_smoke_torch_left = new Entity();
	{
		particle_system_smoke_torch_left->initAsArchetype(Entity::kArchetype_ParticleSystem);

		// Transform affect to all particles
		ComponentTransform* transform = static_cast<ComponentTransform*>
			(particle_system_smoke_torch_left->getComponent(Component::kComponentKind_Transform));
		transform->translate(glm::vec3(-0.56f, 0.69f, 0.25f));
		transform->scale(glm::vec3(0.4f, 0.4f, 0.4f));

		// Get particle system component
		ComponentParticleSystem* ps = static_cast<ComponentParticleSystem*>
			(particle_system_smoke_torch_left->getComponent(Component::kComponentKind_ParticleSystem));
		// Initialize particle system with max particles
		ps->init(100);
		ps->setLifetime(6.0f);
		//ps->setEmissionRate(0.3f);
		ps->setParticleColor(glm::vec4(1.0f, 1.0f, 1.0f, 0.17f));
		ps->setInitialVelocity(glm::vec3(-0.06f, -0.06f, 0.08f), glm::vec3(0.06f, 0.06f, 0.12f));
		ps->setAlphaColorOverTime(0.0f);

		ps->loadTexture("../../../resources/textures/smoke.png");

		scene->addEntity(particle_system_smoke_torch_left, (int)ParticleEditor::MaterialParent::kMaterialParent_Particles);
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