/*
 *	Author: Diego Ochando Torres
 *  Date: 30/12/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

#ifndef __BASIC_PS_APP_H__
#define __BASIC_PS_APP_H__

// ------------------------------------------------------------------------- // 

#include "input.h"
#include "camera.h"

// ------------------------------------------------------------------------- //

class BasicPSApp {
public:

  // Main base app instance
  static BasicPSApp& instance();

  void run();

  Camera* getCamera();

//   void loadModel(const char* model_path);
//   void loadTexture(const char* texture_path);

private:
  BasicPSApp();
  ~BasicPSApp();

  void init();
  void shutDown();

  void input();
  void update();
  void render();

  // Particle system things
  // Scene things
  // Other logic things

  InputManager* input_;
  Camera* camera_;

  struct AppData;
  AppData* app_data_;

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