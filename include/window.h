/*
 *	Author: Diego Ochando Torres
 *  Date: 25/11/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

 // ------------------------------------------------------------------------- // 

#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "GLFW/glfw3.h"

class Window {
public:
  Window();
  ~Window();

  // Initializes GLFW and creates a window
  void init(int width = 800, int height = 600);

  bool shouldClose();

  void pollEvents();

  void close();

private:
  GLFWwindow* window;

  int kWindowWidth;
  int kWindowHeight;

};

#endif
