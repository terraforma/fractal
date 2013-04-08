#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <glm/glm.hpp>

#define PI 3.141592654f
#define PI_RAD (PI/180.0f)

class Camera
{
public:
    Camera();
    virtual ~Camera();

	void Move(glm::vec3 amount);
	void Rotate(glm::vec3 amount);

	glm::mat4 Apply();
private:
	glm::vec3 m_viewDirection;
	glm::vec3 m_rightVector;
	glm::vec3 m_upVector;
	glm::vec3 m_position;
	glm::vec3 m_rotation;
};

#endif /* _CAMERA_H_ */
