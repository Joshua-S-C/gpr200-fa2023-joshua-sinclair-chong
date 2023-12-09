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
#include <jsc/light.h>
#include <jsc/waves.h>

#define MAX_LIGHTS 4
//int numLights = MAX_LIGHTS;
int numLights = 1;

int numFrames = 0;
float timePerFrame = 0;


void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void resetCamera(ew::Camera& camera, ew::CameraController& cameraController);
void setWave(ew::Shader& shader, jsc::Wave wave);
void frameRate(float& timePerFrame, int& numFrames, float& lastTime, float time);

int SCREEN_WIDTH = 1080;
int SCREEN_HEIGHT = 720;

float prevTime;
//ew::Vec3 bgColor = ew::Vec3(0.1f);

struct AppSettings {
	const char* shaderTypeNames[2] = { "SimpleSin", "Gerstner"};
	int waterShaderIndex = 1;
	const char* vertPaths[2] = { "assets/simpleSin.vert", "assets/gerstner.vert" };
	const char* fragPaths[2] = { "assets/simpleSin.frag", "assets/gerstner.frag" };

	const char* shadingModeNames[4] = { "Solid Color","Normals","UVs","Texture"};
	int shadingModeIndex = 0;

	ew::Vec3 bgColor = ew::Vec3(0.1f);
	ew::Vec3 shapeColor = ew::Vec3(1.0f);	//Unsused
	
	int blendModeIndex = 7;
	int blendModes[20] = { GL_ZERO ,GL_ONE ,GL_SRC_COLOR ,GL_ONE_MINUS_SRC_COLOR ,GL_DST_COLOR ,GL_ONE_MINUS_DST_COLOR ,GL_SRC_ALPHA ,GL_ONE_MINUS_SRC_ALPHA ,GL_DST_ALPHA ,GL_ONE_MINUS_DST_ALPHA ,GL_CONSTANT_COLOR ,GL_ONE_MINUS_CONSTANT_COLOR ,GL_CONSTANT_ALPHA ,GL_ONE_MINUS_CONSTANT_ALPHA ,GL_SRC_ALPHA_SATURATE ,GL_SRC1_COLOR ,GL_ONE_MINUS_SRC1_COLOR ,GL_SRC1_ALPHA ,GL_ONE_MINUS_SRC1_ALPHA };

	bool lit = true;
	bool wireframe = true;
	bool drawAsPoints = false;
	bool backFaceCulling = true;
	bool renderLights = true;
}appSettings;



ew::Camera camera;
ew::CameraController cameraController;

bool phong = false;

int main() {
// Initialize -----------------------------------------------------------*/
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Final", NULL, NULL);
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
	glCullFace(GL_BACK);	// TODO Will need to be changed for the skybox
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, appSettings.blendModes[appSettings.blendModeIndex]);
	glEnable(GL_BLEND);

// Objects & Shaders ----------------------------------------------------*/
	ew::Shader shader("assets/simpleSin.vert", "assets/simpleSin.frag"); // Wave shader
	
	ew::Shader lightsShader("assets/lights.vert", "assets/lights.frag");

	jsc::Wave wave1(2.0f, 0.3f, 1.0f, 1.0f, 1.0f, ew::Vec3{ .5f,.8f,1 });
	
	unsigned int brickTexture = ew::loadTexture("assets/brick_color.jpg",GL_REPEAT,GL_LINEAR);

	// Material
	jsc::Material mat;

	// Lights
	ew::Mesh lightMesh(ew::createSphere(0.2f, 32));
	jsc::Light lights[4];
	lights[0].transform.position = { 0,0,0 };
	lights[0].clr = { 1,1,1 };
	for (int i = 1; i < numLights; i++) {
		lights[i].transform.position = { (float)(2 * (i % 2) - 1),1,(float)(2 * (int)(i < 2) - 1) };
		lights[i].clr = { (float)i / 4,1 - (float)i / 4,1 };
	}

	// Objects
	int planeSubdivs = 50;
	float planeSize = 10.0f;
	ew::Mesh planeMesh(ew::createPlane(planeSize, planeSize, planeSubdivs));
	ew::Transform planeTransform;
	planeTransform.position = ew::Vec3(0, -1.0, 0);

	// Other objects
	//ew::Mesh cubeMesh(ew::createCube(1.0f));
	//ew::Mesh sphereMesh(ew::createSphere(0.5f, 64));
	//ew::Mesh cylinderMesh(ew::createCylinder(0.5f, 1.0f, 32));
	//ew::Transform cubeTransform;
	//ew::Transform sphereTransform;
	//ew::Transform cylinderTransform;
	//ew::Transform torusTransform;
	//sphereTransform.position = ew::Vec3(-1.5f, 0.0f, 0.0f);
	//cylinderTransform.position = ew::Vec3(1.5f, 0.0f, 0.0f);

	resetCamera(camera,cameraController);

