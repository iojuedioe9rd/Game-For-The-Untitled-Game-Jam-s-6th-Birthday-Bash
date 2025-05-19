#include "Shader.h"
#include "feline_load.h"
#include "Platform/OpenGL/gl.h"
#include <iostream> // For error logging

namespace Engine
{
	Ref<Shader> Shader::Create(const char* vertexFilePath, const char* fragmentFilePath)
	{
		return CreateRef<Shader>(vertexFilePath, fragmentFilePath);
	}

	Shader::Shader(const char* vertexFilePath, const char* fragmentFilePath)
	{
		File vertexFile = feline_load_file(vertexFilePath, true);
		File fragmentFile = feline_load_file(fragmentFilePath, true);

		const char* vertexSource = (char*)vertexFile.buffer;
		const char* fragmentSource = (char*)fragmentFile.buffer;

		// Create and compile vertex shader
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexSource, nullptr);
		glCompileShader(vertexShader);
		CheckCompileErrors(vertexShader, "VERTEX");

		// Create and compile fragment shader
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
		glCompileShader(fragmentShader);
		CheckCompileErrors(fragmentShader, "FRAGMENT");

		// Link shaders into a program
		ID = glCreateProgram();
		glAttachShader(ID, vertexShader);
		glAttachShader(ID, fragmentShader);
		glLinkProgram(ID);
		CheckLinkErrors(ID);

		// Delete individual shaders after linking
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	Shader::~Shader()
	{
		if (ID != 0)
			Delete();
	}

	void Shader::Activate() const
	{
		glUseProgram(ID);
	}

	void Shader::Delete()
	{
		glDeleteProgram(ID);
		ID = 0;
	}

	void Shader::CheckCompileErrors(GLuint shader, const char* type)
	{
		GLint success;
		GLchar infoLog[1024];
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
			std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
				<< infoLog << "\n";
		}
	}

	void Shader::CheckLinkErrors(GLuint program)
	{
		
		GLint success;
		GLchar infoLog[1024];
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(program, 1024, nullptr, infoLog);
			std::cerr << "ERROR::PROGRAM_LINKING_ERROR\n" << infoLog << "\n";
		}
	}
}
