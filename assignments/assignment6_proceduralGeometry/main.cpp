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

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void resetCamera(ew::Camera& camera, ew::CameraController& cameraController);

int SCREEN_WIDTH = 1080;
int SCREEN_HEIGHT = 720;

float prevTime;

struct AppSettings {
	const char* shadingModeNames[6] = { "Solid Color","Normals","UVs","Texture","Lit","Texture Lit"};
	int shadingModeIndex;

	ew::Vec3 bgColor = ew::Vec3(0.1f);
	ew::Vec3 shapeColor = ew::Vec3(1.0f);
	bool wireframe = true;
	bool drawAsPoints = false;
	bool backFaceCulling = true;

	//Euler angles (degrees)
	ew::Vec3 lightRotation = ew::Vec3(0, 0, 0);
}appSettings;

ew::Camera camera;
ew::CameraController cameraController;

int main() {
// Initialize -----------------------------------------------------------*/
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Procedural Geometry", NULL, NULL);
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

	//Enable back face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//Depth testing - required for depth sorting!
	glEnable(GL_DEPTH_TEST);
	glPointSize(3.0f);
	glPolygonMode(GL_FRONT_AND_BACK, appSettings.wireframe ? GL_LINE : GL_FILL);

// Shader , Camera & Cubes ----------------------------------------------*/
	ew::Shader shader("assets/vertexShader.vert", "assets/fragmentShader.frag");
	ew::Shader shader2("assets/vertexShader.vert", "assets/fragmentShader.frag");
	unsigned int brickTexture = ew::loadTexture("assets/brick_color.jpg",GL_REPEAT,GL_LINEAR);

	// Create Cube
	float cubeSize = 1;
	ew::MeshData cubeMeshData = ew::createCube(cubeSize);
	ew::Mesh cubeMesh(cubeMeshData);

	// Create Plane
	float planeSize = 1;
	int planeSubdivs = 8;
	ew::MeshData planeMD = ew::createPlane(planeSize, planeSubdivs);
	ew::Mesh planeMesh(planeMD);

	// Create Cylinder
	float cylinderRadius = 1;
	float cylinderHeight = 2;
	int cylinderSubdivs = 8;
	ew::MeshData cylinderMD = ew::createCylinder(cylinderRadius, cylinderHeight, cylinderSubdivs);
	ew::Mesh cylinderMesh(cylinderMD);
	
	// Create Sphere
	float sphereRadius = 1;
	int sphereSubdivs = 8;
	ew::MeshData sphereMD = ew::createSphere(sphereRadius, sphereSubdivs);
	ew::Mesh sphereMesh(sphereMD);

	// Create Torus
	float torusSize[2] = { 1, .5 };
	int torusSubdivs[2] = { 8, 16 };
	ew::MeshData torusMD = ew::createTorus(torusSize[0], torusSize[1], torusSubdivs[0], torusSubdivs[1]);
	ew::Mesh torusMesh(torusMD);


	// Initialize transforms
	ew::Transform cubeTransform;
	cubeTransform.position.x = 0;

	ew::Transform planeTransform;
	planeTransform.position.x = 3;

	ew::Transform cylinderTransform;
	cylinderTransform.position.x = 6;

	ew::Transform sphereTransform;
	sphereTransform.position.x = 9;

	ew::Transform torusTransform;
	torusTransform.position.x = 12;


	resetCamera(camera,cameraController);

// Render Loop ----------------------------------------------------------*/
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		camera.aspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;

		float time = (float)glfwGetTime();
		float deltaTime = time - prevTime;
		prevTime = time;

		cameraController.Move(window, &camera, deltaTime);

		//Render
		glClearColor(appSettings.bgColor.x, appSettings.bgColor.y, appSettings.bgColor.z,1.0f);

		//Clear both color buffer AND depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
// Uniforms & Draw ------------------------------------------------------*/
		shader.use();
		glBindTexture(GL_TEXTURE_2D, brickTexture);
		shader.setInt("_Texture", 0);
		shader.setInt("_Mode", appSettings.shadingModeIndex);
		shader.setVec3("_Color", appSettings.shapeColor);
		shader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());

		// Euler angels to forward vector
		ew::Vec3 lightRot = appSettings.lightRotation * ew::DEG2RAD;
		ew::Vec3 lightF = ew::Vec3(sinf(lightRot.y) * cosf(lightRot.x), sinf(lightRot.x), -cosf(lightRot.y) * cosf(lightRot.x));
		shader.setVec3("_LightDir", lightF);

		// Draw cube
		shader.setMat4("_Model", cubeTransform.getModelMatrix());
		cubeMesh.draw((ew::DrawMode)appSettings.drawAsPoints);
		
		// Draw plane
		shader.setMat4("_Model", planeTransform.getModelMatrix());
		planeMesh.draw((ew::DrawMode)appSettings.drawAsPoints);

		// Draw cylinder
		shader.setMat4("_Model", cylinderTransform.getModelMatrix());
		cylinderMesh.draw((ew::DrawMode)appSettings.drawAsPoints);

		// Draw polar sphere
		shader.setMat4("_Model", sphereTransform.getModelMatrix());
		sphereMesh.draw((ew::DrawMode)appSettings.drawAsPoints);
		
		// Draw torus
		shader.setMat4("_Model", torusTransform.getModelMatrix());
		torusMesh.draw((ew::DrawMode)appSettings.drawAsPoints);

