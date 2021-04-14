/*
 *	Author: Diego Ochando Torres
 *  Date: 30/12/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

#ifndef __PARTICLE_EDITOR_H__
#define __PARTICLE_EDITOR_H__

// ------------------------------------------------------------------------- // 

#include "engine/input.h"
#include "engine/camera.h"
#include "engine/scene.h"

// ------------------------------------------------------------------------- //

// This will be send as parameter to the app to initialize certain values
/*struct AppSettings {
  int max_entities;
  int max_particles;
  int max_lights;

  // Global settings
  bool use_msaa;
  // etc

};*/

// ------------------------------------------------------------------------- //

class ParticleEditor {
public:
	/// @brief Editor running instance, use it to run the program (Singleton pattern).
	static ParticleEditor& instance();

  /// @brief Corresponds to internal vertex and index buffers for the defined geometries.
	enum DefaultMesh {
		kDefaultMesh_Quad = 0,
		kDefaultMesh_Cube = 1,
		// ...
	};

  /// @brief Corresponds to internal material parents that contains GPU resources.
  enum MaterialParent {
    kMaterialParent_Opaque = 0,
    kMaterialParent_Translucent = 1,
    kMaterialParent_Particles = 2,
  }; 



  /// @brief Set a scene to run in the editor.
  void loadScene(Scene* scene);
  /// @brief This method has to be called to start the editor after setting a scene.
  void run();



  /// @return Current camera.
	Camera* getCamera();
  /// @return Current scene running.
	Scene* getScene();

private:
  ParticleEditor();
  ~ParticleEditor();

  /// @brief Starts the editor internal functions.
  void init();
  /// @brief Shuts down the editor internal functions and close it.
  void shutDown();

  void input();
  void update();
  void render();


  /// @brief Input manager for keyboard and mouse input.
  InputManager* input_;
  /// @brief Camera to travel around a scene or in the particle editor view.
  Camera* camera_;
  /// @brief The scene which is loaded to run on the editor.
  Scene* active_scene_;

  /// @brief Internal data and functions to manage internal APIs resources hiding them from the user.
  struct AppData;
  AppData* app_data_;


  // Friend classes that can access to AppData
	friend class Camera;
	
  friend class Material;
	friend class OpaqueMaterial;
	friend class TranslucentMaterial;
	friend class ParticlesMaterial;

	friend class ComponentMesh;
	friend class ComponentMaterial;
	friend class ComponentParticleSystem;
	friend class SystemDrawObjects;
	friend class SystemDrawTranslucents;
	friend class SystemDrawParticles;

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