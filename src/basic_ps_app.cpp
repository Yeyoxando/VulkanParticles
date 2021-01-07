/*
 *	Author: Diego Ochando Torres
 *  Date: 30/12/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

// ------------------------------------------------------------------------- //

#include "basic_ps_app.h"
#include "internal/internal_app_data.h"

// ------------------------------------------------------------------------- //

BasicPSApp::BasicPSApp() {

  app_data_ = new AppData();

  input_ = new InputManager();
  camera_ = new Camera();

}

// ------------------------------------------------------------------------- //

BasicPSApp::~BasicPSApp() {

}

// ------------------------------------------------------------------------- //

void BasicPSApp::run() {

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

Camera* BasicPSApp::getCamera() {

  return camera_;

}

// ------------------------------------------------------------------------- //

void BasicPSApp::init() {

  app_data_->initWindow();
  app_data_->initVulkan();

  input_->init(app_data_->window_);

}

// ------------------------------------------------------------------------- //

void BasicPSApp::shutDown() {

  app_data_->renderLoopEnd();
  app_data_->closeVulkan();

  delete camera_;
  delete input_;

  delete app_data_;

}

// ------------------------------------------------------------------------- //

BasicPSApp& BasicPSApp::instance() {

  static BasicPSApp* instance = new BasicPSApp();
  return *instance;

}

// ------------------------------------------------------------------------- //

void BasicPSApp::input() {

  glfwPollEvents();

  // Camera
  if (input_->getMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
    camera_->updateViewMatrix(input_->getMousePos());
    glfwSetInputMode(app_data_->window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwGetMouseButton(app_data_->window_, GLFW_MOUSE_BUTTON_RIGHT);
  }
  if (!input_->getMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
    camera_->setRotating(false);
    glfwSetInputMode(app_data_->window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }
  if (input_->getMouseScroll() > 0.05f || input_->getMouseScroll() < -0.05f) {
    camera_->moveFront(input_->getState()->wheel_offset * 0.05f);
    input_->getState()->wheel_offset = 0.0f;
  }

  // Close window
  if (input_->getKeyPressed(GLFW_KEY_ESCAPE)) {
    app_data_->close_window_ = true;
  }

}

// ------------------------------------------------------------------------- //

void BasicPSApp::update() {

  app_data_->updateFrame();

}

// ------------------------------------------------------------------------- //

void BasicPSApp::render() {

  app_data_->drawFrame();

}

// ------------------------------------------------------------------------- //
