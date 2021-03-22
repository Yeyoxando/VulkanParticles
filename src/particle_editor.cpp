/*
 *	Author: Diego Ochando Torres
 *  Date: 30/12/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

// ------------------------------------------------------------------------- //

#include "particle_editor.h"
#include "engine_internal/internal_app_data.h"

// ------------------------------------------------------------------------- //

ParticleEditor::ParticleEditor() {

  app_data_ = new AppData();

  input_ = nullptr;
  camera_ = nullptr;

  active_scene_ = nullptr;

}

// ------------------------------------------------------------------------- //

ParticleEditor::~ParticleEditor() {



}

// ------------------------------------------------------------------------- //

void ParticleEditor::run() {

  init();

  // Render Loop
  while (!glfwWindowShouldClose(app_data_->window_) && !app_data_->close_window_) {
    input();
    update();
    render();
  }

  shutDown();

}

// ------------------------------------------------------------------------- //

Camera* ParticleEditor::getCamera() {

  return camera_;

}

// ------------------------------------------------------------------------- //

Scene* ParticleEditor::getScene(){

  return active_scene_;

}

// ------------------------------------------------------------------------- //

void ParticleEditor::init() {

	input_ = new InputManager();
	camera_ = new Camera();

	if (active_scene_ == nullptr) {
		throw std::runtime_error("\n A scene has not been set to run.");
	}
	active_scene_->init();

  app_data_->initWindow();
  app_data_->initVulkan();

  input_->init(app_data_->window_);
	camera_->setupProjection(90.0f, (float)app_data_->window_width_ / (float)app_data_->window_height_, 0.1f, 10.0f);


}

// ------------------------------------------------------------------------- //

void ParticleEditor::shutDown() {
  
  app_data_->renderLoopEnd();

  delete active_scene_;
  app_data_->closeVulkan();


  delete camera_;
  delete input_;

  delete app_data_;

}

// ------------------------------------------------------------------------- //

ParticleEditor& ParticleEditor::instance() {

  static ParticleEditor* instance = new ParticleEditor();
  return *instance;

}

// ------------------------------------------------------------------------- //

void ParticleEditor::loadScene(Scene* scene){

  active_scene_ = scene;

}

// ------------------------------------------------------------------------- //

void ParticleEditor::input() {

  glfwPollEvents();

  // Camera
  if (input_->getMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
    camera_->updateViewMatrix(input_->getMousePos());
    glfwSetInputMode(app_data_->window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwGetMouseButton(app_data_->window_, GLFW_MOUSE_BUTTON_RIGHT);
  }
  if (!input_->getMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
    camera_->finishMoving();
    glfwSetInputMode(app_data_->window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }
  if (input_->getMouseScroll() > 0.05f || input_->getMouseScroll() < -0.05f) {
    camera_->zoom(input_->getState()->wheel_offset * 0.05f);
    input_->getState()->wheel_offset = 0.0f;
  }

  // Close window
  if (input_->getKeyPressed(GLFW_KEY_ESCAPE)) {
    app_data_->close_window_ = true;
  }

}

// ------------------------------------------------------------------------- //

void ParticleEditor::update() {

  active_scene_->update();
  app_data_->updateFrame();

}

// ------------------------------------------------------------------------- //

void ParticleEditor::render() {

  app_data_->drawFrame();

}

// ------------------------------------------------------------------------- //
