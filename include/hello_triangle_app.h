/*
 *	Author: Diego Ochando Torres
 *  Date: 25/11/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

 // ------------------------------------------------------------------------- // 

#ifndef __HELLO_TRIANGLE_APP_H__
#define __HELLO_TRIANGLE_APP_H__

class HelloTriangleApp {
public:
  HelloTriangleApp();
  ~HelloTriangleApp();

	void run();

private:

	void init();

	void renderLoop();

	void close();

};

#endif