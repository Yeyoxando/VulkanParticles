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

class System {
public:
  System();
  virtual ~System();

  // Check if the entity has the components required by the system
  bool hasRequiredComponents(Entity* entity);

  // Called to set the components directly using an archetype
  void setRequiredArchetype(Entity::Archetype archetype);


protected:

	// Can be called multiple times to set where the system should act
	void setRequiredComponent(Component::ComponentKind comp_kind);

  std::vector<Component::ComponentKind> required_components_;

};

// ------------------------------------------------------------------------- //

#endif // __SYSTEM_H__