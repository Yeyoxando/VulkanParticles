/*
 *	Author: Diego Ochando Torres
 *  Date: 25/11/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

// ------------------------------------------------------------------------- // 

#include "hello_triangle_app.h"

// ------------------------------------------------------------------------- // 

HelloTriangleApp::HelloTriangleApp() {

}

// ------------------------------------------------------------------------- // 

HelloTriangleApp::~HelloTriangleApp() {

}

// ------------------------------------------------------------------------- // 

void HelloTriangleApp::run() {

  window.init();

  init();
  renderLoop();
  close();

}

// ------------------------------------------------------------------------- // 

void HelloTriangleApp::init() {

}

// ------------------------------------------------------------------------- // 

void HelloTriangleApp::renderLoop() {

  while (!window.shouldClose()) {
    
    window.pollEvents();

    // Draw things

  }

}

// ------------------------------------------------------------------------- // 

void HelloTriangleApp::close() {

  // Other close things

  window.close();

}

// ------------------------------------------------------------------------- // 


