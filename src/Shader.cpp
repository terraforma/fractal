#include <Shader.h>

#include <GL/glew.h>
#include <GL/glfw.h>
#include <fstream>
#include <sstream>

const char* LoadShader(const char* name)
{
	std::ifstream stream(name);
	std::stringstream buf;
	buf << stream.rdbuf();
	return buf.str().c_str();
}

Shader::Shader()
	: m_vertProgId(0), m_fragProgId(0), m_id(0)
{

}

void Shader::Init(const char* vertShader, const char* fragShader)
{
	m_vertProgId = glCreateShader(GL_VERTEX_SHADER);
	m_fragProgId = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(m_vertProgId, 1, &vertShader, 0);
	glShaderSource(m_fragProgId, 1, &fragShader, 0);

	glCompileShader(m_vertProgId);
	glCompileShader(m_fragProgId);

	m_id = glCreateProgram();
	glAttachShader(m_id, m_vertProgId);
	glAttachShader(m_id, m_fragProgId);
	glLinkProgram(m_id);
}

Shader::~Shader()
{
	if (m_id != 0) {
		glDetachShader(m_id, m_vertProgId);
		glDetachShader(m_id, m_fragProgId);

		glDeleteShader(m_vertProgId);
		glDeleteShader(m_fragProgId);
		glDeleteProgram(m_id);
	}
}

unsigned int Shader::Id()
{
	return m_id;
}

void Shader::Bind()
{
	glUseProgram(m_id);
}

void Shader::Unbind()
{
	glUseProgram(0);
}
