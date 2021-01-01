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

  void run();

  void init();
  void shutDown();

  void input();
  void update();
  void render();

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


 /* Notes Other classes to create

Scene

Object?
 - Different to particles, to represent a model or another geometry with a simple material
 - ...

Editor
 - ImGUI
 - Scene mode and editor mode

 Commands? maybe some encapsulation with VK commands to do multiple at the same time, like draw a game object or draw the sky box or draw a particle
*/

#endif //__BASIC_PS_APP_H__