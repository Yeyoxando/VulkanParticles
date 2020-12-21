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

  void moveFront(float offset);
  void rotateYOrbital();

  void updateViewMatrix();
  void updateViewMatrix(glm::vec2 new_mouse_pos);

  void setRotating(bool is_rotating);

  glm::mat4x4 getViewMatrix();

private:
  float distance_to_center_;
  glm::vec3 position_;
  glm::vec3 up_;
  glm::vec3 target_;
  
  glm::mat4x4 view_;

  glm::vec2 last_mouse_pos_;
  float dist_x;
  float dist_y;
  float camera_distance_;
  bool is_rotating_;
  //glm::mat4x4 projection_matrix_;

};

#endif // __CAMERA_H__