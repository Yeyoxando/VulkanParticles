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
    kComponentKind_Invalid = -1,
    kComponentKind_Mesh = 0,
    kComponentKind_Material,
    kComponentKind_ParticleSystem,
  };

  explicit Component(ComponentKind comp_kind) : component_kind_(comp_kind) {};

  ComponentKind component_kind_;

protected:
  ~Component();

  int entity_reference_id_;

};

// ------------------------------------------------------------------------- //

#endif // __COMPONENT_H__
