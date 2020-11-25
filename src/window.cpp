/*
 *	Author: Diego Ochando Torres
 *  Date: 25/11/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

// ------------------------------------------------------------------------- // 

#include "window.h"

// ------------------------------------------------------------------------- // 

Window::Window() {

  window = nullptr;
  kWindowWidth = 0;
  kWindowHeight = 0;

}

// ------------------------------------------------------------------------- // 

Window::~Window() {

}

// ------------------------------------------------------------------------- // 

void Window::init(int width /*= 800*/, int height /*= 600*/) {

  glfwInit();

  // Tell GLFW to not use OpenGL API
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  // No resize allowed for now
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  window = glfwCreateWindow(width, height, "Vulkan Particles", nullptr, nullptr);
  kWindowWidth = width;
  kWindowHeight = height;

}

// ------------------------------------------------------------------------- // 

bool Window::shouldClose() {

  return glfwWindowShouldClose(window);

}

// ------------------------------------------------------------------------- // 

void Window::pollEvents() {

  glfwPollEvents();

}

// ------------------------------------------------------------------------- // 

void Window::close() {

  glfwDestroyWindow(window);
  glfwTerminate();

}

// ------------------------------------------------------------------------- // 
