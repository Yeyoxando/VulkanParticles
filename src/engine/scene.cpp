/*
 *	Author: Diego Ochando Torres
 *  Date: 30/12/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

 // ------------------------------------------------------------------------- //

#include "engine/scene.h"
#include "systems/system.h"
#include "components/component_particle_system.h"

#include <stdexcept>

// ------------------------------------------------------------------------- //

Scene::Scene() {

}

// ------------------------------------------------------------------------- //

Scene::~Scene() {

	for (int i = 0; i < opaque_entities_.size(); i++) {
		delete opaque_entities_[i];
	}

	for (int i = 0; i < translucent_entities_.size(); i++) {
		delete translucent_entities_[i];
	}

	for (int i = 0; i < particle_entities_.size(); i++) {
		delete particle_entities_[i];
	}

}

// ------------------------------------------------------------------------- //

void Scene::setName(const char* scene_name){

	name_ = scene_name;

}

// ------------------------------------------------------------------------- //

void Scene::addEntity(Entity* entity, int material_id){

	switch (material_id) {
	case 0: {
		opaque_entities_.push_back(entity);
		break;
	}
	case 1: {
		translucent_entities_.push_back(entity);
		break;
	}
	case 2: {
		particle_entities_.push_back(entity);
		break;
	}
	default: {
		throw std::runtime_error("\n Material ID do not exist.");
		break;
	}
	}

}

// ------------------------------------------------------------------------- //

void Scene::init(){



}

// ------------------------------------------------------------------------- //

void Scene::update(double time){

	for (int i = 0; i < particle_entities_.size(); ++i){

		auto ps = static_cast<ComponentParticleSystem*>
			(particle_entities_[i]->getComponent(Component::ComponentKind::kComponentKind_ParticleSystem));
		ps->emit(time);
		ps->update(time);
		ps->sort();

	}

}

// ------------------------------------------------------------------------- //

std::vector<Entity*> Scene::getEntities(int material_id){

	switch (material_id) {
	case 0: {
		return opaque_entities_;
		break;
	}
	case 1: {
		return translucent_entities_;
		break;
	}
	case 2: {
		return particle_entities_;
		break;
	}
	default: {
		throw std::runtime_error("\n Material ID do not exist.");
		break;
	}
	}

	return std::vector<Entity*>();

}

// ------------------------------------------------------------------------- //

int Scene::getNumberOfObjects(int material_id){

	switch (material_id) {
	case 0: {
		return opaque_entities_.size();
		break;
	}
	case 1: {
		return translucent_entities_.size();
		break;
	}
	case 2: {
		int num_particles = 0;
		for (int i = 0; i < particle_entities_.size(); ++i) {
			auto ps = static_cast<ComponentParticleSystem*>
				(particle_entities_[i]->getComponent(Component::ComponentKind::kComponentKind_ParticleSystem));
			num_particles += ps->getMaxParticles();
		}
		return num_particles;
		break;
	}
	default: {
		throw std::runtime_error("\n Material ID do not exist.");
		break;
	}
	}

	return 0;

}

// ------------------------------------------------------------------------- //
