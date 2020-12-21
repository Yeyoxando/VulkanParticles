/*
 *	Author: Diego Ochando Torres
 *  Date: 21/12/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

#include "input.h"

InputState* InputManager::input_state_ = nullptr;

 // ------------------------------------------------------------------------- // 

InputManager::InputManager() {

  if (input_state_ == nullptr)
    input_state_ = new InputState();

  input_state_->window_ref_ = nullptr;

}

// ------------------------------------------------------------------------- // 

InputManager::~InputManager() {

  delete input_state_;

}

// ------------------------------------------------------------------------- // 

void button_click_callback(GLFWwindow* window, int button, int action, int mods) {

  if (button == GLFW_MOUSE_BUTTON_RIGHT) InputManager::getState()->right_click_action_ = action;
  if (button == GLFW_MOUSE_BUTTON_LEFT) InputManager::getState()->left_click_action_ = action;

}

// ------------------------------------------------------------------------- // 

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

  if (key == GLFW_KEY_ESCAPE) InputManager::getState()->escape_key_action = action;
  if (key == GLFW_KEY_W) InputManager::getState()->w_key_action = action;
  if (key == GLFW_KEY_S) InputManager::getState()->s_key_action = action;

}

// ------------------------------------------------------------------------- // 

void scroll_callback(GLFWwindow* window, double x_offset, double y_offset) {

  InputManager::getState()->wheel_offset = y_offset;

}

// ------------------------------------------------------------------------- // 

void InputManager::init(GLFWwindow* window) {

  input_state_->window_ref_ = window;

  glfwSetMouseButtonCallback(window, button_click_callback);
  glfwSetKeyCallback(window, key_callback);
  glfwSetScrollCallback(window, scroll_callback);

}

// ------------------------------------------------------------------------- // 

InputState* InputManager::getState() {

  return input_state_;

}

// ------------------------------------------------------------------------- // 

glm::vec2 InputManager::getMousePos() {

  double mx, my;
  glfwGetCursorPos(input_state_->window_ref_, &mx, &my);

  return glm::vec2((float)mx, (float)my);

}

// ------------------------------------------------------------------------- // 
