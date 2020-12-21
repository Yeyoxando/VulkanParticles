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

  window_ref_ = nullptr;

}

// ------------------------------------------------------------------------- // 

InputManager::~InputManager() {

  delete input_state_;

}

// ------------------------------------------------------------------------- // 

void button_click_callback(GLFWwindow* window, int button, int action, int mods) {

  if (button == GLFW_MOUSE_BUTTON_RIGHT) InputManager::getState()->right_click_action = (uint8_t)action;
  if (button == GLFW_MOUSE_BUTTON_LEFT) InputManager::getState()->left_click_action = (uint8_t)action;

}

// ------------------------------------------------------------------------- // 

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

  switch (key) {
  case GLFW_KEY_SPACE: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_APOSTROPHE: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_COMMA: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_MINUS: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_PERIOD: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_SLASH: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_0: {
    InputManager::getState()->zero_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_1: {
    InputManager::getState()->one_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_2: {
    InputManager::getState()->two_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_3: {
    InputManager::getState()->three_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_4: {
    InputManager::getState()->four_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_5: {
    InputManager::getState()->five_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_6: {
    InputManager::getState()->six_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_7: {
    InputManager::getState()->seven_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_8: {
    InputManager::getState()->eight_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_9: {
    InputManager::getState()->nine_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_SEMICOLON: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_EQUAL: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_A: {
    InputManager::getState()->a_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_B: {
    InputManager::getState()->b_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_C: {
    InputManager::getState()->c_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_D: {
    InputManager::getState()->d_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_E: {
    InputManager::getState()->e_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_F: {
    InputManager::getState()->f_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_G: {
    InputManager::getState()->g_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_H: {
    InputManager::getState()->h_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_I: {
    InputManager::getState()->i_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_J: {
    InputManager::getState()->j_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_K: {
    InputManager::getState()->k_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_L: {
    InputManager::getState()->l_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_M: {
    InputManager::getState()->m_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_N: {
    InputManager::getState()->n_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_O: {
    InputManager::getState()->o_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_P: {
    InputManager::getState()->p_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_Q: {
    InputManager::getState()->q_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_R: {
    InputManager::getState()->r_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_S: {
    InputManager::getState()->s_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_T: {
    InputManager::getState()->t_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_U: {
    InputManager::getState()->u_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_V: {
    InputManager::getState()->v_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_W: {
    InputManager::getState()->w_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_X: {
    InputManager::getState()->x_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_Y: {
    InputManager::getState()->y_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_Z: {
    InputManager::getState()->z_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_LEFT_BRACKET: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_BACKSLASH: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_RIGHT_BRACKET: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_GRAVE_ACCENT: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_WORLD_1: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_WORLD_2: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_ESCAPE: {
    InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_ENTER: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_TAB: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_BACKSPACE: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_INSERT: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_DELETE: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_RIGHT: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_LEFT: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_DOWN: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_UP: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_PAGE_UP: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_PAGE_DOWN: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_HOME: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_END: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_CAPS_LOCK: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_SCROLL_LOCK: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_NUM_LOCK: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_PRINT_SCREEN: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_PAUSE: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_F1: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_F2: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_F3: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_F4: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_F5: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_F6: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_F7: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_F8: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_F9: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_F10: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_F11: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_F12: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_KP_0: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_KP_1: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_KP_2: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_KP_3: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_KP_4: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_KP_5: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_KP_6: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_KP_7: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_KP_8: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_KP_9: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_KP_DECIMAL: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_KP_DIVIDE: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_KP_MULTIPLY: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_KP_SUBTRACT: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_KP_ADD: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_KP_ENTER: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_LEFT_SHIFT: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_LEFT_CONTROL: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_LEFT_ALT: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_LEFT_SUPER: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_RIGHT_SHIFT: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_RIGHT_CONTROL: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_RIGHT_ALT: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_RIGHT_SUPER: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  case GLFW_KEY_MENU: {
    //InputManager::getState()->escape_key_action = (uint8_t)action;
    break;
  }
  default: {
    break;
  }
  }

}

// ------------------------------------------------------------------------- // 

void scroll_callback(GLFWwindow* window, double x_offset, double y_offset) {

  InputManager::getState()->wheel_offset = (float)y_offset;

}

// ------------------------------------------------------------------------- // 

void InputManager::init(GLFWwindow* window) {

  window_ref_ = window;

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
  glfwGetCursorPos(window_ref_, &mx, &my);

  return glm::vec2((float)mx, (float)my);

}

// ------------------------------------------------------------------------- // 

bool InputManager::getKeyPressed(int key_code) {

  switch (key_code) {
  case GLFW_KEY_SPACE: {
    //return input_state_->escape_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_APOSTROPHE: {
    //return input_state_->escape_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_COMMA: {
    //return input_state_->escape_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_MINUS: {
    //return input_state_->escape_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_PERIOD: {
    //return input_state_->escape_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_SLASH: {
    //return input_state_->escape_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_0: {
    return input_state_->zero_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_1: {
    return input_state_->one_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_2: {
    return input_state_->two_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_3: {
    return input_state_->three_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_4: {
    return input_state_->four_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_5: {
    return input_state_->five_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_6: {
    return input_state_->six_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_7: {
    return input_state_->seven_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_8: {
    return input_state_->eight_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_9: {
    return input_state_->nine_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_SEMICOLON: {
    //return input_state_->escape_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_EQUAL: {
    //return input_state_->escape_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_A: {
    return input_state_->a_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_B: {
    return input_state_->b_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_C: {
    return input_state_->c_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_D: {
    return input_state_->d_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_E: {
    return input_state_->e_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_F: {
    return input_state_->f_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_G: {
    return input_state_->g_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_H: {
    return input_state_->h_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_I: {
    return input_state_->i_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_J: {
    return input_state_->j_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_K: {
    return input_state_->k_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_L: {
    return input_state_->l_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_M: {
    return input_state_->m_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_N: {
    return input_state_->n_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_O: {
    return input_state_->o_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_P: {
    return input_state_->p_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_Q: {
    return input_state_->q_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_R: {
    return input_state_->r_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_S: {
    return input_state_->s_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_T: {
    return input_state_->t_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_U: {
    return input_state_->u_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_V: {
    return input_state_->v_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_W: {
    return input_state_->w_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_X: {
    return input_state_->x_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_Y: {
    return input_state_->y_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_Z: {
    return input_state_->z_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_LEFT_BRACKET: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_BACKSLASH: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_RIGHT_BRACKET: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_GRAVE_ACCENT: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_WORLD_1: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_WORLD_2: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_ESCAPE: {
    return input_state_->escape_key_action == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_ENTER: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_TAB: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_BACKSPACE: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_INSERT: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_DELETE: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_RIGHT: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_LEFT: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_DOWN: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_UP: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_PAGE_UP: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_PAGE_DOWN: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_HOME: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_END: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_CAPS_LOCK: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_SCROLL_LOCK: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_NUM_LOCK: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_PRINT_SCREEN: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_PAUSE: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_F1: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_F2: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_F3: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_F4: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_F5: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_F6: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_F7: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_F8: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_F9: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_F10: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_F11: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_F12: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_KP_0: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_KP_1: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_KP_2: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_KP_3: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_KP_4: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_KP_5: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_KP_6: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_KP_7: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_KP_8: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_KP_9: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_KP_DECIMAL: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_KP_DIVIDE: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_KP_MULTIPLY: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_KP_SUBTRACT: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_KP_ADD: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_KP_ENTER: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_LEFT_SHIFT: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_LEFT_CONTROL: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_LEFT_ALT: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_LEFT_SUPER: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_RIGHT_SHIFT: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_RIGHT_CONTROL: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_RIGHT_ALT: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_RIGHT_SUPER: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  case GLFW_KEY_MENU: {
    //return input_state_->a_key_action_ == GLFW_PRESS;
    break;
  }
  default: {
    return false;
    break;
  }
  }

}

// ------------------------------------------------------------------------- // 

bool InputManager::getMouseButtonPressed(int button) {

  switch (button) {
  case GLFW_MOUSE_BUTTON_RIGHT: {
    return input_state_->right_click_action == GLFW_PRESS;
    break;
  }
  case GLFW_MOUSE_BUTTON_LEFT: {
    return input_state_->left_click_action == GLFW_PRESS;
    break;
  }
  default: {
    return false;
    break;
  }
  }

}

// ------------------------------------------------------------------------- // 

float InputManager::getMouseScroll() {

  return input_state_->wheel_offset;

}

// ------------------------------------------------------------------------- // 
