#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <Landscape.h>
#include <Camera.h>

#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/glfw.h>

class Renderer 
{
public:
	Renderer(int width, int height, Landscape* l);
	virtual ~Renderer();

	GLFWthread StartThread();
	void Render();
	static void WindowResize(int width, int height);
private:
	int m_width;
	int m_height;
	Landscape* m_landscape;
	Camera m_camera;

	static Renderer* m_singleton;
};

#endif /* _RENDERER_H_ */
