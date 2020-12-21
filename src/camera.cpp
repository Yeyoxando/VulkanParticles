/*
 *	Author: Diego Ochando Torres
 *  Date: 21/12/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <math.h>

 // ------------------------------------------------------------------------- // 

Camera::Camera() {

  camera_distance_ = 1.0f;
  dist_x = 0.0f;
  dist_y = 2.0f;

  position_ = glm::vec3(camera_distance_ * cos(dist_x), 
    camera_distance_ * sin(dist_x), camera_distance_ * sin(dist_y));
  target_ = glm::vec3(0.0f, 0.0f, 0.0f);
  up_ = glm::vec3(0.0f, 0.0f, 1.0f);

  view_ = glm::lookAt(position_, target_, up_);

  last_mouse_pos_ = glm::vec2(400.0f, 300.0f);
  is_rotating_ = false;

}

// ------------------------------------------------------------------------- // 

Camera::~Camera() {



}

// ------------------------------------------------------------------------- // 

void Camera::moveFront(float offset) {

  camera_distance_ -= offset;
  updateViewMatrix();

}

// ------------------------------------------------------------------------- // 

void Camera::rotateYOrbital() {

}

// ------------------------------------------------------------------------- // 

void Camera::updateViewMatrix() {

  position_ = glm::vec3(camera_distance_ * cos(dist_x),
    camera_distance_ * sin(dist_x), camera_distance_ * sin(dist_y));
  
  view_ = glm::lookAt(position_, target_, up_);

}

// ------------------------------------------------------------------------- // 

void Camera::updateViewMatrix(glm::vec2 new_mouse_pos) {

  // position
  if (is_rotating_) {
    dist_x += (last_mouse_pos_.r - new_mouse_pos.r) * 0.001f;

    float diff = (last_mouse_pos_.g - new_mouse_pos.g) * 0.002f;
    // Limit movement in top and bottom
    if (sin(dist_y + diff) < 0.99f && sin(dist_y + diff) > -0.99f) {
      dist_y += diff;
    }

  }
  else {
    is_rotating_ = true;
  }

  position_ = glm::vec3(camera_distance_ * cos(dist_x), 
    camera_distance_ * sin(dist_x), camera_distance_ * sin(dist_y));
  last_mouse_pos_ = new_mouse_pos;

  view_ = glm::lookAt(position_, target_, up_);

}

// ------------------------------------------------------------------------- // 

void Camera::setRotating(bool is_rotating) {

  is_rotating_ = is_rotating;

}

// ------------------------------------------------------------------------- // 

glm::mat4x4 Camera::getViewMatrix() {

  return view_;

}

// ------------------------------------------------------------------------- // 
