/*
 *	Author: Diego Ochando Torres
 *  Date: 09/01/2021
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

// ------------------------------------------------------------------------- //

#include "systems/system.h"

// ------------------------------------------------------------------------- //

System::System(){



}

// ------------------------------------------------------------------------- //

bool System::hasRequiredComponents(Entity* entity){

	for (auto comp_kind : required_components_) {
		if (!entity->hasComponent(comp_kind)) return false;
	}

	return true;

}

// ------------------------------------------------------------------------- //

void System::setRequiredComponent(Component::ComponentKind comp_kind){

	required_components_.push_back(comp_kind);

}

// ------------------------------------------------------------------------- //

void System::setRequiredArchetype(Entity::Archetype archetype){

	// transform components are not required because are added by default

	switch (archetype) {
	case Entity::Archetype::kArchetype_3DObject: {
		required_components_.push_back(Component::ComponentKind::kComponentKind_Mesh);
		required_components_.push_back(Component::ComponentKind::kComponentKind_Material);
		break;
	}
	case Entity::Archetype::kArchetype_ParticleSystem: {
		required_components_.push_back(Component::ComponentKind::kComponentKind_ParticleSystem);
		break;
	}
	default: {
		// assert
		break;
	}
	}

}

// ------------------------------------------------------------------------- //

System::~System() {



}

// ------------------------------------------------------------------------- //

