/*
 *	Author: Diego Ochando Torres
 *  Date: 30/12/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

// ------------------------------------------------------------------------- //

#include "basic_ps_app.h"

// ------------------------------------------------------------------------- //

BasicPSApp::BasicPSApp() {

  render_manager_ = nullptr;
  resource_manager_ = nullptr;

}

// ------------------------------------------------------------------------- //

BasicPSApp::~BasicPSApp() {

}

// ------------------------------------------------------------------------- //

void BasicPSApp::init() {

  resource_manager_ = new ResourceManager();
  render_manager_ = new RenderManager();

  resource_manager_->init();
  render_manager_->init();

}

// ------------------------------------------------------------------------- //

void BasicPSApp::shutDown() {

  render_manager_->shutDown();
  resource_manager_->shutDown();

  delete render_manager_;
  delete resource_manager_;

}

// ------------------------------------------------------------------------- //