// Render Loop ----------------------------------------------------------*/
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		float deltaTime = time - prevTime;
		prevTime = time;

		// Update frame draw time
		float lastTime = 0;
		frameRate(timePerFrame, numFrames, lastTime, time);

		//Update camera
		camera.aspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
		cameraController.Move(window, &camera, deltaTime);

		//RENDER
		glClearColor(appSettings.bgColor.x, appSettings.bgColor.y, appSettings.bgColor.z,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

// Uniforms & Draw ------------------------------------------------------*/
		shader.use();
		glBindTexture(GL_TEXTURE_2D, brickTexture);

		// 
		setWave(shader, wave1);
		//shader.setFloat("_wave1.l", wave1.l);
		//shader.setFloat("_wave1.a", wave1.a);
		//shader.setFloat("_wave1.s", wave1.s);
		//shader.setFloat("alpha", wave1.alpha);
		//shader.setFloat("blend", wave1.blend);
		//shader.setVec3("_wave1.clr", wave1.clr);

		shader.setInt("_Texture", 0);

		shader.setInt("_Mode", appSettings.shadingModeIndex);
		shader.setFloat("_Time", (float)glfwGetTime());
		shader.setVec3("_UnshadedColor", wave1.clr);
		
		shader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());
		shader.setMaterial("_Material", mat);
		shader.setVec3("_ViewPos", camera.position);
		shader.setBool("_Phong", phong);
		shader.setInt("_NumLights", numLights);

		for (int i = 0; i < numLights; i++) {
			shader.setVec3("_Lights[" +	std::to_string(i) + "].pos", lights[i].transform.position);
			shader.setVec3("_Lights[" + std::to_string(i) + "].clr", lights[i].clr);
		}

		// Draw shapes
		shader.setMat4("_Model", planeTransform.getModelMatrix());
		planeMesh.draw((ew::DrawMode)appSettings.drawAsPoints);

		//shader.setMat4("_Model", cubeTransform.getModelMatrix());
		//cubeMesh.draw();
		//shader.setMat4("_Model", sphereTransform.getModelMatrix());
		//sphereMesh.draw();
		//shader.setMat4("_Model", cylinderTransform.getModelMatrix());
		//cylinderMesh.draw();

		// Render point lights
		if (appSettings.renderLights) {
			lightsShader.use();
			lightsShader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());
			for (int i = 0; i < numLights; i++) {
				lightsShader.setMat4("_Model", lights[i].transform.getModelMatrix());
				lightsShader.setVec3("_Color", lights[i].clr);
				lightMesh.draw();
			}
		}

