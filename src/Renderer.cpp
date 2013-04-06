#include <Renderer.h>

#include <stdexcept>
#include <cstdio>

Renderer* Renderer::m_singleton = 0;

Renderer::Renderer(int width, int height, Landscape* l)
	: m_width(width), m_height(height), m_landscape(l)
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
}

void Renderer::Render()
{
	if (!glfwOpenWindow(m_width, m_height, 8, 8, 8, 0, 24, 0, GLFW_WINDOW))
		throw std::runtime_error("Unable to initialize glfw");

	if (glewInit() != GLEW_OK)
		throw std::runtime_error("Unable to initialize glew");

	glfwSetWindowSizeCallback(Renderer::WindowResize);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glTranslatef(0.0f, 0.0f, -100.0f);

	bool running = true;

	while (running)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glLoadIdentity();

		glPushMatrix();
		glColor3f(1.0f, 1.0f, 1.0f);
		m_landscape->Render();
		glPopMatrix();
		
		glfwSwapBuffers();

		running = glfwGetWindowParam(GLFW_OPENED);
	}

}
