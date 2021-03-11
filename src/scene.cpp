/*
 *	Author: Diego Ochando Torres
 *  Date: 30/12/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

 // ------------------------------------------------------------------------- //

#include "scene.h"

// ------------------------------------------------------------------------- //

Scene::Scene() {

}

// ------------------------------------------------------------------------- //

Scene::~Scene() {

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
