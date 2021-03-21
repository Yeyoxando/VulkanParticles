/*
 *	Author: Diego Ochando Torres
 *  Date: 30/12/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

#ifndef __PARTICLE_EDITOR_H__
#define __PARTICLE_EDITOR_H__

// ------------------------------------------------------------------------- // 

#include "input.h"
#include "camera.h"
#include "scene.h"

// ------------------------------------------------------------------------- //

struct AppSettings {
  // This will be send as parameter to the app to initialize certain values
  int max_entities;
  int max_particles;
  int max_objects;

  // Global settings

  bool use_msaa;
};

// ------------------------------------------------------------------------- //

class ParticleEditor {
public:
	// Main base app instance, use it to run the editor
	static ParticleEditor& instance();

  // Correspond to internal vertex and index buffers for these geometries
	enum DefaultMesh {
		kDefaultMesh_Quad = 0,
		kDefaultMesh_Cube = 1,
		// ...
	};

  // Correspond to internal material which have a pipeline and a descriptor set associated
  enum MaterialParent {
    kMaterialParent_Opaque = 0,
    kMaterialParent_Translucent = 1,
  }; 



  // set a scene to run
  void loadScene(Scene* scene);
  // call this method to start the editor after setting a scene
  void run();



  // returns the current camera
	Camera* getCamera();
	// returns the current scene
	Scene* getScene();

private:
  ParticleEditor();
  ~ParticleEditor();

  void init();
  void shutDown();

  void input();
  void update();
  void render();



  // Input manager for keyboard and mouse inputs
  InputManager* input_;
  // Camera to travel around scene or particle edition
  Camera* camera_;

  // The scene which is loaded to run on the editor
  Scene* active_scene_;



  // Internal data and functions to manage everything hiding it from the user
  struct AppData;
  AppData* app_data_;


  // Friends to access app_data
	friend class Camera;
	friend class Material;
	friend class OpaqueMaterial;
	friend class TranslucentMaterial;
	friend class ComponentMesh;
	friend class ComponentMaterial;
	friend class SystemDrawObjects;

};

// ------------------------------------------------------------------------- //


 /* Notes
 Other classes to create

Editor
 - ImGUI
 - Scene mode and editor mode

 Commands? maybe some encapsulation with VK commands to do multiple at the same time, like draw a game object or draw the sky box or draw a particle
*/

#endif //__PARTICLE_EDITOR_H__