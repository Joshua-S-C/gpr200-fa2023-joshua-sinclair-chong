#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/shader.h>
#include <ew/texture.h>
#include <ew/procGen.h>
#include <ew/transform.h>
#include <ew/camera.h>
#include <ew/cameraController.h>

#include <jsc/transformations.h>
#include <jsc/texture.h>
#include <jsc/camera.h>
#include <jsc/procGen.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void resetCamera(ew::Camera& camera, ew::CameraController& cameraController);

int SCREEN_WIDTH = 1080;
int SCREEN_HEIGHT = 720;

float prevTime;
ew::Vec3 bgColor = ew::Vec3(0.1f);

ew::Camera camera;
ew::CameraController cameraController;

struct Light {
	ew::Transform transform; // World Space
	ew::Vec3 clr; // RBG
};

struct Material {
	// Coefficients. 0-1
	float ambientK = .2;
	float diffuseK = .5; 
	float specularK = .5;
	float shininess = 32;
};

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

	//Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	//Global settings
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

// Objects & Shaders ----------------------------------------------------*/
	ew::Shader shader("assets/defaultLit.vert", "assets/defaultLit.frag");
	ew::Shader unlitShader("assets/unlit.vert", "assets/unlit.frag");
	
	unsigned int brickTexture = ew::loadTexture("assets/brick_color.jpg",GL_REPEAT,GL_LINEAR);
	

	// Lights
	Light light;
	light.transform.position = { 0,0,1 };
	light.clr = { 1,0,1 };
	ew::Mesh lightMesh(ew::createSphere(0.2f, 32));

	// Materials
	Material mat;

	// Objects
	ew::Mesh cubeMesh(ew::createCube(1.0f));
	ew::Mesh planeMesh(ew::createPlane(5.0f, 5.0f, 10));
	ew::Mesh sphereMesh(ew::createSphere(0.5f, 64));
	ew::Mesh cylinderMesh(ew::createCylinder(0.5f, 1.0f, 32));
	//ew::Mesh torusMesh(jsc::createTorus(1.0f, 0.3f, 32, 16));

	ew::Transform cubeTransform;
	ew::Transform planeTransform;
	ew::Transform sphereTransform;
	ew::Transform cylinderTransform;
	ew::Transform torusTransform;
	planeTransform.position = ew::Vec3(0, -1.0, 0);
	sphereTransform.position = ew::Vec3(-1.5f, 0.0f, 0.0f);
	cylinderTransform.position = ew::Vec3(1.5f, 0.0f, 0.0f);
	//torusTransform.position = ew::Vec3(0, 0.0f, -3.0f);

	resetCamera(camera,cameraController);

// Render Loop ----------------------------------------------------------*/
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		float deltaTime = time - prevTime;
		prevTime = time;

		//Update camera
		camera.aspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
		cameraController.Move(window, &camera, deltaTime);

		//RENDER
		glClearColor(bgColor.x, bgColor.y,bgColor.z,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

// Uniforms & Draw ------------------------------------------------------*/
		shader.use();
		glBindTexture(GL_TEXTURE_2D, brickTexture);
		shader.setInt("_Texture", 0);
		shader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());
		
		shader.setVec3("_Light.pos", light.transform.position);
		shader.setVec3("_Light.clr", light.clr);

		// I want a function for this but Im using eric's shader object
		shader.setFloat("_Material.ambientK", mat.ambientK);
		shader.setFloat("_Material.diffuseK", mat.diffuseK);
		shader.setFloat("_Material.specularK", mat.specularK);
		shader.setFloat("_Material.shininess", mat.shininess);

		shader.setVec3("_viewPos", camera.position);

		//Draw shapes
		shader.setMat4("_Model", cubeTransform.getModelMatrix());
		cubeMesh.draw();

		shader.setMat4("_Model", planeTransform.getModelMatrix());
		planeMesh.draw();

		shader.setMat4("_Model", sphereTransform.getModelMatrix());
		sphereMesh.draw();

		shader.setMat4("_Model", cylinderTransform.getModelMatrix());
		cylinderMesh.draw();

		//shader.setMat4("_Model", torusTransform.getModelMatrix());
		//torusMesh.draw();

		//TODO: Render point lights
		unlitShader.use();
		unlitShader.setMat4("_ViewProjection", camera.ProjectionMatrix()* camera.ViewMatrix());
		unlitShader.setMat4("_Model", light.transform.getModelMatrix());
		unlitShader.setVec3("_Color", light.clr);
		lightMesh.draw();

// Render UI ------------------------------------------------------------*/
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			if (ImGui::CollapsingHeader("Camera")) {
				ImGui::DragFloat3("Position", &camera.position.x, 0.1f);
				ImGui::DragFloat3("Target", &camera.target.x, 0.1f);
				ImGui::Checkbox("Orthographic", &camera.orthographic);
				if (camera.orthographic) {
					ImGui::DragFloat("Ortho Height", &camera.orthoHeight, 0.1f);
				}
				else {
					ImGui::SliderFloat("FOV", &camera.fov, 0.0f, 180.0f);
				}
				ImGui::DragFloat("Near Plane", &camera.nearPlane, 0.1f, 0.0f);
				ImGui::DragFloat("Far Plane", &camera.farPlane, 0.1f, 0.0f);
				ImGui::DragFloat("Move Speed", &cameraController.moveSpeed, 0.1f);
				ImGui::DragFloat("Sprint Speed", &cameraController.sprintMoveSpeed, 0.1f);
				if (ImGui::Button("Reset")) {
					resetCamera(camera, cameraController);
				}
			}

			if (ImGui::CollapsingHeader("Material")) {
				ImGui::DragFloat("Ambient K", &mat.ambientK, 0.1f);
				ImGui::DragFloat("Diffuse K", &mat.diffuseK, 0.1f);
				ImGui::DragFloat("Specular K", &mat.specularK, 0.1f);
				ImGui::DragFloat("Shininess", &mat.shininess, 0.1f);

			}

			if (ImGui::CollapsingHeader("Light 1")) {
				ImGui::DragFloat3("Light Pos", &light.transform.position.x, 0.1f);
				ImGui::ColorEdit3("Light Color", &light.clr.x);
			}



			ImGui::ColorEdit3("BG color", &bgColor.x);
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
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
}

void resetCamera(ew::Camera& camera, ew::CameraController& cameraController) {
	camera.position = ew::Vec3(0, 0, 5);
	camera.target = ew::Vec3(0);
	camera.fov = 60.0f;
	camera.orthoHeight = 6.0f;
	camera.nearPlane = 0.1f;
	camera.farPlane = 100.0f;
	camera.orthographic = false;

	cameraController.yaw = 0.0f;
	cameraController.pitch = 0.0f;
}


