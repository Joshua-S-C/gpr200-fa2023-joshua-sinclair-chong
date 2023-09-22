#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <jsc/shader.h>

// Structs
struct Vertex {
	float x, y, z;
	float u, v;
};

// Functions
unsigned int createVAO(Vertex* vertexData, int numVertices, unsigned int* indicesData, int numIndices);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

// Vars
const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

Vertex vertices[4]{
	{-1.0, -1.0,  1.0, 0, 0},	//BTM LFT
	{ 1.0, -1.0,  1.0, 1, 0},	//BTM RGT
	{ 1.0,  1.0,  1.0, 1, 1},	//TOP RGT
	{-1.0,  1.0,  1.0, 0, 1}	//TOP LFT
};
unsigned int indices[6] = {
	0, 1, 2, //Triangle 1. BTM RGHT
	0, 2, 3  //Triangle 2. TOP LFT
	//I am the most intelligent human being for taking only 30 minutes to realize that the indexes start at 0 :P
};
bool showImGUIDemoWindow = true;


int main() {
// Initialize Window ----------------------------------------------------*/
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "A beautiful sunset :3", NULL, NULL);
	if (window == NULL) {
		printf("GLFW failed to create window");
		return 1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return 1;
	}

// Initialize ImGUI -----------------------------------------------------*/
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

// Shader & VAO ---------------------------------------------------------*/
	jsc::Shader shader("assets/vertexShader.vert", "assets/fragmentShader.frag");
	shader.use();
	unsigned int vao = createVAO(vertices, 4, indices, 6);
	glBindVertexArray(vao);

// Render Mode ----------------------------------------------------------*/
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//Wireframe
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	//Shaded

// UNIFORMS -------------------------------------------------------------*/
	// BG Uniforms
	float bgClrTop[3] = { 0.0,1.0,1.0 };
	float bgClrBtm[3] = { .4 + sin((float)glfwGetTime()) * .2,0.0,0.3 };
	
	// Sun Uniforms
	float sunRadius = .2;
	float sunInner = .3;
	float sunClr[3] = { 1.0,1.0,0.0 };

	// Wave 1 Uniforms
	float wave1ClrTop[3] = { 0.2f, 0.50f, 0.80f };
	float wave1ClrBtm[3] = { 0.3f, 0.65f, 1.00f };
	float wave1F = 1.0;
	float wave1A = 0.2;
	float wave1O = -0.2;

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//General Uniforms
		shader.setFloat("_Time", (float)glfwGetTime());
		shader.setVec2("_Resolution", SCREEN_WIDTH, SCREEN_HEIGHT);

		// BG Uniforms
		shader.setVec3("bgClrTop", bgClrTop[0], bgClrTop[1], bgClrTop[2]);
		shader.setVec3("bgClrBtm", bgClrBtm[0], bgClrBtm[1], bgClrBtm[2]);

		// Sun Uniforms
		shader.setFloat("sunRadius", sunRadius);
		shader.setFloat("sunInner", sunInner);
		shader.setVec3("sunClr", sunClr[0], sunClr[1], sunClr[2]);

		// Wave 1 Uniforms
		shader.setVec3("wave1ClrTop", wave1ClrTop[0], wave1ClrTop[1], wave1ClrTop[2]);
		shader.setVec3("wave1ClrBtm", wave1ClrBtm[0], wave1ClrBtm[1], wave1ClrBtm[2]);
		shader.setFloat("wave1F", wave1F);
		shader.setFloat("wave1A", wave1A);
		shader.setFloat("wave1O", wave1O);

		// Draw Calls
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

		// Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			// Misc Uniforms
			ImGui::Begin("Settings");
			ImGui::Checkbox("Show Demo Window", &showImGUIDemoWindow);

			// BG Uniforms 
			ImGui::ColorEdit3("bgClrTop", bgClrTop);
			ImGui::ColorEdit3("bgClrBtm", bgClrBtm);

			// Sun Uniforms
			ImGui::SliderFloat("sunRadius", &sunRadius, 0.0f, 5.0f);
			ImGui::SliderFloat("sunInner", &sunInner, 0.0f, 1.0f);
			ImGui::ColorEdit3("sunClr", sunClr);

			// Wave 1 Uniforms
			ImGui::ColorEdit3("wave1ClrTop", wave1ClrTop);
			ImGui::ColorEdit3("wave1ClrBtm", wave1ClrBtm);
			ImGui::SliderFloat("wave1F", &wave1F, -5.0f, 5.0f);
			ImGui::SliderFloat("wave1A", &wave1A, -3.0f, 3.0f);
			ImGui::SliderFloat("wave1O", &wave1O, -1.0f, 1.0f);

			ImGui::End();
			if (showImGUIDemoWindow) {
				ImGui::ShowDemoWindow(&showImGUIDemoWindow);
			}

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}



unsigned int createVAO(Vertex* vertexData, int numVertices, unsigned int* indicesData, int numIndices) {
	int stride = sizeof(Vertex);

	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	unsigned int ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numIndices, indicesData, GL_STATIC_DRAW);

	//Define a new buffer id
	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//Allocate space for + send vertex data to GPU.
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numVertices * stride, vertexData, GL_STATIC_DRAW);


	// Position : 3 floats
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (const void*)offsetof(Vertex,x));
	glEnableVertexAttribArray(0);

	// UV : 2 floats
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (const void*)offsetof(Vertex,u));
	glEnableVertexAttribArray(1);

	return vao;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

