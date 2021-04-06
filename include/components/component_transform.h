/*
* Author: Diego Ochando Torres
* Date: 31/12/2020
* e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
*/

#ifndef __COMPONENT_TRANSFORM_H__
#define __COMPONENT_TRANSFORM_H__

// ------------------------------------------------------------------------- // 

#include "component.h"
#include "glm.hpp"

// ------------------------------------------------------------------------- //

class ComponentTransform : public Component {
public:
  ComponentTransform();

  void translate(glm::vec3 position);
  void rotate(glm::vec3 rotation_degrees);
  void scale(glm::vec3 scale);

  glm::vec3 getPosition();
  glm::vec3 getRotation();
  glm::vec3 getScale();
  glm::mat4 getModelMatrix();

protected:
  ~ComponentTransform();

  void computeModelMatrix();

  glm::vec3 position_;
  glm::vec3 rotation_;
  glm::vec3 scale_;

  glm::mat4 model_;

};

// ------------------------------------------------------------------------- //

#endif // __COMPONENT_TRANSFORM_H__
