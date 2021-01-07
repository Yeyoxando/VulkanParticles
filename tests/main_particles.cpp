/*
 *	Author: Diego Ochando Torres
 *  Date: 05/12/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */
 
// ------------------------------------------------------------------------- // 

#include "common_def.h"
#ifdef MAIN_BILLBOARDS

#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include "basic_ps_app.h"

int main(){

  try {
    BasicPSApp::instance().run();
  }
  catch(const std::exception& e) {
    printf(e.what());
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
  
}

#endif