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
	if (!ShaderCompiled(m_vertProgId))
	{
		printf("Vertex shader compile failed:\n");
		DumpLog(m_vertProgId);
	}
	glCompileShader(m_fragProgId);
	if (!ShaderCompiled(m_fragProgId))
	{
		printf("Fragment shader compile failed:\n");
		DumpLog(m_fragProgId);
	}

	m_id = glCreateProgram();
	glAttachShader(m_id, m_vertProgId);
	glAttachShader(m_id, m_fragProgId);
	glLinkProgram(m_id);
	if (!ProgLinked(m_id))
	{
		DumpLog(m_id);
	}
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

bool Shader::ShaderCompiled(unsigned int id)
{
	int compiled;
	glGetObjectParameterivARB(id, GL_COMPILE_STATUS, &compiled);
	return compiled;
}

bool Shader::ProgLinked(unsigned int id)
{
	int linked;
	glGetObjectParameterivARB(id, GL_LINK_STATUS, &linked);
	return linked;
}

void Shader::DumpLog(unsigned int id)
{
	int bufferLen = 0;
	int len = 0;

	glGetShaderiv(id, GL_INFO_LOG_LENGTH , &bufferLen);
	if (bufferLen > 1)
	{
		char* compileLog = new char[bufferLen];
		glGetInfoLogARB(id, bufferLen, &len, compileLog);
		printf("Compile log: %s\n", compileLog);
		delete[] compileLog;
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
