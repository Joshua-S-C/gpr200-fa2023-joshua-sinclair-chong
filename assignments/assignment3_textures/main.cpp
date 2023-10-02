#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/shader.h>
#include <jsc/shader.h>
#include <jsc/texture.h>

struct Vertex {
	float x, y, z;
	float u, v;
};

unsigned int createVAO(Vertex* vertexData, int numVertices, unsigned short* indicesData, int numIndices);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

const int SCREEN_WIDTH = 1080;	
const int SCREEN_HEIGHT = 720;

Vertex vertices[4] = {
	{-1.0, -1.0, 0.0, 0.0, 0.0},
	{1.0, -1.0, 0.0, 1.0, 0.0},
	{1.0, 1.0, 0.0, 1.0, 1.0},
	{-1.0, 1.0, 0.0, 0.0, 1.0}
};
unsigned short indices[6] = {
	0, 1, 2,
	2, 3, 0
};

int wrapModes[] = { GL_REPEAT,  GL_MIRRORED_REPEAT , GL_CLAMP_TO_EDGE , GL_CLAMP_TO_BORDER , GL_MIRROR_CLAMP_TO_EDGE };
int filterModes[] = { GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST , GL_NEAREST_MIPMAP_LINEAR , GL_LINEAR_MIPMAP_LINEAR };

// TODO : how to use these in the combo cuz strings and .c_str() dont work
std::string wrapModesList = "Repeat\0Mirrored Repeat\0Clamp to Edge\0Clamp to Border\0Mirror Clamp to Edge";
std::string filterModesList = "Nearest Neighboor\0Linear\0Nearest Mipmap-Nearest\0Linear Mipmap-Nearest\0Nearest Mipmap-Linear\0Linear Mipmap-Linear";

int main() {
// Initialize Window ----------------------------------------------------*/
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Textures", NULL, NULL);
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

// Create Shaders & Load Textures ---------------------------------------*/
	unsigned int quadVAO = createVAO(vertices, 4, indices, 6);
	glBindVertexArray(quadVAO);

	ew::Shader backgroundShader("assets/shaders/background.vert", "assets/shaders/background.frag");
	unsigned int noiseTexture = loadTexture("assets/Images/noise.png", GL_REPEAT, GL_LINEAR);
	unsigned int backgroundTexture = loadTexture("assets/Images/brick.png", GL_REPEAT, GL_LINEAR);
	unsigned int backgroundObjectTexture = loadTexture("assets/Images/smile2.png", GL_REPEAT, GL_LINEAR);
	//unsigned int backgroundTexture = loadTexture("assets/Images/underwaterImages2.png", GL_REPEAT, GL_LINEAR);
	//unsigned int backgroundObjectTexture = loadTexture("assets/Images/notNemo.png", GL_REPEAT, GL_LINEAR);

	ew::Shader characterShader("assets/shaders/character.vert", "assets/shaders/character.frag");
	unsigned int characterTexture = loadTexture("assets/Images/character.png", GL_CLAMP_TO_EDGE, GL_NEAREST);

// Initialize Uniforms --------------------------------------------------*/
	float distortionStrength = .1;
	float distortionSpeed = .1;
	float text2Alpha = .5;
	float noiseColour[] = {0.0, 0.0, 0.0};
	float noiseAlpha = 0.5;


	//float characterScale[] = { 0.5, 1.0 };	// For images that are 3:2
	float characterScale[] = { 0.25, 1.0 };		// For images that are 2:3, also now just using this as a constant
	float characterScaleRatio = characterScale[0] / characterScale[1];
	float characterSpeed = 1.0;
	float distance[2] = { 0.5, 0.5 };
	float characterOffset = 0.5;

// Render Loop ----------------------------------------------------------*/
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

// Background -----------------------------------------------------------*/
		backgroundShader.use();

		backgroundShader.setFloat("_Time", (float)glfwGetTime());
		backgroundShader.setFloat("_DistortionStrength", distortionStrength);
		backgroundShader.setFloat("_DistortionSpeed", distortionSpeed);
		backgroundShader.setFloat("_Text2Alpha", text2Alpha);
		backgroundShader.setFloat("_NoiseAlpha", noiseAlpha);
		backgroundShader.setVec3("_NoiseColour", noiseColour[0], noiseColour[1], noiseColour[2]);

		backgroundShader.setInt("_Texture", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, backgroundTexture);

		backgroundShader.setInt("_Texture2", 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, backgroundObjectTexture);

		backgroundShader.setInt("_Texture3", 2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, noiseTexture);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);

