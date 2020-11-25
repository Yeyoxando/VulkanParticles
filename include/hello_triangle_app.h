/*
 *	Author: Diego Ochando Torres
 *  Date: 25/11/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

 // ------------------------------------------------------------------------- // 

#ifndef __HELLO_TRIANGLE_APP_H__
#define __HELLO_TRIANGLE_APP_H__

#include "window.h"

class HelloTriangleApp {
public:
  HelloTriangleApp();
  ~HelloTriangleApp();

	// Executes the application
	void run();

private:

	// Allocates all the necessary vulkan resources
	void init();

	// Executes the main render loop
	void renderLoop();

	// Frees all the allocated resources and close the app
	void close();

	// Variables
	Window window;

};

#endif