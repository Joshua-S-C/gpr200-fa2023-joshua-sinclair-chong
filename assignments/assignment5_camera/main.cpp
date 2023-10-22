#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/shader.h>
#include <ew/procGen.h>
#include <ew/transform.h>

#include <jsc/transformations.h>
#include <jsc/texture.h>
#include <jsc/camera.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);

// Projection will account for aspect ratio! WOW!
const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

const int NUM_CUBES = 4;
ew::Transform cubeTransforms[NUM_CUBES];

float prevTime;



int main() {
// Initialize -----------------------------------------------------------*/
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Camera", NULL, NULL);
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

	// Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	// Enable back face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Depth testing - required for depth sorting!
	glEnable(GL_DEPTH_TEST);

	// Delta Time!
	float time = (float)glfwGetTime(); 
	float deltaTime = time - prevTime;
	prevTime = time;

// Shader , Camera & Cubes ----------------------------------------------*/
	ew::Shader shader("assets/vertexShader.vert", "assets/fragmentShader.frag");
	jsc::Camera camera;
	jsc::CameraControls cameraControls;

	//Cube mesh & positions
	ew::Mesh cubeMesh(ew::createCube(0.5f));
	for (size_t i = 0; i < NUM_CUBES; i++)
	{
		cubeTransforms[i].position.x = i % (NUM_CUBES / 2) - 0.5;
		cubeTransforms[i].position.y = i / (NUM_CUBES / 2) - 0.5;
	}

// Render Loop ----------------------------------------------------------*/
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		//Clear both color buffer AND depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

// Uniforms & Draw ------------------------------------------------------*/
		jsc::moveCamera(window, &camera, &cameraControls, prevTime);
		camera.UpdateAspectRatio();
		
		shader.use();
		shader.setMat4("_View", camera.ViewMatrix());
		shader.setMat4("_Projection", camera.ProjectionMatrix());
		for (size_t i = 0; i < NUM_CUBES; i++) {
			shader.setMat4("_Model", cubeTransforms[i].getModelMatrix());
			cubeMesh.draw();
		}

// Render UI ------------------------------------------------------------*/
		// TODO : Add option to select a target and then have the camera focus on that object?? . Maybe even make it tween althought it would be hard cuz the subesdon't have any general position member
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();
			ImGui::SetNextWindowPos({ 0,0 });
			ImGui::SetNextWindowSize({ 300, 720 });
			ImGui::Begin("Settings", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);

			ImGui::Checkbox("Orthgraphic", &camera.orthographic);
			if (ImGui::Button("Reset Camera")) camera.Reset();

			int itemIndex;
			ImGui::Combo("Select Target", &itemIndex, "Cube 1\0Cube 2\0Cube 3\0Cube 4");
			// TODO : The thing
			if (ImGui::Button("Focus Object")) cameraFocus(&camera, &cameraControls, deltaTime, cubeTransforms[itemIndex].position);


			if (ImGui::CollapsingHeader("Control Settings")) {
				ImGui::DragFloat("Sensitivity", &cameraControls.mouseSens ,0.05f);
				ImGui::DragFloat("Speed", &cameraControls.moveSpd ,0.05f);
				ImGui::DragFloat("Sprint Speed", &cameraControls.sprintSpd ,0.05f);
				if (ImGui::Button("Reset")) cameraControls.Reset();
			}
			
			if (ImGui::CollapsingHeader("Camera Settings")) {
				// Orbit checkbox
				ImGui::DragFloat3("Position", &camera.position.x, 0.05f);
				ImGui::DragFloat3("Target", &camera.target.x, 0.05f);
				ImGui::DragFloat("Ortho Height", &camera.orthoSize, 0.05f);
				ImGui::DragFloat("FOV", &camera.fov, 0.05f);
				ImGui::DragFloat("Near Plane", &camera.nearPlane, 0.05f);
				ImGui::DragFloat("Far Plane", &camera.farPlane, 0.05f);
				ImGui::DragFloat("Aspect Ratio Manual", &camera.aspectRatio, 0.05f);
			}

			if (ImGui::CollapsingHeader("Cubes")) {
				for (size_t i = 0; i < NUM_CUBES; i++) {
					ImGui::PushID(i);
					if (ImGui::CollapsingHeader("Transform")) {
						ImGui::DragFloat3("Position", &cubeTransforms[i].position.x, 0.05f);
						ImGui::DragFloat3("Rotation", &cubeTransforms[i].rotation.x, 1.0f);
						ImGui::DragFloat3("Scale", &cubeTransforms[i].scale.x, 0.05f);
					}
					ImGui::PopID();
				}
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

