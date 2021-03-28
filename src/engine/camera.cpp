/*
 *	Author: Diego Ochando Torres
 *  Date: 21/12/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

#include "engine/camera.h"
#include "particle_editor.h"
#include "../src/engine_internal/internal_app_data.h"

#include <glm/gtc/matrix_transform.hpp>
#include <math.h>

 // ------------------------------------------------------------------------- // 

Camera::Camera() {

  position_ = glm::vec3(0.0f, 0.0f, -2.0f);
  rotation_ = glm::vec3(-45.0f, 0.0f, -90.0f);
  view_pos_ = glm::vec4();

  last_mouse_pos_ = glm::vec2(-1.0f, -1.0f);
  is_moving_ = false;

  updateViewMatrix();

}

// ------------------------------------------------------------------------- // 

Camera::~Camera() {



}

// ------------------------------------------------------------------------- // 

void Camera::setupProjection(float fov_degrees, float aspect_ratio, float cam_near, float cam_far){

	projection_ = glm::perspective(glm::radians(fov_degrees), aspect_ratio, cam_near, cam_far);

	// Invert clip Y due to GLM works with OpenGL and its inverted
	projection_[1].y *= -1;

	updateViewMatrix();

}

// ------------------------------------------------------------------------- // 

void Camera::updateViewMatrix() {

  glm::mat4 rot_mat = glm::mat4(1.0f);
  glm::mat4 trans_mat;

  rot_mat = glm::translate(rot_mat, position_);
  rot_mat = glm::rotate(rot_mat, glm::radians(rotation_.x), glm::vec3(1.0f, 0.0f, 0.0f));
  rot_mat = glm::rotate(rot_mat, glm::radians(rotation_.y), glm::vec3(0.0f, 1.0f, 0.0f));
  rot_mat = glm::rotate(rot_mat, glm::radians(rotation_.z), glm::vec3(0.0f, 0.0f, 1.0f));

  //trans_mat = glm::translate(glm::mat4(1.0f), position_);

  //view_ = trans_mat * rot_mat ;

  view_pos_ = glm::vec4(position_, 0.0f) * glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f);


  ParticleEditor::AppData* app_data = ParticleEditor::instance().app_data_;

	// Fill the ubos with the updated data
  auto materials = app_data->materials_;
  for (int i = 0; i < materials.size(); ++i) {
    materials[i]->scene_ubo_.view = rot_mat;
    materials[i]->scene_ubo_.projection = projection_;
  }

}

// ------------------------------------------------------------------------- // 

void Camera::updateViewMatrix(glm::vec2 new_mouse_pos) {

  if (!is_moving_) {
    last_mouse_pos_ = new_mouse_pos;
    is_moving_ = true;
    return;
  }

	float dist_x_ = last_mouse_pos_.x - new_mouse_pos.x;
	float dist_y_ = last_mouse_pos_.y - new_mouse_pos.y;

  rotation_ += glm::vec3(-dist_y_, 0.0f, -dist_x_);
  

	glm::mat4 rot_mat = glm::mat4(1.0f);
	glm::mat4 trans_mat;

  rot_mat = glm::translate(rot_mat, position_);
	rot_mat = glm::rotate(rot_mat, glm::radians(rotation_.x), glm::vec3(1.0f, 0.0f, 0.0f));
	rot_mat = glm::rotate(rot_mat, glm::radians(rotation_.y), glm::vec3(0.0f, 1.0f, 0.0f));
	rot_mat = glm::rotate(rot_mat, glm::radians(rotation_.z), glm::vec3(0.0f, 0.0f, 1.0f));


  //trans_mat = glm::translate(glm::mat4(1.0f), position_);

  //view_ = trans_mat * rot_mat ;

	view_pos_ = glm::vec4(position_, 0.0f) * glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f);


	last_mouse_pos_ = new_mouse_pos;


	ParticleEditor::AppData* app_data = ParticleEditor::instance().app_data_;
	// Fill the ubos with the updated data
	auto materials = app_data->materials_;
	for (int i = 0; i < materials.size(); ++i) {
		materials[i]->scene_ubo_.view = rot_mat;
		materials[i]->scene_ubo_.projection = projection_;
	}

}

// ------------------------------------------------------------------------- // 

glm::mat4x4 Camera::getViewMatrix() {

  return view_;

}

// ------------------------------------------------------------------------- // 

glm::mat4 Camera::getProjectionMatrix() {

	return projection_;

}

// ------------------------------------------------------------------------- // 

void Camera::zoom(float wheel_offset){

  glm::vec3 front_ = glm::vec3(0.0f, 0.0f, 0.0f) - position_;

  position_ += front_ * wheel_offset;

  updateViewMatrix();

}

// ------------------------------------------------------------------------- // 
