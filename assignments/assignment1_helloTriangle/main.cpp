#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

float vertices2[9] = {
//	x		y		z
	-0.5,	-0.5,	0.0, // Bottom Left
	 0.5,	-0.5,	0.0, // Bottom Right
	 0.0,	 0.5,	0.0	 // Top Centre
};

float vertices[21] = {
	//x   //y  //z   //r  //g  //b  //a
	-0.5, -0.5, 0.0, 1.0, 0.0, 0.0, 1.0, //Bottom left
	 0.5, -0.5, 0.0, 0.0, 1.0, 0.0, 1.0, //Bottom right // BT and TC aren't changing colour; they're staying black
	 0.0,  0.5, 0.0, 0.0, 0.0, 1.0, 1.0  //Top center
};


const char* vertexShaderSource = R"(
	#version 450
	layout(location = 0) in vec3 vPos;
	layout(location = 1) in vec4 vColor;
	out vec4 Color;
	void main(){
		Color = vColor;
		gl_Position = vec4(vPos,1.0);
	}
)";

const char* fragmentShaderSource = R"(
	#version 450
	out vec4 FragColor;
	in vec4 Color;
	void main(){
		FragColor = Color;
	}
)";

// Setting up vertex data on CPU
// Constant values here will change when adding in colour attributes
unsigned int createVAO(float* vertexData, int numVertices) {
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	int stride = 7;	// Will change based on the num of attributes

	// Define new buffer id
	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// Allocates space for & send vertex data to GPU
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * stride * numVertices, vertexData, GL_STATIC_DRAW);

// Attributes : 7 floats

	// Position Attribute : 3 floats
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * stride, (const void*)0);
	glEnableVertexAttribArray(0);

	// Colour Attribute : 4 floats
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * stride, (const void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);

	return vao;
}


unsigned int createShader(GLenum shaderType, const char* sourceCode) {
	unsigned int vertexShader = glCreateShader(shaderType);
	glShaderSource(vertexShader, 1, &sourceCode, NULL);	// Supply the shader object with source code
	glCompileShader(vertexShader);	// Compile the shader object

	int success;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if (!success) {
		char infoLog[512];	// Arbitrary length
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		printf("Failed to compile shader: %s", infoLog);
	}

	return vertexShader;
}

//Creates a new shader program with vertex + fragment stages
//Returns id of new shader program if successful, 0 if failed
unsigned int createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource) {
	unsigned int vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderSource);
	unsigned int fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
	unsigned int shaderProgram = glCreateProgram();

	// Attach
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	// Link
	glLinkProgram(shaderProgram);

	//Check for linking errors
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


int main() {
	// Initialization
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello Triangle :D", NULL, NULL);
	if (window == NULL) {
		printf("GLFW failed to create window");
		return 1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return 1;
	}

	// OpenGL initialization
	unsigned int shader = createShaderProgram(vertexShaderSource, fragmentShaderSource);
	unsigned int vao = createVAO(vertices, 3);

	// Render Loop
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.6f, 0.4f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Draw Calls
		glUseProgram(shader);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}
