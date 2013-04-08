#include <Camera.h>

#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <GL/glfw.h>
#include <cstdio>
#include <cmath>

Camera::Camera()
{
	m_position = glm::vec3(0.0f, 0.0f, 2.0f);
	m_viewDirection = glm::vec3(0.0f, 0.0f, -1.0f);
	m_rightVector = glm::vec3(1.0f, 0.0f, 0.0f);
	m_upVector = glm::vec3(0.0f, 1.0f, 0.0f);
	m_rotation = glm::vec3(0.0f, 0.0f, 0.0f);
}

Camera::~Camera()
{

}

void Camera::Rotate(glm::vec3 amount)
{
	m_rotation.x += amount.x;
	m_viewDirection = glm::normalize(m_viewDirection * float(cos(amount.x*PI_RAD)) + m_upVector * float(sin(amount.x*PI_RAD)));
	m_upVector = glm::cross(m_viewDirection, m_rightVector) * -1.0f;

	m_rotation.y += amount.y;
	m_viewDirection = glm::normalize(m_viewDirection * float(cos(amount.y*PI_RAD)) - m_rightVector * float(sin(amount.y*PI_RAD)));
	m_rightVector = glm::cross(m_viewDirection, m_upVector);

	m_rotation.z += amount.z;
	m_rightVector = glm::normalize(m_rightVector * float(cos(amount.z*PI_RAD)) + m_upVector * float(sin(amount.z*PI_RAD)));
	m_upVector = glm::cross(m_viewDirection, m_rightVector) * -1.0f;
}

void Camera::Move(glm::vec3 amt)
{
	m_position = m_position + m_viewDirection * -amt.x;
	m_position = m_position + m_rightVector * amt.y;
	m_position = m_position + m_upVector * amt.z;
}

glm::mat4 Camera::Apply()
{
	glm::vec3 view = m_position + m_viewDirection;

	return glm::lookAt(m_position, view, m_upVector);
}
