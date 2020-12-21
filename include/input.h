/*
 *	Author: Diego Ochando Torres
 *  Date: 21/12/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

#ifndef _INPUT_MANAGER_H__
#define _INPUT_MANAGER_H__

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

struct InputState {

  int right_click_action_;
  int left_click_action_;
  double wheel_offset;

  int escape_key_action;
  int w_key_action;
  int s_key_action;

  GLFWwindow* window_ref_;

};

class InputManager {
public:
  InputManager();
  ~InputManager();

  void init(GLFWwindow* window);

  static InputState* getState();
  glm::vec2 getMousePos();
  
  // int getKeyState(Keycode) returns an int of the key state
  // int getMouseButton(Keycode) returns an int of the button state

private:

  static InputState* input_state_;

};

#endif // _INPUT_MANAGER_H__