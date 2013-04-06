#include <Renderer.h>

#include <stdexcept>

int main(int _argc, char *_argv[])
{
	Landscape l(_argv[1], _argv[2], _argv[3], _argv[4]);
	l.Build();
	Renderer r(800, 600, &l);
	GLFWthread renderThread;
	if ((renderThread = r.StartThread()) < 0) 
	{
		throw std::runtime_error("unable to create thread");
	}
	glfwWaitThread(renderThread, GLFW_WAIT);
	return 0;
}
