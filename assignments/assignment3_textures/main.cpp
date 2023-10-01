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

int wrapModes[2] = { GL_REPEAT, GL_CLAMP_TO_EDGE };
int filterModes[2] = { GL_NEAREST, GL_LINEAR };

// TODO : how to use these in the combo cuz strings dont work
std::string wrapModesString = "Clamp to Edge\0Repeat";
std::string filterModesString = "Nearest Neighboor\0Linear";

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
	unsigned int brickTexture = loadTexture("assets/brick.png", GL_REPEAT, GL_LINEAR);
	unsigned int brick2Texture = loadTexture("assets/smile2.png", GL_REPEAT, GL_LINEAR);
	unsigned int noiseTexture = loadTexture("assets/noise2.png", GL_REPEAT, GL_LINEAR);

	ew::Shader characterShader("assets/shaders/character.vert", "assets/shaders/character.frag");
	unsigned int characterTexture = loadTexture("assets/character.png", GL_CLAMP_TO_EDGE, GL_NEAREST);

// Initialize Uniforms --------------------------------------------------*/
	float distortionStrength = .1;
	float distortionSpeed = .1;
	float text2Alpha = .5;
	float noiseColour[] = {0.0, 0.0, 0.0};
	float noiseAlpha = 0.5;


	float characterScale = 0.5;
	float characterSpeed = 0.5;
	float distanceX = 0.5;
	float distanceY = 0.5;

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
		glBindTexture(GL_TEXTURE_2D, brickTexture);

		backgroundShader.setInt("_Texture2", 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, brick2Texture);

		backgroundShader.setInt("_Texture3", 2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, noiseTexture);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);

// Character ------------------------------------------------------------*/
		characterShader.use();

		characterShader.setFloat("_Time", (float)glfwGetTime());
		characterShader.setFloat("_Scale", characterScale);
		characterShader.setFloat("_Speed", characterSpeed);
		characterShader.setFloat("_DistanceX", distanceX);
		characterShader.setFloat("_DistanceY", distanceY);

		characterShader.setInt("_Texture", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, characterTexture);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);
		

// Render UI ------------------------------------------------------------*/
	{
		ImGui_ImplGlfw_NewFrame();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();

		ImGui::SetNextWindowPos({ 0,0 });
		ImGui::SetNextWindowSize({ 250,400 });
		ImGui::Begin("Settings");

		// Background Settings
		if (ImGui::CollapsingHeader("Background")) {
		// Who would've thought that constantly reloading 3 big images would obliterate performance. 
		// TODO something about this
		
			//// Change wrap mode 
			//static int wrapIndex = 0;
			//ImGui::Combo("Wrap", &wrapIndex, "Clamp to Edge\0Repeat");
			//// Change filter mode 
			//static int filterIndex = 0;
			//ImGui::Combo("Filter", &filterIndex, "Nearest Neighboor\0Linear");
			//brickTexture = loadTexture("assets/brick.png", wrapModes[wrapIndex], filterModes[filterIndex]);
			//brick2Texture = loadTexture("assets/smile2.png", wrapModes[wrapIndex], filterModes[filterIndex]);
			//noiseTexture = loadTexture("assets/noise2.png", wrapModes[wrapIndex], filterModes[filterIndex]);

			ImGui::SliderFloat("Strength", &distortionStrength, .0, .5);
			ImGui::SliderFloat("Speed", &distortionSpeed, .0, .5);
			ImGui::SliderFloat("Alpha", &text2Alpha, .0, 1.0);
			ImGui::SliderFloat("Noise Alpha", &noiseAlpha, .0, 1.0);
			ImGui::ColorPicker3("Noise Colour", noiseColour);

		}

		// Character Settings
		if (ImGui::CollapsingHeader("Character")) {
			// Change wrap mode 
			static int wrapIndex = 0;
			ImGui::Combo("Wrap", &wrapIndex, "Clamp to Edge\0Repeat");
			// Change filter mode
			static int filterIndex = 0;
			ImGui::Combo("Filter", &filterIndex, "Nearest Neighboor\0Linear");

			characterTexture = loadTexture("assets/character.png", wrapModes[wrapIndex], filterModes[filterIndex]);

			ImGui::SliderFloat("Scale", &characterScale, 0, 1);
			ImGui::SliderFloat("Speed", &characterSpeed, 0, 1);
			ImGui::SliderFloat("X Distance", &distanceX, 0, 1);
			ImGui::SliderFloat("Y Distance", &distanceY, 0, 1);

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

