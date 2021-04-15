/*
* Author: Diego Ochando Torres
* Date: 31/12/2020
* e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
*/

#ifndef __COMPONENT_TRANSFORM_H__
#define __COMPONENT_TRANSFORM_H__

// ------------------------------------------------------------------------- // 

#include <glm.hpp>

#include "component.h"

// ------------------------------------------------------------------------- //

/**
* @brief Transform component for all entity archetypes.
*        Always added by default when creating an entity.
*/
class ComponentTransform : public Component {
public:
	ComponentTransform();

	/// @brief Translates the entity to a new position in world space.
	void translate(glm::vec3 position);
	/// @brief Rotates the entity to a new rotation in degrees in world space.
	void rotate(glm::vec3 rotation_degrees);
	/// @brief Scales the entity in world space.
	void scale(glm::vec3 scale);

	glm::vec3 getPosition();
	glm::vec3 getRotation();
	glm::vec3 getScale();
	/// @brief Get the model matrix for the current transform of the entity.
	glm::mat4 getModelMatrix();

protected:
	~ComponentTransform();

	/// @brief Computes the model matrix with the current transform of the entity.
	void computeModelMatrix();

	glm::vec3 position_;
	glm::vec3 rotation_;
	glm::vec3 scale_;

	/// @brief Last computed model matrix.
	glm::mat4 model_;

};

// ------------------------------------------------------------------------- //

#endif // __COMPONENT_TRANSFORM_H__