// Character ------------------------------------------------------------*/
		characterShader.use();

		characterShader.setFloat("_Time", (float)glfwGetTime());
		characterShader.setVec2("_Scale", characterScale[0], characterScale[1]);
		characterShader.setFloat("_ScaleRatio", characterScaleRatio);
		characterShader.setFloat("_Speed", characterSpeed);
		characterShader.setVec2("_Distance", distance[0], distance[1]);
		characterShader.setFloat("_VertOffset", characterOffset);

		characterShader.setInt("_Texture", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, characterTexture);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);
		

// Render UI ------------------------------------------------------------*/
	static int clicked = 0;
	{
		ImGui_ImplGlfw_NewFrame();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();

		ImGui::SetNextWindowPos({ 0,0 });
		ImGui::SetNextWindowSize({ 300, 720 });
		ImGui::Begin("Settings", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);

		// Background Settings
		if (ImGui::CollapsingHeader("Background")) {
		// Who would've thought that constantly reloading 3 big images would obliterate performance. 
		// TODO something about this. Using buttons only kinda works
		
			// Change wrap mode 
			static int wrapIndex = 0;
			ImGui::Combo("Wrap", &wrapIndex, "Repeat\0Mirrored Repeat\0Clamp to Edge\0Clamp to Border\0Mirror Clamp to Edge");
			// Change filter mode 
			static int filterIndex = 0;
			ImGui::Combo("Filter", &filterIndex, "Nearest Neighboor\0Linear\0Nearest Mipmap-Nearest\0Linear Mipmap-Nearest\0Nearest Mipmap-Linear\0Linear Mipmap-Linear");

			// Apply Filters
			clicked = 0;
			if (ImGui::Button("Apply Filters")) clicked++;
			if (clicked & 1) {
				backgroundTexture = loadTexture("assets/Images/brick.png", wrapModes[wrapIndex], filterModes[filterIndex]);
				backgroundObjectTexture = loadTexture("assets/Images/smile2.png", wrapModes[wrapIndex], filterModes[filterIndex]);
				noiseTexture = loadTexture("assets/Images/noise2.png", wrapModes[wrapIndex], filterModes[filterIndex]);
			}

			ImGui::SliderFloat("Object Alpha", &text2Alpha, .0, 1.0);
			ImGui::SliderFloat("Strength", &distortionStrength, .0, .5);
			ImGui::SliderFloat("Speed", &distortionSpeed, .0, .5);

			ImGui::SliderFloat("Noise Alpha", &noiseAlpha, .0, 1.0);
			ImGui::ColorPicker3("Noise Colour", noiseColour);

		}

		// Character Settings
		if (ImGui::CollapsingHeader("Character")) {
			// Change filter mode
			static int filterIndex = 0;
			ImGui::Combo("Filter", &filterIndex, filterModesList.c_str());
			// Apply Filters
			clicked = 0;
			if (ImGui::Button("Apply Filters")) clicked++;
			if (clicked & 1) {
				characterTexture = loadTexture("assets/Images/character.png", GL_CLAMP_TO_EDGE, filterModes[filterIndex]);
			}

			ImGui::SliderFloat("Speed", &characterSpeed, 0, 10);
			//ImGui::SliderFloat("X Scale", &characterScale[0], 0, 1);
			//ImGui::SliderFloat("Y Scale", &characterScale[1], 0, 1);
			ImGui::SliderFloat("Scale", &characterScaleRatio, 0, 1);
			ImGui::SliderFloat("X Distance", &distance[0], 0, 1);
			ImGui::SliderFloat("Y Distance", &distance[1], 0, 1);

			ImGui::SliderFloat("Offset", &characterOffset, -3, 2);
		}

		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
	} // End of UI

	}
	printf("Shutting down...");
}

unsigned int createVAO(Vertex* vertexData, int numVertices, unsigned short* indicesData, int numIndices) {
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//Vertex Buffer Object 
	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*numVertices, vertexData, GL_STATIC_DRAW);

	//Element Buffer Object
	unsigned int ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * numIndices, indicesData, GL_STATIC_DRAW);

	//Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex,x));
	glEnableVertexAttribArray(0);

	//UV attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(offsetof(Vertex, u)));
	glEnableVertexAttribArray(1);

	return vao;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