// Render UI ------------------------------------------------------------*/
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");

			ImGui::Text(std::to_string(timePerFrame).c_str());

			// Change the shader
			if (ImGui::Checkbox("Lit", &appSettings.lit)){
				shader.reload(appSettings.vertPaths[appSettings.waterShaderIndex],
					appSettings.lit ? appSettings.fragPaths[appSettings.waterShaderIndex] : "assets/unlit.frag");
			}
			if (ImGui::Combo("Shader Select", &appSettings.waterShaderIndex, appSettings.shaderTypeNames, IM_ARRAYSIZE(appSettings.shaderTypeNames))) {
				shader.reload(appSettings.vertPaths[appSettings.waterShaderIndex], 
					appSettings.lit ? appSettings.fragPaths[appSettings.waterShaderIndex] : "assets/unlit.frag");
			}


			// Wave Settings
			if (ImGui::CollapsingHeader("Wave Properties")) {
				ImGui::DragFloat("Wavelength", &wave1.l, 0.01f, 0, 10);
				ImGui::DragFloat("Amplitude", &wave1.a, 0.01f, 0, 10);
				ImGui::DragFloat("Speed", &wave1.s, 0.01f, 0, 10);
				ImGui::DragFloat("Alpha", &wave1.alpha, 0.01f, 0, 10);
				ImGui::DragFloat("Blend", &wave1.blend, 0.01f, 0, 10);
				ImGui::ColorPicker3("Colour", &wave1.clr.x);
			}


			// View Modes
			if (!appSettings.lit) {
				ImGui::Combo("Shading mode", &appSettings.shadingModeIndex, appSettings.shadingModeNames, IM_ARRAYSIZE(appSettings.shadingModeNames));
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
			if (ImGui::SliderInt("Blend Mode", &appSettings.blendModeIndex, 0, 20))
				glBlendFunc(GL_SRC_ALPHA, appSettings.blendModes[appSettings.blendModeIndex]);

			// Lighting
			ImGui::Checkbox("Render Lights", &appSettings.renderLights);
			ImGui::Checkbox("Phong?", &phong);
			ImGui::SliderInt("# of Lights", &numLights, 0, MAX_LIGHTS);
			if (ImGui::CollapsingHeader("Lights")) {
				for (int i = 0; i < numLights; i++) {
					ImGui::PushID(i);
					ImGui::DragFloat3("Position", &lights[i].transform.position.x, 0.1f);
					ImGui::ColorEdit3("Colour", &lights[i].clr.x, 0.1f);
					ImGui::PopID();
				}
			}

			// Plane Properties
			if (ImGui::CollapsingHeader("Plane Properties")) {
				if (ImGui::SliderInt("Plane Subdivs", &planeSubdivs, 3, 100)) {
					planeMesh = ew::createPlane(planeSize, planeSize, planeSubdivs);
					planeMesh = planeMesh;
				}

				if (ImGui::SliderFloat("Plane Size", &planeSize, 1, 10)) {
					planeMesh = ew::createPlane(planeSize, planeSize, planeSubdivs);
					planeMesh = planeMesh;
				}
			}

			// Material Properties
			if (ImGui::CollapsingHeader("Material Properties")) {
				ImGui::DragFloat("Ambient K", &mat.ambientK, 0.01f, 0, 1);
				ImGui::DragFloat("Diffuse K", &mat.diffuseK, 0.01f, 0, 1);
				ImGui::DragFloat("Specular K", &mat.specularK, 0.01f, 0, 1);
				ImGui::DragFloat("Shininess", &mat.shininess, 0.1f, 0);
			}

			// Camera Properties
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
				ImGui::ColorEdit3("BG color", &appSettings.bgColor.x);
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


/// <summary>
/// Sets all relevant uniforms
/// </summary>
void setWave(ew::Shader& shader, jsc::Wave wave) {
	shader.setFloat("_wave1.l", wave.l);
	shader.setFloat("_wave1.a", wave.a);
	shader.setFloat("_wave1.s", wave.s);
	shader.setFloat("alpha", wave.alpha);
	shader.setFloat("blend", wave.blend);
	shader.setVec3("_wave1.clr", wave.clr);
}

/// <summary>
/// Calcs the number of ms between drawing frames
/// </summary>
void frameRate(float& timePerFrame, int& numFrames, float& lastTime, float time) {
	printf(std::to_string(numFrames).c_str());
	printf(std::to_string(lastTime).c_str());
	printf(std::to_string(time).c_str());
	printf(std::to_string(timePerFrame).c_str());
	printf("\n");
	
	numFrames++;

	// Update every second
	if (time - lastTime > 1) {
		timePerFrame = 1000.0 / double(numFrames);
		numFrames = 0;
		lastTime += 1;
	}

}