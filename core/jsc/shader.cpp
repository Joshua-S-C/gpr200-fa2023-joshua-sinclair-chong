#include <fstream>
#include <sstream>
#include "shader.h"
#include "../ew/shader.h"
#include "../ew/ewMath/mat4.h"
#include "../ew/external/glad.h"
#include "../jsc/waves.h"


namespace jsc {
	std::string loadShaderSourceFromFile(const std::string& filePath) {
		std::ifstream fstream(filePath);
		if (!fstream.is_open()) {
			printf("Failed to load file %s", filePath.c_str());
			return {};
		}

		std::stringstream buffer;
		buffer << fstream.rdbuf();
		return buffer.str();
	}

	unsigned int createShader(GLenum shaderType, const char* sourceCode) {
		//Create a new vertex shader object
		unsigned int shader = glCreateShader(shaderType);
		//Supply the shader object with source code
		glShaderSource(shader, 1, &sourceCode, NULL);
		//Compile the shader object
		glCompileShader(shader);
		int success;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			//512 is an arbitrary length, but should be plenty of characters for our error message.
			char infoLog[512];
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			printf("Failed to compile shader: %s", infoLog);
		}
		return shader;
	}

	unsigned int createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource) {
		unsigned int vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderSource);
		unsigned int fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

		unsigned int shaderProgram = glCreateProgram();
		//Attach each stage
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		//Link all the stages together
		glLinkProgram(shaderProgram);
		int success;
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		if (!success) {
			char infoLog[512];
			glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
			printf("Failed to link shader program: %s", infoLog);
		}
		//The linked program now contains our compiled code, so we can delete these intermediate objects
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		return shaderProgram;
	}

	// Shader class
	Shader::Shader(const std::string& vertexShader, const std::string& fragmentShader) {
		std::string vertexShaderSource = loadShaderSourceFromFile(vertexShader.c_str());
		std::string fragmentShaderSource = loadShaderSourceFromFile(fragmentShader.c_str());
		m_id = createShaderProgram(vertexShaderSource.c_str(), fragmentShaderSource.c_str());
	}

	void Shader::use() {
		glUseProgram(m_id);
	}

	void Shader::setInt(const std::string& name, int v) const {
		glUniform1i(glGetUniformLocation(m_id, name.c_str()), v);
	}

	void Shader::setFloat(const std::string& name, float v) const {
		glUniform1f(glGetUniformLocation(m_id, name.c_str()), v);
	}

	void Shader::setVec2(const std::string& name, float x, float y) const {

		glUniform2f(glGetUniformLocation(m_id, name.c_str()), x, y);
	}

	void Shader::setVec3(const std::string& name, float x, float y, float z) const {

		glUniform3f(glGetUniformLocation(m_id, name.c_str()), x, y, z);
	}

	void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const {

		glUniform4f(glGetUniformLocation(m_id, name.c_str()), x, y, z, w);
	}

	void Shader::setMat4(const std::string& name, const ew::Mat4& v) const {
		glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, &v[0][0]);
	}

}

namespace ew {
	// Added. (Apparently this didn't exist)
	void Shader::setBool(const std::string& name, bool v) const
	{
		glUniform1i(glGetUniformLocation(m_id, name.c_str()), v);
	}
	// Added
	void Shader::setMaterial(const std::string& name, jsc::Material v) const
	{
		glUniform1f(glGetUniformLocation(m_id, (name + ".ambientK").c_str()), v.ambientK);
		glUniform1f(glGetUniformLocation(m_id, (name + ".diffuseK").c_str()), v.diffuseK);
		glUniform1f(glGetUniformLocation(m_id, (name + ".specularK").c_str()), v.specularK);
		glUniform1f(glGetUniformLocation(m_id, (name + ".shininess").c_str()), v.shininess);
	}
	// Added
	void Shader::setWave(const std::string& name, jsc::Wave w) const
	{
		glUniform1f(glGetUniformLocation(m_id, (name + ".a").c_str()), w.a);
		glUniform1f(glGetUniformLocation(m_id, (name + ".f").c_str()), w.f);
		glUniform3f(glGetUniformLocation(m_id, (name + ".clr").c_str()), w.clr.x, w.clr.x, w.clr.z);
	}
}