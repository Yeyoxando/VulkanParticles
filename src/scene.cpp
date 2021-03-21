/*
 *	Author: Diego Ochando Torres
 *  Date: 30/12/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

 // ------------------------------------------------------------------------- //

#include "scene.h"
#include "systems/system.h"

// ------------------------------------------------------------------------- //

Scene::Scene() {

}

// ------------------------------------------------------------------------- //

Scene::~Scene() {

	for (int i = 0; i < entities_.size(); i++) {
		delete entities_[i];
	}

}

// ------------------------------------------------------------------------- //

void Scene::setName(const char* scene_name){

	name_ = scene_name;

}

// ------------------------------------------------------------------------- //

void Scene::addEntity(Entity* entity){

	entities_.push_back(entity);

}

// ------------------------------------------------------------------------- //

void Scene::init(){



}

// ------------------------------------------------------------------------- //

void Scene::update(){



}

// ------------------------------------------------------------------------- //

std::vector<Entity*> Scene::getEntities(){

	return entities_;

}

// ------------------------------------------------------------------------- //

int Scene::getNumberOfObjects(){

	int objects = 0;
	System helper_system;
	helper_system.setRequiredArchetype(Entity::kArchetype_3DObject);

	for (auto entity : entities_) {
		if (helper_system.hasRequiredComponents(entity)) {
			objects++;
		}
	}

	return objects;

}

// ------------------------------------------------------------------------- //
