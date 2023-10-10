#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/shader.h>
#include <ew/ewMath/vec3.h>
#include <ew/procGen.h>

#include <jsc/transformations.h>
#include <jsc/texture.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);

//Square aspect ratio for now. We will account for this with projection later.
const int SCREEN_WIDTH = 720;
const int SCREEN_HEIGHT = 720;
const int NUM_CUBES = 4;

int main() {
// Initialize Window & ImGui --------------------------------------------*/
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Transformations", NULL, NULL);
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

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
	
// Initialize Mesh & Transforms -----------------------------------------*/
	//Enable back face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//Depth testing - required for depth sorting!
	glEnable(GL_DEPTH_TEST);

	ew::Shader shader("assets/vertexShader.vert", "assets/fragmentShader.frag");
	ew::Mesh cubeMesh(ew::createCube(0.5f));
	jsc::Transform cubeTransforms[4];

	cubeTransforms[0].position = {-.5, .5, 0};
	cubeTransforms[1].position = {-.5, -.5, 0};
	cubeTransforms[2].position = {.5, .5, 0};
	cubeTransforms[3].position = {.5, -.5, 0};

// Render Loop ----------------------------------------------------------*/
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		//Clear both color buffer AND depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

// Drawing Cubes w/ Transforms ------------------------------------------*/
		shader.use();
		for (int i = 0; i < NUM_CUBES; i++) {
			shader.setMat4("_Model", cubeTransforms[i].getModelMatrix());
			cubeMesh.draw();
		}

// Uniforms -------------------------------------------------------------*/
		// wow such empty
		
// Render UI ------------------------------------------------------------*/
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::SetNextWindowPos({ 0,0 });
			ImGui::SetNextWindowSize({ 300, 720 });
			ImGui::Begin("Settings", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);

			if (ImGui::Button("Reset")) {
				cubeTransforms[0].position = { -.5, .5, 0 };
				cubeTransforms[1].position = { -.5, -.5, 0 };
				cubeTransforms[2].position = { .5, .5, 0 };
				cubeTransforms[3].position = { .5, -.5, 0 };
				
				for (int i = 0; i < NUM_CUBES; i++) {
					cubeTransforms[i].rotation = { 0,0,0 };
					cubeTransforms[i].scale = { 1,1,1 };
				}
			}

			for (int i = 0; i < NUM_CUBES; i++) {
				ImGui::PushID(i);
				if (ImGui::CollapsingHeader("Transform")) {
					ImGui::DragFloat3("Position", &cubeTransforms[i].position.x, 0.05f);
					ImGui::DragFloat3("Rotation", &cubeTransforms[i].rotation.x, 1.0f);
					ImGui::DragFloat3("Scale", &cubeTransforms[i].scale.x, 0.05f);
				}
				ImGui::PopID();
			}
			ImGui::End();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