// Render UI ------------------------------------------------------------*/
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();
			ImGui::SetNextWindowPos({ 0,0 });
			ImGui::SetNextWindowSize({ 300, 720 });
			ImGui::Begin("Settings", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
			
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

			ImGui::ColorEdit3("BG color", &appSettings.bgColor.x);
			ImGui::ColorEdit3("Shape color", &appSettings.shapeColor.x);

			ImGui::Combo("Shading mode", &appSettings.shadingModeIndex, appSettings.shadingModeNames, IM_ARRAYSIZE(appSettings.shadingModeNames));
			if (appSettings.shadingModeIndex > 3) {
				ImGui::DragFloat3("Light Rotation", &appSettings.lightRotation.x, 1.0f);
			}

			ImGui::Checkbox("Draw as points", &appSettings.drawAsPoints);
			if (ImGui::Checkbox("Wireframe", &appSettings.wireframe)) {
				glPolygonMode(GL_FRONT_AND_BACK, appSettings.wireframe ? GL_LINE : GL_FILL);
			}
			if (ImGui::Checkbox("Back-face culling", &appSettings.backFaceCulling)) {
				if (appSettings.backFaceCulling)
					glEnable(GL_CULL_FACE);
				else
					glDisable(GL_CULL_FACE);
			}

			// Prolly put this all in a menu where you select an object like before, along with size
			// If slider is true, then the value changed
			// Sliders Need to be in separate statements otherwise changing one will refresh those below it

			// Cube
			if (ImGui::CollapsingHeader("Cube")) {
				if (ImGui::SliderFloat("Cube Size", &cubeSize, 1, 10)) {
					cubeMeshData = ew::createCube(cubeSize);
					cubeMesh = cubeMeshData;
				}
			}

			// Plane
			if (ImGui::CollapsingHeader("Plane")) {
				if (ImGui::SliderInt("Plane Subdivs", &planeSubdivs, 3, 100)) {
					planeMD = ew::createPlane(planeSize, planeSubdivs);
					planeMesh = planeMD;
				}

				if (ImGui::SliderFloat("Plane Size", &planeSize, 1, 10)) {
					planeMD = ew::createPlane(planeSize, planeSubdivs);
					planeMesh = planeMD;
				}
			}
			
			// Cylinder
			if (ImGui::CollapsingHeader("Cylinder")) {
				if (ImGui::SliderInt("Cyl Subdivs", &cylinderSubdivs, 3, 100)) {
					cylinderMD = ew::createCylinder(cylinderRadius, cylinderHeight, cylinderSubdivs);
					cylinderMesh = cylinderMD;
				}
				if (ImGui::SliderFloat("Cyl Radius", &cylinderRadius, 1, 5)) {
					cylinderMD = ew::createCylinder(cylinderRadius, cylinderHeight, cylinderSubdivs);
					cylinderMesh = cylinderMD;
				}
				if (ImGui::SliderFloat("Cyl Height", &cylinderHeight, 1, 5)) {
					cylinderMD = ew::createCylinder(cylinderRadius, cylinderHeight, cylinderSubdivs);
					cylinderMesh = cylinderMD;
				}
			}

			// Sphere
			if (ImGui::CollapsingHeader("Sphere")) {
				if (ImGui::SliderInt("Sph Subdivs", &sphereSubdivs, 3, 100)) {
					sphereMD = ew::createSphere(sphereRadius, sphereSubdivs);
					sphereMesh = sphereMD;
				}
				if (ImGui::SliderFloat("Sph Radius", &sphereRadius, 1, 10)) {
					sphereMD = ew::createSphere(sphereRadius, sphereSubdivs);
					sphereMesh = sphereMD;
				}
			}

			// Torus
			if (ImGui::CollapsingHeader("Torus")) {
				if (ImGui::SliderInt("Torus Subdivs", &torusSubdivs[0], 3, 100)) {
					torusMD = ew::createTorus(torusSize[0], torusSize[1], torusSubdivs[0], torusSubdivs[1]);
					torusMesh = torusMD;
				}
				if (ImGui::SliderInt("Torus Ring Subdivs", &torusSubdivs[1], 3, 100)) {
					torusMD = ew::createTorus(torusSize[0], torusSize[1], torusSubdivs[0], torusSubdivs[1]);
					torusMesh = torusMD;
				}
				if (ImGui::SliderFloat("Torus Radius", &torusSize[0], 0, 10)) {
					torusMD = ew::createTorus(torusSize[0], torusSize[1], torusSubdivs[0], torusSubdivs[1]);
					torusMesh = torusMD;
				}
				if (ImGui::SliderFloat("Torus Ring Radius", &torusSize[1], 0, 10)) {
					torusMD = ew::createTorus(torusSize[0], torusSize[1], torusSubdivs[0], torusSubdivs[1]);
					torusMesh = torusMD;
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
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
	camera.aspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
}

void resetCamera(ew::Camera& camera, ew::CameraController& cameraController) {
	camera.position = ew::Vec3(0, 0, 3);
	camera.target = ew::Vec3(0);
	camera.aspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
	camera.fov = 60.0f;
	camera.orthoHeight = 6.0f;
	camera.nearPlane = 0.1f;
	camera.farPlane = 100.0f;
	camera.orthographic = false;

	cameraController.yaw = 0.0f;
	cameraController.pitch = 0.0f;
}


