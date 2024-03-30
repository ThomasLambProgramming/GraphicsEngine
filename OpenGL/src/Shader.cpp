#include "Shader.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "Renderer.h"

Shader::Shader(const std::string& filepath) : m_FilePath(filepath), m_RendererID(0)
{
	ShaderProgramSource source = ParseShader(filepath);
	m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
	glDeleteProgram(m_RendererID);
}
ShaderProgramSource Shader::ParseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);

	enum class ShaderType
	{
		NONE = -1,
		VERTEX = 0,
		FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];

	ShaderType type = ShaderType::NONE;

	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
			{
				//vertex mode
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos)
			{
				//fragment mode
				type = ShaderType::FRAGMENT;
			}
		}
		else
		{
			ss[(int)type] << line << "\n";
		}
	}
	return { ss[0].str(), ss[1].str() };
}
unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	//Parameters: ShaderID to be overwitten, count of shader source, source string of char** so the memory address of the char*, then length, 
	//nullptr means that the string is assumed to be null terminated (\0)
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;

	//getshaderiv is a error checking thing (returns error codes and bools for if passed)
	//glgetshaderiv is id of shader, what you want to check, then a GLint* OTHERWISE KNOWN AS A NORMAL POINTER TO AN INT
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	//it returns GL_TRUE and etc to the result

	//ERROR CHECKING, holy shit that is horrid to look at.
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char)); \
			//shaderinfo: 
			//shader id, 
			//maxlength (specifies the size of the character buffer for storing the returned information log   
			//length (returns the length of the string returned in infolog (without null terminator)
			//infolog char* that is used for returning the info log
			glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}
unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	//program is the actual shader itself 
	unsigned int program = glCreateProgram();

	//creation of vertex shader and the fragment shader
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	//checks if the program can excute in the first place with the given shader
	glValidateProgram(program);

	//object is basically copied data so original isnt needed
	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}
void Shader::Bind() const
{
	glUseProgram(m_RendererID);
}
void Shader::Unbind() const
{
	glUseProgram(0);
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

void Shader::SetUniform1i(const std::string& name, int value)
{
	glUniform1i(GetUniformLocation(name), value);
	
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}

int Shader::GetUniformLocation(const std::string& name)
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
	{
		return m_UniformLocationCache[name];
	}

	int location = glGetUniformLocation(m_RendererID, name.c_str());
	if (location == -1)
		std::cout << "Warning: uniform " << name << " not found" << std::endl;
	
	m_UniformLocationCache[name] = location;
	return location;
}


