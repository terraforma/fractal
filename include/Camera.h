#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <libterra/Vector.h>

using namespace LibTerra;

#define PI 3.141592654f

class Camera
{
public:
    Camera();
    virtual ~Camera();

	void Move(tfVec3f amount);
	void Rotate(tfVec3f amount);

	void Apply();
private:
	tfVec3f m_viewDirection;
	tfVec3f m_rightVector;
	tfVec3f m_upVector;
	tfVec3f m_position;
	tfVec3f m_rotation;
};

#endif /* _CAMERA_H_ */
