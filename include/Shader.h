#ifndef _SHADER_H_
#define _SHADER_H_

#include <string>

std::string LoadShader(const char* name);

class Shader
{
public:
    Shader();
    virtual ~Shader();

	void Init(std::string vertShader, std::string fragShader);
	unsigned int Id();
	void Bind();
	void Unbind();

private:
	bool ShaderCompiled(unsigned int id);
	bool ProgLinked(unsigned int id);

	void DumpLog(unsigned int id);

	unsigned int m_id;
	unsigned int m_vertProgId;
	unsigned int m_fragProgId;
};

#endif /* _SHADER_H_ */
