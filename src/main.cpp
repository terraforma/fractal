#include <Renderer.h>

#include <stdexcept>

int main(int _argc, char *_argv[])
{
	Renderer r(800, 600);
	GLFWthread renderThread;
	if ((renderThread = r.StartThread()) < 0) 
	{
		throw std::runtime_error("unable to create thread");
	}
	glfwWaitThread(renderThread, GLFW_WAIT);
	return 0;
}
