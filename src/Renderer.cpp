#include <Renderer.h>

#include <stdexcept>
#include <cstdio>

Renderer* Renderer::m_singleton = 0;

Renderer::Renderer(int width, int height, Landscape* l)
	: m_width(width), m_height(height), m_landscape(l), 
	  m_camera(MakeVec3f(0.0f, 0.0f, 2.0f), MakeVec3f(0.0f, 0.0f, 0.0f))
{
	m_singleton = this;

	if (!glfwInit())
		throw std::runtime_error("Unable to initialize glfw");	
}

Renderer::~Renderer()
{

}

void GLFWCALL StartRender(void* r)
{
	Renderer* renderer = (Renderer*)r;
	renderer->Render();
}

GLFWthread Renderer::StartThread()
{
	return glfwCreateThread(StartRender, this);
}

void Renderer::WindowResize(int width, int height)
{
	m_singleton->m_width = width;
	m_singleton->m_height = height;

	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, width/height, 1.0, 100.0);
	
	glMatrixMode(GL_MODELVIEW);
}

void Renderer::Render()
{
	if (!glfwOpenWindow(m_width, m_height, 8, 8, 8, 0, 24, 0, GLFW_WINDOW))
		throw std::runtime_error("Unable to initialize glfw");

	if (glewInit() != GLEW_OK)
		throw std::runtime_error("Unable to initialize glew");

	glfwSetWindowSizeCallback(Renderer::WindowResize);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable (GL_DEPTH_TEST);
	glEnable (GL_LIGHTING);
	glEnable (GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glShadeModel (GL_SMOOTH);

	bool running = true;

	while (running)
	{
		// Poll input
		// Movement
		tfVec3f movement = {0.0f, 0.0f, 0.0f};
		const float moveAmount = 0.1f;
		if (glfwGetKey('W') == GLFW_PRESS) {
			movement.x += moveAmount;
		}
		if (glfwGetKey('S') == GLFW_PRESS) {
			movement.x -= moveAmount;
		}
		if (glfwGetKey('A') == GLFW_PRESS) {
			movement.y -= moveAmount;
		}
		if (glfwGetKey('D') == GLFW_PRESS) {
			movement.y += moveAmount;
		}
		m_camera.Move(movement);
		// Rotation
		tfVec3f rotation = {0.0f, 0.0f, 0.0f};
		const float rotateAmount = 0.2f;
		if (glfwGetKey(GLFW_KEY_LEFT) == GLFW_PRESS) {
			rotation.y -= rotateAmount;
		}
		if (glfwGetKey(GLFW_KEY_RIGHT) == GLFW_PRESS) {
			rotation.y += rotateAmount;
		}
		if (glfwGetKey(GLFW_KEY_UP) == GLFW_PRESS) {
			rotation.x -= rotateAmount;
		}
		if (glfwGetKey(GLFW_KEY_DOWN) == GLFW_PRESS) {
			rotation.x += rotateAmount;
		}
		m_camera.Rotate(rotation);

		// Render
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		m_camera.Apply();

		glColor3f(1.0f, 1.0f, 1.0f);
		m_landscape->Render();
		
		glfwSwapBuffers();

		running = glfwGetWindowParam(GLFW_OPENED);
	}

}
