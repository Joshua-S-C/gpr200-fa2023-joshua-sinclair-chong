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
#define MAX_WAVES 10

int numLights = 2;
int numWaves = 4;

int numFrames = 0;
float timePerFrame = 0;

int SCREEN_WIDTH = 1080;
int SCREEN_HEIGHT = 720;
float prevTime;

// Wave Helper Functions
void setWave(ew::Shader& shader, jsc::Wave wave, std::string name = "_wave");
void setWave(ew::Shader& shader, jsc::GWave wave, std::string name = "_wave");
void setWaves(ew::Shader& shader, jsc::Wave wave[], int numWaves, std::string name = "_waves");
void setWaves(ew::Shader& shader, jsc::GWave wave[], int numWaves, std::string name = "_waves");

// Other Helper Functions
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void resetCamera(ew::Camera& camera, ew::CameraController& cameraController);
void frameRate(float& timePerFrame, int& numFrames, float& lastTime, float time);

struct AppSettings {
	const char* shaderTypeNames[2] = { "Simple Sin", "Gerstner"};
	int waterShaderIndex = 1;
	const char* vertPaths[2] = { "assets/water/simpleSin.vert", "assets/water/gerstner.vert" };
	const char* fragPaths[2] = { "assets/water/simpleSin.frag", "assets/water/gerstner.frag" };

	const char* shadingModeNames[4] = { "Solid Color","Normals","UVs","Texture"};
	int shadingModeIndex = 0;

	ew::Vec3 bgColor = ew::Vec3(0.1f);
//	ew::Vec3 shapeColor = ew::Vec3(1.0f);	//Unsused
	
	int blendModeIndex = 7;
	int blendModes[20] = 
	{ GL_ZERO ,GL_ONE ,GL_SRC_COLOR ,GL_ONE_MINUS_SRC_COLOR ,GL_DST_COLOR ,GL_ONE_MINUS_DST_COLOR ,GL_SRC_ALPHA ,GL_ONE_MINUS_SRC_ALPHA ,GL_DST_ALPHA ,GL_ONE_MINUS_DST_ALPHA ,GL_CONSTANT_COLOR ,GL_ONE_MINUS_CONSTANT_COLOR ,GL_CONSTANT_ALPHA ,GL_ONE_MINUS_CONSTANT_ALPHA ,GL_SRC_ALPHA_SATURATE ,GL_SRC1_COLOR ,GL_ONE_MINUS_SRC1_COLOR ,GL_SRC1_ALPHA ,GL_ONE_MINUS_SRC1_ALPHA };

	bool lit = true;
	bool phong = false;
	bool wireframe = false;
	bool drawAsPoints = false;
	bool backFaceCulling = true;

	bool renderLights = false;
	bool renderTerrain = false;
}appSettings;

ew::Camera camera;
ew::CameraController cameraController;

int main() {
// Initialize Window and Stuf -------------------------------------------*/
	printf("Salut\n");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Later Undulater!", NULL, NULL);
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
	glCullFace(GL_BACK);	// Changed for skybox
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, appSettings.blendModes[appSettings.blendModeIndex]);

	unsigned int brickTexture = ew::loadTexture("assets/brick_color.jpg",GL_REPEAT,GL_LINEAR);  // this is just here lol incase i want to look at lovely bricks
// Initializations for Waves --------------------------------------------*/

	ew::Shader shader("assets/water/gerstner.vert", "assets/water/gerstner.frag");	// Wave shader
	jsc::Material mat(.02, .3, .9, 32);

	jsc::Wave simpleWave(2.0f, 0.3f, 1.0f, 1.0f, 1.0f, ew::Vec3{ .5f,.8f,1 });
	jsc::GWave gerstnerWave(10.0f, 0.3f, ew::Vec2{ 0.0, 0.1 }, ew::Vec3{ .5f,.8f,1 }, 0.45f, .05f);

	jsc::GWave gWaves[MAX_WAVES];
	gerstnerWave.populate(gWaves, MAX_WAVES);

	// Mesh
	int planeSubdivs = 600;
	float planeSize = 200.0f;
	ew::Mesh planeMesh(ew::createPlane(planeSize, planeSize, planeSubdivs));
	ew::Transform planeTransform;
	planeTransform.position = ew::Vec3(0, -1.0, 0);

