/*
 *	Author: Diego Ochando Torres
 *  Date: 30/12/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

 // ------------------------------------------------------------------------- //

#include "entity.h"
#include "components/component_mesh.h"
#include "components/component_material.h"
#include "components/component_transform.h"
#include "components/component_particle_system.h"

// ------------------------------------------------------------------------- //

Entity::Entity() {
  
  components_ = std::map<Component::ComponentKind, Component*>();

  // add transform by default
  ComponentTransform* transform = new ComponentTransform();
  addComponent(transform);

}

// ------------------------------------------------------------------------- //

Entity::~Entity() {

  // For now components won't be shared
  // for the moment when an entity is destroyed it will destroy its associated components
  auto it = components_.cbegin();
  while (it != components_.cend()) {
    components_.erase(it);
    it = components_.cbegin();
    ++it;
  }

}

void Entity::initAsArchetype(Archetype archetype){

  switch (archetype) {
  case kArchetype_3DObject: {
    // create mesh and mat component
		ComponentMesh* mesh = new ComponentMesh();
		addComponent(mesh);
		ComponentMaterial* material = new ComponentMaterial();
		addComponent(material);
    break;
  }
  case kArchetype_ParticleSystem: {
		// create particle system comp
		ComponentParticleSystem* ps = new ComponentParticleSystem();
		addComponent(ps);
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
  if (hasComponent(new_component->component_kind_)) return;

  components_.insert(std::pair(new_component->component_kind_, new_component));

}

// ------------------------------------------------------------------------- //

void Entity::removeComponent(Component::ComponentKind component_kind){

  if (!hasComponent(component_kind)) return;

  components_.erase(component_kind);

}

// ------------------------------------------------------------------------- //

bool Entity::hasComponent(Component::ComponentKind component_kind){

  if (components_.find(component_kind) == components_.end()) return false;

  return true;

}

// ------------------------------------------------------------------------- //

Component* Entity::getComponent(Component::ComponentKind component_kind){

  if (!hasComponent(component_kind)) return nullptr;

  return components_.at(component_kind);

}

// ------------------------------------------------------------------------- //
