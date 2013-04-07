#include <Camera.h>

#include <GL/glew.h>
#include <GL/glfw.h>
#include <cstdio>
#include <cmath>

#define PI 3.14159265f
#define PI_RAD (PI/180.0f)

Camera::Camera()
{
	m_position = MakeVec3f(0.0f, 0.0f, 2.0f);
	m_viewDirection = MakeVec3f(0.0f, 0.0f, -1.0f);
	m_rightVector = MakeVec3f(1.0f, 0.0f, 0.0f);
	m_upVector = MakeVec3f(0.0f, 1.0f, 0.0f);
	m_rotation = MakeVec3f(0.0f, 0.0f, 0.0f);
}

Camera::~Camera()
{

}

void Camera::Rotate(tfVec3f amount)
{
	m_rotation.x += amount.x;
	m_viewDirection = Vec3fNormalize(Vec3fAdd(Vec3fMul(m_viewDirection, cos(amount.x*PI_RAD)), Vec3fMul(m_upVector, sin(amount.x*PI_RAD))));
	m_upVector = Vec3fMul(Vec3fCross(m_viewDirection, m_rightVector), -1);

	m_rotation.y += amount.y;
	m_viewDirection = Vec3fNormalize(Vec3fSub(Vec3fMul(m_viewDirection, cos(amount.y*PI_RAD)), Vec3fMul(m_rightVector, sin(amount.y*PI_RAD))));
	m_rightVector = Vec3fCross(m_viewDirection, m_upVector);

	m_rotation.z += amount.z;
	m_rightVector = Vec3fNormalize(Vec3fAdd(Vec3fMul(m_rightVector, cos(amount.z*PI_RAD)), Vec3fMul(m_upVector, sin(amount.z*PI_RAD))));
	m_upVector = Vec3fMul(Vec3fCross(m_viewDirection, m_rightVector), -1);
}

void Camera::Move(tfVec3f amt)
{
	m_position = Vec3fAdd(m_position, Vec3fMul(m_viewDirection, -amt.x));
	m_position = Vec3fAdd(m_position, Vec3fMul(m_rightVector, amt.y));
	m_position = Vec3fAdd(m_position, Vec3fMul(m_upVector, amt.z));
}

void Camera::Apply()
{
	tfVec3f view = Vec3fAdd(m_position, m_viewDirection);

	gluLookAt(m_position.x, m_position.y, m_position.z, view.x, view.y, view.z, m_upVector.x, m_upVector.y, m_upVector.z);
}