// Initializations for Terrain ------------------------------------------*/
	ew::Shader terrainShader("assets/heightmap/heightmap.vert", "assets/heightmap/heightmap.frag");
	//jsc::Material terrainMat(.4, .2, .7, 32); // Material for terrain
	ew::Transform terrainTransform;
	ew::Mesh terrainMesh(ew::createHeightmap("assets/heightmap/lake_champlain_height_highres.png", 1, 2.0f));
	terrainTransform.position = {0,29.5, 0};
	bool useTerrainTexture = false;
	unsigned int terrainTexture = ew::loadTexture("assets/heightmap/lake_champlain_texture.png", GL_REPEAT, GL_LINEAR);

// Initializations for Lights -------------------------------------------*/
	ew::Shader lightsShader("assets/lights.vert", "assets/lights.frag");
	ew::Mesh lightMesh(ew::createSphere(0.2f, 32));
	jsc::Light lights[4];
	lights[0].transform.position = { 100,20,5 };
	lights[0].clr = { 1,1,1 };
	for (int i = 1; i < numLights; i++) {
		lights[i].transform.position = { (float)(2 * (i % 2) - 1),1,(float)(2 * (int)(i < 2) - 1) };
		lights[i].clr = { (float)i / 4,1 - (float)i / 4,1 };
	}

// Initializations for Skybox -------------------------------------------*/
	jsc::Skybox skybox;
	ew::Shader skyboxShader("assets/skybox/skybox.vert", "assets/skybox/skybox.frag");

// Render Loop ----------------------------------------------------------*/
	resetCamera(camera,cameraController);
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		float deltaTime = time - prevTime;
		prevTime = time;

		// Update frame draw time
		float lastTime = 0;
		frameRate(timePerFrame, numFrames, lastTime, time);

		// Update camera & Skybox
		camera.aspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
		cameraController.Move(window, &camera, deltaTime);
		skybox.UpdatePosition(camera.position);

		// RENDER
		glClearColor(appSettings.bgColor.x, appSettings.bgColor.y, appSettings.bgColor.z,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

// Uniforms & Draw ------------------------------------------------------*/
		// Render Skybox
		glCullFace(GL_FRONT);
		glDepthMask(GL_FALSE);

		skyboxShader.use();
		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.texture);
		skyboxShader.setInt("_Texture", 0);
		skyboxShader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());
		skyboxShader.setMat4("_Model", skybox.transform.getModelMatrix());
		
		skybox.mesh.draw((ew::DrawMode)appSettings.drawAsPoints);
		
		// Reset GL settings after skybox
		glCullFace(GL_BACK);
		glDepthMask(GL_TRUE);
		
		// Render Point Lights
		if (appSettings.renderLights) {
			lightsShader.use();
			lightsShader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());
			for (int i = 0; i < numLights; i++) {
				lightsShader.setMat4("_Model", lights[i].transform.getModelMatrix());
				lightsShader.setVec3("_Color", lights[i].clr);
				lightMesh.draw((ew::DrawMode)appSettings.drawAsPoints);
			}
		}

		// Render Terrain
		if (appSettings.renderTerrain) {
			terrainShader.use();
			terrainShader.setInt("_UseTexture", useTerrainTexture);
			glBindTexture(GL_TEXTURE_2D, terrainTexture);
			terrainShader.setInt("_Texture", 0);

			terrainShader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());
			terrainShader.setVec3("_ViewPos", camera.position);
			terrainShader.setMat4("_Model", terrainTransform.getModelMatrix());

			terrainShader.setInt("_Mode", appSettings.shadingModeIndex);
			
			//terrainShader.setMaterial("_Material", terrainMat);
			//terrainShader.setBool("_Phong", appSettings.phong);
			//terrainShader.setInt("_NumLights", numLights);
			//for (int i = 0; i < numLights; i++) {
			//	terrainShader.setVec3("_Lights[" + std::to_string(i) + "].pos", lights[i].transform.position);
			//	terrainShader.setVec3("_Lights[" + std::to_string(i) + "].clr", lights[i].clr);
			//}

			terrainMesh.draw((ew::DrawMode)appSettings.drawAsPoints);
		}

		// Render Waves
		shader.use();
		
		switch (appSettings.waterShaderIndex) {
			case 0:
				setWave(shader, simpleWave);
				break;
			case 1:
				shader.setInt("_NumWaves", numWaves);
				setWave(shader, gerstnerWave);
				setWaves(shader, gWaves, MAX_WAVES);
				break;
		}

		glBindTexture(GL_TEXTURE_2D, brickTexture);
		shader.setInt("_Texture", 0);

		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.texture);
		shader.setInt("_Skybox", 0);

		shader.setInt("_Mode", appSettings.shadingModeIndex);
		shader.setFloat("_Time", (float)glfwGetTime());
		shader.setVec3("_UnshadedColor", simpleWave.clr);
		
		shader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());
		shader.setVec3("_ViewPos", camera.position);
		shader.setMaterial("_Material", mat);
		shader.setBool("_Phong", appSettings.phong);
		shader.setInt("_NumLights", numLights);

		for (int i = 0; i < numLights; i++) {
			shader.setVec3("_Lights[" +	std::to_string(i) + "].pos", lights[i].transform.position);
			shader.setVec3("_Lights[" + std::to_string(i) + "].clr", lights[i].clr);
		}

		shader.setMat4("_Model", planeTransform.getModelMatrix());
		planeMesh.draw((ew::DrawMode)appSettings.drawAsPoints);

