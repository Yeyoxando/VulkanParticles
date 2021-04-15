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
struct SceneSettings {
	// This will be send as parameter to the scene to initialize certain values

	// name
	// settings like fog
	// Global lighting
	// ...

};
*/
// ------------------------------------------------------------------------- //

/**
* @brief Class used to create examples using the particle editor.
*        ECS based.
*/
class Scene {
public:
	Scene();
	~Scene();

	/// @brief Set the scene name, to identify it easier when multiple ones are created.
	void setName(const char* scene_name);

	void init();
	void update(double time);

	/// @brief Adds a new entity to the scene, with its correspondent material ID
	void addEntity(Entity* entity, int material_id);
	// void removeEntity (Not for the moment due to fixed command buffer)

	/// @brief Return a vector of the entities which contains the given material id.
	std::vector<Entity*> getEntities(int material_id);
	/// @brief Return the current number of objects from the entities list of a material kind.
	int getNumberOfObjects(int material_id);

private:
	const char* name_;

	/// @brief Separated opaque objects by material to use them easier in the systems, it also can benefit in a later DOD improvement.
	std::vector<Entity*> opaque_entities_;
	/// @brief Separated translucent objects by material to use them easier in the systems, it also can benefit in a later DOD improvement.
	std::vector<Entity*> translucent_entities_;
	/// @brief Separated particle system objects by material to use them easier in the systems, it also can benefit in a later DOD improvement.
	std::vector<Entity*> particle_entities_;

};

// ------------------------------------------------------------------------- //

#endif // __SCENE_H__

