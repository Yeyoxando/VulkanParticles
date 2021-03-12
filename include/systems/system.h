/*
 *	Author: Diego Ochando Torres
 *  Date: 09/01/2020
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
  ~System();

protected:
  // Check if the entity has the components required by the system
  bool hasRequiredComponents(Entity* entity);
  
  // Can be called multiple times to set where the system should act
  void setRequiredComponent(Component::ComponentKind comp_kind);

  // Called to set the components directly using an archetype
  void setRequiredArchetype(Entity::Archetype archetype);


  std::vector<Component::ComponentKind> required_components_;

};

/* 
  - Systems will need to be executed by each entity which has certain components
  
*/

// ------------------------------------------------------------------------- //

#endif // __SYSTEM_H__