// Render UI ------------------------------------------------------------*/
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::SetNextWindowPos({ 0,0 });
			ImGui::SetNextWindowSize({ 300, (float)SCREEN_HEIGHT });
			ImGui::Begin("Settings", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);


			//// Debug Text
			//bool debug;
			//ImGui::Checkbox("Debug List", &debug);
			//if (debug) {
			//	ImGui::Text(std::to_string(appSettings.lit).c_str());
			//}

			// Change # of Waves (duh.)
			ImGui::SliderInt("# of Waves", &numWaves, 0, MAX_WAVES);

			// Change Shader
			if (ImGui::Checkbox("Lit", &appSettings.lit)){
				shader.reload(appSettings.vertPaths[appSettings.waterShaderIndex],
					appSettings.lit ? appSettings.fragPaths[appSettings.waterShaderIndex] : "assets/unlit.frag");

				terrainShader.reload("assets/heightmap/heightmap.vert", appSettings.lit ? "assets/heightmap/heightmap.frag" : "assets/unlit.frag");
			}

			if (ImGui::Combo("Shader Select", &appSettings.waterShaderIndex, appSettings.shaderTypeNames, IM_ARRAYSIZE(appSettings.shaderTypeNames))) {
				shader.reload(appSettings.vertPaths[appSettings.waterShaderIndex], 
					appSettings.lit ? appSettings.fragPaths[appSettings.waterShaderIndex] : "assets/unlit.frag");

				terrainShader.reload("assets/heightmap/heightmap.vert", appSettings.lit ? "assets/heightmap/heightmap.frag" : "assets/unlit.frag");
			}

			// Wave Settings
			switch (appSettings.waterShaderIndex) {
				case 0:
					if (ImGui::CollapsingHeader("Simple Wave Properties")) {
						ImGui::DragFloat("Wavelength", &simpleWave.l, 0.01f, 0, 10);
						ImGui::DragFloat("Amplitude", &simpleWave.a, 0.01f, 0, 10);
						ImGui::DragFloat("Speed", &simpleWave.s, 0.01f, 0, 10);
						ImGui::DragFloat("Alpha", &simpleWave.alpha, 0.01f, 0, 10);
						ImGui::DragFloat("Blend", &simpleWave.blend, 0.01f, 0, 10);
						ImGui::ColorPicker3("Colour", &simpleWave.clr.x);
					}
					break;
				case 1:
					if (ImGui::CollapsingHeader("Gerstner Wave Properties")) {
						ImGui::DragFloat("Alpha", &gWaves[0].alpha, 0.01f, 0, 10);
						ImGui::DragFloat("Blend", &gWaves[0].blend, 0.01f, 0, 10);
						ImGui::Spacing();

						for (int i = 0; i < numWaves; i++) {
							ImGui::PushID(i);
							ImGui::DragFloat("Wavelength", &gWaves[i].l, 0.01f, 0, 10);
							ImGui::DragFloat("Gravity", &gWaves[i].g, 0.01f, 0, 10);
							ImGui::DragFloat2("Direction", &gWaves[i].dir.x, 0.01f, 0, 10);
							ImGui::Spacing();

							//ImGui::DragFloat("Steepness", &gWaves[i].s, 0.01f, 0, 10);
							//ImGui::ColorEdit3("Colour", &gWaves[i].clr.x);
							ImGui::PopID();
						}
					}
					break;
			}
				
			// How It Renders
			if (ImGui::CollapsingHeader("Render Properties")) {
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
			}

			// Lighting
			if (ImGui::CollapsingHeader("Lighting")) {
				ImGui::Checkbox("Render Lights", &appSettings.renderLights);
				ImGui::Checkbox("Phong?", &appSettings.phong);
				ImGui::SliderInt("# of Lights", &numLights, 0, MAX_LIGHTS);
				if (ImGui::CollapsingHeader("Lights")) {
					for (int i = 0; i < numLights; i++) {
						ImGui::PushID(i);
						ImGui::DragFloat3("Position", &lights[i].transform.position.x, 0.1f);
						ImGui::ColorEdit3("Colour", &lights[i].clr.x, 0.1f);
						ImGui::PopID();
					}
				}
			}

			// Terrain
			if (ImGui::CollapsingHeader("Terrain")) {
				ImGui::Checkbox("Render Terrain", &appSettings.renderTerrain);
				ImGui::Checkbox("Use Texture", &useTerrainTexture);
				if (ImGui::DragFloat("Scale", &terrainTransform.scale.x, 0.05f)) {
					terrainTransform.scale.y = terrainTransform.scale.x;
					terrainTransform.scale.z = terrainTransform.scale.x;
				}
				ImGui::DragFloat3("T-Position", &terrainTransform.position.x, 0.1f);

			}

			// Plane Properties
			if (ImGui::CollapsingHeader("Plane")) {
				if (ImGui::SliderInt("Plane Subdivs", &planeSubdivs, 3, 1500)) {
					planeMesh = ew::createPlane(planeSize, planeSize, planeSubdivs);
					planeMesh = planeMesh;
				}

				if (ImGui::SliderFloat("Plane Size", &planeSize, 1, 1500)) {
					planeMesh = ew::createPlane(planeSize, planeSize, planeSubdivs);
					planeMesh = planeMesh;
				}
			}

			// Material Properties
			if (ImGui::CollapsingHeader("Material")) {
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
	printf("Sayonara\n");
}


