/*
* Author: Diego Ochando Torres
* Date: 30/12/2020
* e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
*/

#ifndef __ENTITY_H__
#define __ENTITY_H__

// ------------------------------------------------------------------------- // 

// Includes

// ------------------------------------------------------------------------- //

/*
  - ECS based (Possible improvement after particles to DOD)
  - Geometry / Mesh
  - Material
  - Particle system
  - etc

  - Upload uniform buffer at rendering
*/

class Entity{
public:
  Entity();
  ~Entity();

private:
  int id_;

};

// ------------------------------------------------------------------------- //

#endif // __GAME_OBJECT_H__

