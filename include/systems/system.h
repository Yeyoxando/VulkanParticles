/*
 *	Author: Diego Ochando Torres
 *  Date: 09/01/2021
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

// ------------------------------------------------------------------------- //

#include "components/component.h"
#include "entity.h"

#include <vector>

// ------------------------------------------------------------------------- //

/**
* @brief Base for all the systems implemented in the ECS.
*/
class System {
public:
	System();
	virtual ~System();

	/// @brief Checks if an entity has the components required by the system.
	bool hasRequiredComponents(Entity* entity);

	/// @brief Sets the required components directly using an archetype for the system to act.
	void setRequiredArchetype(Entity::Archetype archetype);

protected:

	std::vector<Component::ComponentKind> required_components_;

};

// ------------------------------------------------------------------------- //

#endif // __SYSTEM_H__