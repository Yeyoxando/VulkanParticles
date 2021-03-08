/*
 *	Author: Diego Ochando Torres
 *  Date: 21/12/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

 // ------------------------------------------------------------------------- // 

/*
 - Two modes, orbital for the editor and free move for the scene

*/

#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <glm/glm.hpp>

class Camera {
public:
  Camera();
  ~Camera();

  void updateViewMatrix();
  void updateViewMatrix(glm::vec2 new_mouse_pos);

  glm::mat4 getViewMatrix();

  void finishMoving() { is_moving_ = false; }

private:

  glm::vec3 position_;
  glm::vec3 rotation_;
  glm::vec4 view_pos_;
  
  glm::mat4 view_;
  //glm::mat4 projection_matrix_;

  glm::vec2 last_mouse_pos_;
  bool is_moving_;

};

#endif // __CAMERA_H__