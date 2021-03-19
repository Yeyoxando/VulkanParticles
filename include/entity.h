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

/*
  - ECS based (Possible improvement after particles to DOD)
  - Use of archetypes could benefit in a DOD implementation to store object data in different 
      chunks for each archetype, where only certain systems will act

*/

class Entity{
public:
  Entity();
  ~Entity();

  enum Archetype {
    kArchetype_3DObject = 0,
    kArchetype_ParticleSystem = 1,
  };

  // create an entity archetype with its required components added by default
  void initAsArchetype(Archetype archetype);

  bool hasComponent(Component::ComponentKind component_kind);
  Component* getComponent(Component::ComponentKind component_kind);

private:
  
  void addComponent(Component* new_component);
  void removeComponent(Component::ComponentKind component_kind);


  std::map<Component::ComponentKind, Component*> components_;
  int id_;
  //bool is_archetype_;

};

// ------------------------------------------------------------------------- //

#endif // __GAME_OBJECT_H__

