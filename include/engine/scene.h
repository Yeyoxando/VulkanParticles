/*
* Author: Diego Ochando Torres
* Date: 30/12/2020
* e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
*/

#ifndef __SCENE_H__
#define __SCENE_H__

// ------------------------------------------------------------------------- // 

#include <vector>

#include "entity.h"

// ------------------------------------------------------------------------- //

struct SceneSettings {
	// This will be send as parameter to the scene to initialize certain values

  // name
	// settings like fog
  // Global lighting
  // ...

};

// ------------------------------------------------------------------------- //

class Scene{
public:
  Scene();
  ~Scene();

  // Set the scene name, to identify it easier
  void setName(const char* scene_name);

  void addEntity(Entity* entity, int material_id);
  // void removeEntity (Not for the moment due to dynamic buffers, until know how they work)

  void init();
  void update(float time);
  // draw()?

  // Return a vector of the entities which contains the given material id
	std::vector<Entity*> getEntities(int material_id);
  // Return the current number of objects from the entities list of a material kind
	int getNumberOfObjects(int material_id);

private:
  const char* name_;

  std::vector<Entity*> opaque_entities_;
  std::vector<Entity*> translucent_entities_;
  std::vector<Entity*> particle_entities_;

};

// ------------------------------------------------------------------------- //

#endif // __SCENE_H__

