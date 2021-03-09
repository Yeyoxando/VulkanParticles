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

/*
  - It'll include a list of entities
  - Also global settings
*/

class Scene{
public:
  Scene();
  ~Scene();

  void setName(const char* scene_name);

  void addEntity(Entity* entity);


  void init();
  void update();
  // draw(), will be needed, or it will get the entities and do the commands from them
  // Or should it be a command like in suffer engine which draws the thing with its components and things

  std::vector<Entity*> getEntities();

private:
  const char* name_;

  std::vector<Entity*> entities_;

};

// ------------------------------------------------------------------------- //

#endif // __SCENE_H__

