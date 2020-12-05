/*
 *	Author: Diego Ochando Torres
 *  Date: 25/11/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */
 
// ------------------------------------------------------------------------- // 

#include "common_def.h"
#ifdef MAIN_HELLO_TRIANGLE

#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include "hello_triangle_app.h"

int main(){

  HelloTriangleApp hello_triangle;

  try {
    hello_triangle.run();
  }
  catch(const std::exception& e) {
    printf(e.what());
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
  
}

#endif