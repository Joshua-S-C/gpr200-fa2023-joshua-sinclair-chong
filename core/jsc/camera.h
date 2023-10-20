#pragma once
#include "../ew/ewMath/mat4.h"
#include "../ew/ewMath/vec3.h"
#include "../ew/ewMath/ewMath.h"

namespace jsc {
	// Uses functions from transformations.h
	struct Camera {
		ew::Vec3 position;	// Camera body position
		ew::Vec3 target;	// Position to look at
		float fov;			// Vertical FOV (degrees)
		float aspectRatio;	// Screen Width / Screen Height
		float nearPlane;	// Near plane distance (+Z)
		float farPlane;		// Far plane distance (+Z)
		bool orthographic;	// Perspective or Ortho
		float orthoSize;	// Height of Ortho frustum

		Camera::Camera() {
			position = { 0, 0, 5 };
			target = { 0, 0, 0 };
			fov = 60;
			aspectRatio = (float)ImGui::GetIO().DisplaySize.x / (float)ImGui::GetIO().DisplaySize.y;
			orthoSize = 6;
			nearPlane = 0.1;
			farPlane = 100;
			orthographic = false;
		};

		// World -> View
		ew::Mat4 ViewMatrix() {
			return LookAt(position, target, ew::Vec3(0, 1, 0));
		}

		// View -> Clip
		ew::Mat4 ProjectionMatrix() {
			return (orthographic) ? Orthographic(orthoSize, aspectRatio, nearPlane, farPlane) : Perspective(fov, aspectRatio, nearPlane, farPlane);
		}

		// Updates Aspect Ratio (that's the name)
		void UpdateAspectRatio() {
			aspectRatio = (float)ImGui::GetIO().DisplaySize.x / (float)ImGui::GetIO().DisplaySize.y;
		}

		// Reset to default values
		void Reset() {
			position = { 0, 0, 5 };
			target = { 0, 0, 0 };
			fov = 60;
			orthoSize = 6;
			nearPlane = 0.1;
			farPlane = 100;
			orthographic = false;
		}
	};

	struct CameraControls {
		double prevMouseX, prevMouseY;	// Mouse position from previous frame
		float yaw = 0, pitch = 0;		// Degrees
		float mouseSens = 0.5f;			// Mouse speed
		bool firstMouse = true;			// Flag for to store initial mouse position
		float moveSpd = 0.1f;			// its in the name
	};

	// TODO : Add controls like minecraft fly lol (Shift adds spd, space goes up, ctrl goes down)

	void moveCamera(GLFWwindow* window, Camera* cam, CameraControls* controls) {
		// If RMB not held, release cursor
		if (!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2)) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			controls->firstMouse = true;
			return;
		}

		// Hides cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);
		
		// Prevents bug where camera moves as soon as click
		if (controls->firstMouse) {
			controls->firstMouse = false;
			controls->prevMouseX = mouseX;
			controls->prevMouseY = mouseY;
		}

		// Add mouse delta to yaw and pitch, Clamp pitch 
		controls->yaw += (mouseX - controls->prevMouseX) * controls->mouseSens;
		controls->pitch -= (mouseY - controls->prevMouseY) * controls->mouseSens;

		if (controls->pitch > 89) controls->pitch = 89;
		if (controls->pitch < -89) controls->pitch = -89;

		controls->prevMouseX = mouseX;
		controls->prevMouseY = mouseY;

		// Copy of pitch & yaw cuz I don't like having a ton of math in my vector
		ew::Vec2 PY = { controls->yaw * ew::DEG2RAD, controls->pitch * ew::DEG2RAD };
		ew::Vec3 forward = {
			sin(PY.x) * cos(PY.y),
			sin(PY.y),
			-cos(PY.x) * cos(PY.y) };
		

		// Change camera position
		
		// Graham-schmidt !!
		// TODO: Change to use normalize
		ew::Vec3 right = ew::Cross(ew::Vec3{0,1,0}, forward) / ew::Magnitude(ew::Cross(ew::Vec3{ 0,1,0 }, forward));
		ew::Vec3 up = ew::Cross(right, forward) / ew::Magnitude(ew::Cross(right, forward));;

		// Keybord controls
		if (glfwGetKey(window, GLFW_KEY_W)) 
			cam->position += forward * controls->moveSpd;
		if (glfwGetKey(window, GLFW_KEY_S)) 
			cam->position -= forward * controls->moveSpd;

		if (glfwGetKey(window, GLFW_KEY_A)) 
			cam->position += right * controls->moveSpd;
		if (glfwGetKey(window, GLFW_KEY_D)) 
			cam->position -= right * controls->moveSpd;

		if (glfwGetKey(window, GLFW_KEY_Q)) 
			cam->position += up * controls->moveSpd;
		if (glfwGetKey(window, GLFW_KEY_E)) 
			cam->position -= up * controls->moveSpd;

		// TODO : More advanced controls


		cam->target = cam->position + forward;
	}
}