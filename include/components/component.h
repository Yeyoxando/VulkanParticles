/*
* Author: Diego Ochando Torres
* Date: 31/12/2020
* e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
*/

#ifndef __COMPONENT_H__
#define __COMPONENT_H__

// ------------------------------------------------------------------------- // 

// Includes

// ------------------------------------------------------------------------- //

class Component {
public:

  enum ComponentKind {
    kComponentKind_Transform = 0,
    kComponentKind_Mesh = 1,
    kComponentKind_Material = 2,
    kComponentKind_ParticleSystem = 3,
  };

  explicit Component(ComponentKind comp_kind) : component_kind_(comp_kind) {};
  virtual ~Component();

  ComponentKind component_kind_;

protected:

  int entity_reference_id_;

};

// ------------------------------------------------------------------------- //

#endif // __COMPONENT_H__
