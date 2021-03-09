/*
 *	Author: Diego Ochando Torres
 *  Date: 30/12/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

 // ------------------------------------------------------------------------- //

#include "entity.h"

// ------------------------------------------------------------------------- //

Entity::Entity() {
  
  // add transform by default

}

// ------------------------------------------------------------------------- //

Entity::~Entity() {

  // For now components won't be shared
  // for the moment when an entity is destroyed it will destroy its associated components

}

void Entity::initAsArchetype(Archetype archetype){

  switch (archetype) {
  case kArchetype_3DObject: {
    // create mesh and mat component
    break;
  }
  case kArchetype_ParticleSystem: {
    // create particle system comp
    break;
  }
  default: {
    // assert
    break;
  }
  }

}

// ------------------------------------------------------------------------- //

void Entity::addComponent(Component* new_component){

	// When an entity adds a new component it will check if its entity id is not initialized,
	// if not it will added, if added to another entity earlier it will trigger an error

}

// ------------------------------------------------------------------------- //

void Entity::removeComponent(Component::ComponentKind component_kind){



}

// ------------------------------------------------------------------------- //

bool Entity::hasComponent(Component::ComponentKind component_kind){

  return false;

}

// ------------------------------------------------------------------------- //

Component* Entity::getComponent(Component::ComponentKind component_kind){

  return nullptr;

}

// ------------------------------------------------------------------------- //
