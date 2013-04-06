#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <libterra/Vector.h>

using namespace LibTerra;

#define PI 3.141592654f

class Camera
{
public:
    Camera(tfVec3f position, tfVec3f rotation);
    virtual ~Camera();

	void Rotate(tfVec3f rotation);
	void Translate(tfVec3f direction);

	void Move(tfVec3f amount);

	void Apply();
private:
	tfVec3f m_position;
	tfVec3f m_rotation;
};

#endif /* _CAMERA_H_ */
