#include <Camera.h>

#include <GL/glew.h>
#include <GL/glfw.h>
#include <cstdio>
#include <cmath>

Camera::Camera(tfVec3f position, tfVec3f rotation)
	: m_position(position), m_rotation(rotation)
{

}

Camera::~Camera()
{

}

void Camera::Rotate(tfVec3f rotation)
{
	m_rotation = Vec3fAdd(m_rotation, rotation);
}

void Camera::Translate(tfVec3f direction)
{
	m_position = Vec3fAdd(m_position, direction);
}

void Camera::Move(tfVec3f amount)
{
	tfVec3f movement;

    float xRad = (m_rotation.x / 180.0f * PI); 
    float yRad = (m_rotation.y / 180.0f * PI);
    float zRad = (m_rotation.z / 180.0f * PI); 

	// Do forward/backward movement
    movement = MakeVec3f(sin(yRad)*amount.x, -sin(xRad)*amount.x, -cos(yRad)*amount.x);
	m_position = Vec3fAdd(m_position, movement);

	// Do left/right movement
	movement = MakeVec3f(cos(yRad)*amount.y, 0.0f, sin(yRad)*amount.y);
	m_position = Vec3fAdd(m_position, movement);

	// Do up/down movement
}

void Camera::Apply()
{
	glRotatef(m_rotation.x, 1.0f, 0.0f, 0.0f);
	glRotatef(m_rotation.y, 0.0f, 1.0f, 0.0f);
	glRotatef(m_rotation.z, 0.0f, 0.0f, 1.0f);
	glTranslatef(-m_position.x, -m_position.y, -m_position.z);
}
