/*
 *	Author: Diego Ochando Torres
 *  Date: 21/12/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

#ifndef __INPUT_MANAGER_H__
#define __INPUT_MANAGER_H__

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <map>

struct InputState {

  // Mouse
  uint8_t right_click_action;
  uint8_t left_click_action;
  float wheel_offset;

  // Maybe a map could be better (std::map<int, int> key_states)
  // Keyboard
  uint8_t escape_key_action;
  uint8_t zero_key_action;
  uint8_t one_key_action;
  uint8_t two_key_action;
  uint8_t three_key_action;
  uint8_t four_key_action;
  uint8_t five_key_action;
  uint8_t six_key_action;
  uint8_t seven_key_action;
  uint8_t eight_key_action;
  uint8_t nine_key_action;
  uint8_t a_key_action;
  uint8_t b_key_action;
  uint8_t c_key_action;
  uint8_t d_key_action;
  uint8_t e_key_action;
  uint8_t f_key_action;
  uint8_t g_key_action;
  uint8_t h_key_action;
  uint8_t i_key_action;
  uint8_t j_key_action;
  uint8_t k_key_action;
  uint8_t l_key_action;
  uint8_t m_key_action;
  uint8_t n_key_action;
  uint8_t o_key_action;
  uint8_t p_key_action;
  uint8_t q_key_action;
  uint8_t r_key_action;
  uint8_t s_key_action;
  uint8_t t_key_action;
  uint8_t u_key_action;
  uint8_t v_key_action;
  uint8_t w_key_action;
  uint8_t x_key_action;
  uint8_t y_key_action;
  uint8_t z_key_action;

};

class InputManager {
public:
  InputManager();
  ~InputManager();

  void init(GLFWwindow* window);

  static InputState* getState();
  glm::vec2 getMousePos();
  
  bool getMouseButtonPressed(int button);
  float getMouseScroll();

  bool getKeyPressed(int key_code);

private:

  static InputState* input_state_;

  GLFWwindow* window_ref_;

};

#endif // __INPUT_MANAGER_H__