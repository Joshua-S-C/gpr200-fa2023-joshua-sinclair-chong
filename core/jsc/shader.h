#pragma once
#include <sstream>
#include <fstream>
#include "../ew/external/glad.h"
#include "../ew/ewMath/mat4.h"


namespace jsc {
	std::string loadShaderSourceFromFile(const std::string& filePath);
	unsigned int createShader(GLenum shaderType, const char* sourceCode);
	unsigned int createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource);

	class Shader {
	public:
		Shader(const std::string& vertexShader, const std::string& fragmentShader);
		void use();
		void setInt(const std::string& name, int v) const;
		void setFloat(const std::string& name, float v) const;
		void setVec2(const std::string& name, float x, float y) const;
		void setVec3(const std::string& name, float x, float y, float z) const;
		void setVec4(const std::string& name, float x, float y, float z, float w) const;
		void setMat4(const std::string& name, const ew::Mat4& v) const;
	private:
		unsigned int m_id; //OpenGL program handle

	};
}
