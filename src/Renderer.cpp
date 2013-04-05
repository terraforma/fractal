#include <Renderer.h>

#include <stdexcept>
#include <cstdio>

Renderer::Renderer(int width, int height)
	: m_width(width), m_height(height)
{
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

void Renderer::Render()
{
	if (!glfwOpenWindow(m_width, m_height, 8, 8, 8, 0, 24, 0, GLFW_WINDOW))
		throw std::runtime_error("Unable to initialize glfw");

	if (glewInit() != GLEW_OK)
		throw std::runtime_error("Unable to initialize glew");

	while (glfwGetWindowParam(GLFW_OPENED))
	{
		
		
		glfwSwapBuffers();
	}

}
