/*
 *	Author: Diego Ochando Torres
 *  Date: 30/12/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

#ifndef __BASIC_PS_APP_H__
#define __BASIC_PS_APP_H__

// ------------------------------------------------------------------------- // 

#include "render_manager.h"
#include "resource_manager.h"

// ------------------------------------------------------------------------- //

class BasicPSApp {
public:
  BasicPSApp();
  ~BasicPSApp();

  // Main base app as a example of advanced and structured features
  // Will have a reference to render and resource manager which will have some vulkan features in them.
  // The rest of the Vulkan features that doesn't fit in a manager will be here for now.

  void init();
  void shutDown();

private:
  RenderManager* render_manager_;
  ResourceManager* resource_manager_;
  // Other managers
  // Vulkan things -> struct AppData? with internal.h

  // Particle system things
  // Scene things
  // Camera things
  // Other logic things

};

// ------------------------------------------------------------------------- //

#endif //__BASIC_PS_APP_H__