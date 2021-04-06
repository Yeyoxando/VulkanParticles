/*
* Author: Diego Ochando Torres
* Date: 31/12/2020
* e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
*/


#include "components/component_transform.h"
#include <matrix_transform.hpp>

// ------------------------------------------------------------------------- //

ComponentTransform::ComponentTransform() : Component(Component::kComponentKind_Transform) {

	position_ = glm::vec3(0.0f, 0.0f, 0.0f);
	rotation_ = glm::vec3(0.0f, 0.0f, 0.0f);
	scale_ = glm::vec3(1.0f, 1.0f, 1.0f);
	computeModelMatrix();

}

// ------------------------------------------------------------------------- //

void ComponentTransform::translate(glm::vec3 position){

	position_ = glm::vec3(-position.z, position.x, position.y);
	computeModelMatrix();

}

// ------------------------------------------------------------------------- //

void ComponentTransform::rotate(glm::vec3 rotation_degrees){

	rotation_ = glm::vec3(-rotation_degrees.z, -rotation_degrees.x, rotation_degrees.y);
	computeModelMatrix();

}

// ------------------------------------------------------------------------- //

void ComponentTransform::scale(glm::vec3 scale){

	scale_ = glm::vec3(scale.z, scale.x, scale.y);
	computeModelMatrix();

}

// ------------------------------------------------------------------------- //

glm::vec3 ComponentTransform::getPosition(){

	return position_;

}

// ------------------------------------------------------------------------- //

glm::vec3 ComponentTransform::getRotation(){

	return rotation_;

}

// ------------------------------------------------------------------------- //

glm::vec3 ComponentTransform::getScale(){

	return scale_;

}

// ------------------------------------------------------------------------- //

glm::mat4 ComponentTransform::getModelMatrix(){

	return model_;

}

// ------------------------------------------------------------------------- //

ComponentTransform::~ComponentTransform() {

}

// ------------------------------------------------------------------------- //

void ComponentTransform::computeModelMatrix(){

	//multiply matrices and store them in component
	glm::mat4 model = glm::mat4(1.0f);

  model = glm::translate(model, position_);
	model = glm::rotate(model, glm::radians(rotation_.x), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(rotation_.y), glm::vec3(0.0f, 1.0f, 0.0f));
  model = glm::rotate(model, glm::radians(rotation_.z), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, scale_);

	model_ = model;

}

// ------------------------------------------------------------------------- //