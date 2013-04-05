#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <GL/glfw.h>

class Renderer 
{
public:
	Renderer(int width, int height);
	virtual ~Renderer();

	GLFWthread StartThread();
	void Render();
private:
	int m_width;
	int m_height;
};

#endif /* _RENDERER_H_ */