// Wave Helper Functions ------------------------------------------------*/

/// <summary>
/// Simple waves. Sets all relevant uniforms 
/// </summary>
void setWave(ew::Shader& shader, jsc::Wave wave, std::string name) {
	shader.setFloat(name + ".l", wave.l);
	shader.setFloat(name + ".a", wave.a);
	shader.setFloat(name + ".s", wave.s);
	shader.setFloat("alpha", wave.alpha);
	shader.setFloat("blend", wave.blend);
	shader.setVec3(name + ".clr", wave.clr);
}

/// <summary>
/// Gerstner wave. Sets all relevant uniforms
/// </summary>
void setWave(ew::Shader& shader, jsc::GWave wave, std::string name) {
	shader.setFloat(name + ".l", wave.l);
	shader.setFloat(name + ".s", wave.s);
	shader.setFloat(name + ".g", wave.g);
	shader.setVec2(name + ".dir", wave.dir);
	shader.setVec3(name + ".clr", wave.clr);
}

/// <summary>
/// Set array of Simple Waves.
/// </summary>
void setWaves(ew::Shader& shader, jsc::Wave wave[], int numWaves, std::string name) {
	for (int i = 0; i < numWaves; i++) {
		setWave(shader, wave[i], name + "[" + std::to_string(i) + "]");
	}
}

/// <summary>
/// Set array of Gerstner waves.
/// </summary>
void setWaves(ew::Shader& shader, jsc::GWave wave[], int numWaves, std::string name) {
	shader.setFloat("alpha", wave[0].alpha);
	shader.setFloat("blend", wave[0].blend);
	for (int i = 0; i < numWaves; i++) {
		setWave(shader, wave[i], name + "[" + std::to_string(i) + "]");
	}
}

// Other Helper Functions -----------------------------------------------*/

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
	camera.farPlane = 1000.0f;
	camera.orthographic = false;

	cameraController.yaw = 0.0f;
	cameraController.pitch = 0.0f;
	cameraController.sprintMoveSpeed = 50.0f;
}

/// <summary>
/// Calcs the number of ms between drawing frames
/// I forgot about this, it doens't work.
/// </summary>
void frameRate(float& timePerFrame, int& numFrames, float& lastTime, float time) {
	//printf(std::to_string(numFrames).c_str());
	//printf(std::to_string(lastTime).c_str());
	//printf(std::to_string(time).c_str());
	//printf(std::to_string(timePerFrame).c_str());
	//printf("\n");
	
	numFrames++;

	// Update every second
	if (time - lastTime > 1) {
		timePerFrame = 1000.0 / double(numFrames);
		numFrames = 0;
		lastTime += 1;
	}

}