/*
* Author: Diego Ochando Torres
* Date: 30/12/2020
* e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
*/

#ifndef __ENTITY_H__
#define __ENTITY_H__

// ------------------------------------------------------------------------- // 

#include <map>

#include "components/component.h"

// ------------------------------------------------------------------------- //

/**
* @brief Represents an object in the world.
*        Entity Component System based (Possible improvement after particles to DOD).
*/
class Entity {
public:
	Entity();
	~Entity();

	/**
	* @brief: Use of archetypes could benefit in a DOD implementation to store object data in
	* 		    different data chunks for each archetype, where only certain systems will act.
	*/
	enum Archetype {
		kArchetype_3DObject = 0,
		kArchetype_ParticleSystem = 1,
	};

	/// @brief Add the required components for an archetype.
	void initAsArchetype(Archetype archetype);

	bool hasComponent(Component::ComponentKind component_kind);
	Component* getComponent(Component::ComponentKind component_kind);

private:
	void addComponent(Component* new_component);
	void removeComponent(Component::ComponentKind component_kind);

	/// @brief Stored components with a key to its kind.
	std::map<Component::ComponentKind, Component*> components_;

};

// ------------------------------------------------------------------------- //

#endif // __GAME_OBJECT_H__

