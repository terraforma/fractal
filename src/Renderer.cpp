#include <Renderer.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
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

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glm::mat4 projMatrix = glm::perspective(60.0f, float(width/height), 0.1f, 100.0f);
	glLoadMatrixf(glm::value_ptr(projMatrix));
	
	glMatrixMode(GL_MODELVIEW);
}

void Renderer::Render()
{
	if (!glfwOpenWindow(m_width, m_height, 8, 8, 8, 0, 24, 0, GLFW_WINDOW))
		throw std::runtime_error("Unable to initialize glfw");

	if (glewInit() != GLEW_OK)
		throw std::runtime_error("Unable to initialize glew");

	m_landscape->Build();

	glfwSetWindowSizeCallback(Renderer::WindowResize);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable (GL_DEPTH_TEST);
	glShadeModel (GL_SMOOTH);

	glm::vec4 lightPos(0.0f, 0.0f, 1.0f, 0.0f);

	bool running = true;

	while (running)
	{
		// Poll input
		// Movement
		glm::vec3 movement(0.0f, 0.0f, 0.0f);
		const float moveAmount = 0.01f;
		if (glfwGetKey('W') == GLFW_PRESS) {
			movement.x -= moveAmount;
		}
		if (glfwGetKey('S') == GLFW_PRESS) {
			movement.x += moveAmount;
		}
		if (glfwGetKey('A') == GLFW_PRESS) {
			movement.y -= moveAmount;
		}
		if (glfwGetKey('D') == GLFW_PRESS) {
			movement.y += moveAmount;
		}
		if (glfwGetKey('Q') == GLFW_PRESS) {
			movement.z -= moveAmount;
		}
		if (glfwGetKey('E') == GLFW_PRESS) {
			movement.z += moveAmount;
		}
		m_camera.Move(movement);
		// Rotation
		glm::vec3 rotation(0.0f, 0.0f, 0.0f);
		const float rotateAmount = 0.7f;
		if (glfwGetKey(GLFW_KEY_UP) == GLFW_PRESS) {
			rotation.x += rotateAmount;
		}
		if (glfwGetKey(GLFW_KEY_DOWN) == GLFW_PRESS) {
			rotation.x -= rotateAmount;
		}
		if (glfwGetKey(GLFW_KEY_LEFT) == GLFW_PRESS) {
			rotation.y += rotateAmount;
		}
		if (glfwGetKey(GLFW_KEY_RIGHT) == GLFW_PRESS) {
			rotation.y -= rotateAmount;
		}
		if (glfwGetKey('Z') == GLFW_PRESS) {
			rotation.z += rotateAmount;
		}
		if (glfwGetKey('C') == GLFW_PRESS) {
			rotation.z -= rotateAmount;
		}
		
		m_camera.Rotate(rotation);

		// Render
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		glm::mat4 viewMatrix = m_camera.Apply();
		glLoadMatrixf(glm::value_ptr(viewMatrix));

		// Calculate light position
		glm::vec4 adjustedLightPos = viewMatrix * lightPos;

		glColor3f(1.0f, 1.0f, 1.0f);
		m_landscape->Render(adjustedLightPos);
		
		glfwSwapBuffers();

		running = glfwGetWindowParam(GLFW_OPENED);
	}

}
