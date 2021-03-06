#include "program.h"

Program::Program(const std::string & vertexShaderFile, const std::string & fragmentShaderFile)
	: m_programID(loader::shaderLoader(vertexShaderFile, fragmentShaderFile))
{
	//Nothing
}

Program::~Program()
{
	glDeleteProgram(m_programID);
}

void Program::bind()
{
	glUseProgram(m_programID);
}

void Program::unbind()
{
	glUseProgram(0);
}

GLuint Program::getID() const
{
	return m_programID;
}

void Program::loadInt(GLuint location, int & value)
{
	glUniform1i(location, value);
}

void Program::loadFloat(GLuint location, float & value)
{
	glUniform1f(location, value);
}

void Program::loadVector2f(GLuint location, const vmath::vec2 & vector)
{
	glUniform2f(location, vector[0], vector[1]);
}

void Program::loadVector3f(GLuint location, const vmath::vec3 & vector)
{
	glUniform3f(location, vector[0], vector[1], vector[3]);
}

void Program::loadMatrix4f(GLuint location, const vmath::mat4 & matrix)
{
	glUniformMatrix4fv(location, 1, GL_FALSE, matrix);
}
