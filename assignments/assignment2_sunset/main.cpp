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
bool showImGUIDemoWindow = false;


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

// Uniforms -------------------------------------------------------------*/
	// BG Uniforms
	float bgClrTop[3] = { 0.0,0.2,1.0 };
	float bgClrTop2[3] = { 0.2,0.7,0.9 };
	float bgClrBtm[3] = { 1.0,0.0,0.3 };
	float bgClrBtm2[3] = { 0.0,0.0,0.0 };
	
	// Sun Uniforms
	float sunSpeed = 1;
	float sunBlur = .2;
	float sunInner = .3;
	float sunClr[3] = { 1.0,1.0,0.0 };
	float sunClr2[3] = { 1.0,0.65,0.0 };

	// Wave 1 Uniforms
	float wave1ClrTop[3] = { 0.3, 0.65, 1.00 };
	float wave1ClrBtm[3] = { 0.15, 0.40, 0.70 };
	float wave1F = 1.0;
	float wave1A = 0.2;
	float wave1O = -0.2;

	// Wave 2 Uniforms
	float wave2ClrTop[3] = { 0.3,0.40,0.80 };
	float wave2ClrBtm[3] = { 0.30,0.75,1.00 };
	float wave2F = 5.0;
	float wave2A = 0.07;
	float wave2O = -0.55;

	// Wave 2-Compound Uniforms
	float wave2CF = 11.0;
	float wave2CA = 0.03;

// Render Loop ----------------------------------------------------------*/
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

// Set Uniforms ---------------------------------------------------------*/
		//General Uniforms
		shader.setFloat("_Time", (float)glfwGetTime());
		shader.setVec2("_Resolution", SCREEN_WIDTH, SCREEN_HEIGHT);

		// BG Uniforms
		shader.setVec3("bgClrTop", bgClrTop[0], bgClrTop[1], bgClrTop[2]);
		shader.setVec3("bgClrBtm", bgClrBtm[0], bgClrBtm[1], bgClrBtm[2]);
		shader.setVec3("bgClrTop2", bgClrTop2[0], bgClrTop2[1], bgClrTop2[2]);
		shader.setVec3("bgClrBtm2", bgClrBtm2[0], bgClrBtm2[1], bgClrBtm2[2]);

		// Sun Uniforms
		shader.setFloat("sunSpeed", sunSpeed);
		shader.setFloat("sunBlur", sunBlur);
		shader.setFloat("sunInner", sunInner);
		shader.setVec3("sunClr", sunClr[0], sunClr[1], sunClr[2]);
		shader.setVec3("sunClr2", sunClr2[0], sunClr2[1], sunClr2[2]);

		// Wave 1 Uniforms
		shader.setVec3("wave1ClrTop", wave1ClrTop[0], wave1ClrTop[1], wave1ClrTop[2]);
		shader.setVec3("wave1ClrBtm", wave1ClrBtm[0], wave1ClrBtm[1], wave1ClrBtm[2]);
		shader.setFloat("wave1F", wave1F);
		shader.setFloat("wave1A", wave1A);
		shader.setFloat("wave1O", wave1O);

		// Wave 2 Uniforms
		shader.setVec3("wave2ClrTop", wave2ClrTop[0], wave2ClrTop[1], wave2ClrTop[2]);
		shader.setVec3("wave2ClrBtm", wave2ClrBtm[0], wave2ClrBtm[1], wave2ClrBtm[2]);
		shader.setFloat("wave2F", wave2F);
		shader.setFloat("wave2A", wave2A);
		shader.setFloat("wave2O", wave2O);
		
		// Wave 2-Compound Uniforms
		shader.setFloat("wave2CF", wave2CF);
		shader.setFloat("wave2CA", wave2CA);

		// Draw Call (a singular, lonely draw call)
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

// Render UI ------------------------------------------------------------*/
		ImGui_ImplGlfw_NewFrame();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();

		// Background Settings
		{	ImGui::Begin("Background");

			ImGui::ColorEdit3("Top Colour", bgClrTop);
			ImGui::ColorEdit3("Top Colour 2", bgClrTop2);
			ImGui::ColorEdit3("Bottom Colour", bgClrBtm);
			ImGui::ColorEdit3("Bottom Colour 2", bgClrBtm2);

			//ImGui::Checkbox("Show Demo Window", &showImGUIDemoWindow);
			//if (showImGUIDemoWindow) {
			//	ImGui::ShowDemoWindow(&showImGUIDemoWindow);
			//}

			ImGui::End();	}

		// Sun Settings
		{	ImGui::Begin("Sun");

			ImGui::SliderFloat("Speed", &sunSpeed, 0.0f, 5.0f);
			ImGui::SliderFloat("Blur", &sunBlur, 0.0f, 5.0f);
			ImGui::SliderFloat("Inner", &sunInner, 0.0f, 1.0f);
			ImGui::ColorEdit3("Colour", sunClr);
			ImGui::ColorEdit3("Colour 2", sunClr2);

			ImGui::End();	}

		// Wave 1 Settings
		{	ImGui::Begin("Wave 1");

			ImGui::ColorEdit3("Top Colour", wave1ClrTop);
			ImGui::ColorEdit3("Bottom Colour", wave1ClrBtm);
			ImGui::SliderFloat("Frequency", &wave1F, -5.0f, 5.0f);
			ImGui::SliderFloat("Amplitude", &wave1A, -3.0f, 3.0f);
			ImGui::SliderFloat("Offset", &wave1O, -1.5f, 1.0f);

			ImGui::End();	}

		// Wave 2 Settings
		{	ImGui::Begin("Wave 2 (Compound)");

			//ImGui::ColorEdit3("Top Colour", wave2ClrTop);	// Doesn't actually do anything cuz of how mixing
			ImGui::ColorEdit3("Bottom Colour", wave2ClrBtm);
			ImGui::SliderFloat("Frequency", &wave2F, -5.0f, 5.0f);
			ImGui::SliderFloat("Amplitude", &wave2A, -3.0f, 3.0f);
			ImGui::SliderFloat("Offset", &wave2O, -1.5f, 1.0f);
			
			ImGui::SliderFloat("C-Frequency", &wave2CF, 0.0f, 20.0f);
			ImGui::SliderFloat("C-Amplitude", &wave2CA, 0.0f, 1.0f);

			ImGui::End();	}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); 
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

