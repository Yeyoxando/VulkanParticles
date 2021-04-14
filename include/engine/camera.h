/*
 *	Author: Diego Ochando Torres
 *  Date: 21/12/2020
 *  e-mail: c0022981@my.shu.ac.uk | yeyoxando@gmail.com
 */

#ifndef __CAMERA_H__
#define __CAMERA_H__

 // ------------------------------------------------------------------------- // 

#include <glm.hpp>

// ------------------------------------------------------------------------- // 

/**
* @brief Two modes will be available, orbital for the editor and free FPS move for the scene.
*			   For now only orbital with zooming is available.
*/
class Camera {
public:
	Camera();
	~Camera();

	void setupProjection(float fov_degrees, float aspect_ratio, float cam_near, float cam_far);

	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix();

	/// @brief Uses the wheel offset to zoom out/in
	void zoom(float wheeel_offset);
	void finishMoving() { is_moving_ = false; }

	/// @brief Uses the new mouse position and moves the camera depending on the offset
	void updateViewMatrix(glm::vec2 new_mouse_pos);

private:
	void updateViewMatrix();

	glm::vec3 position_;
	glm::vec3 rotation_;
	glm::vec4 view_pos_;

	glm::mat4 view_;
	glm::mat4 projection_;

	glm::vec2 last_mouse_pos_;
	bool is_moving_;

};

#endif // __CAMERA_H__