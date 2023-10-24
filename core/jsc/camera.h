#pragma once
#include "../ew/ewMath/mat4.h"
#include "../ew/ewMath/vec3.h"
#include "../ew/ewMath/ewMath.h"

namespace jsc {
	struct Camera {
	// Uses functions from transformations.h
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
			return jsc::LookAt(position, target, ew::Vec3(0, 1, 0));
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
		float yaw, pitch;	// Degrees
		float mouseSens;	// Mouse speed
		bool firstMouse;	// Flag for to store initial mouse position
		float moveSpd;		// its in the name
		float sprintSpd;	// When sprinting
		bool enabled;		// If controls are enabled. Only turned off for the orbiting // TODO : Implement this

		CameraControls::CameraControls() {
			yaw = 0, pitch = 0;
			mouseSens = 0.3f;
			firstMouse = true;
			moveSpd = 0.05f;
			sprintSpd = 0.1f;
		}

		// Reset to default values
		void Reset() {
			yaw = 0, pitch = 0;		
			mouseSens = 0.3f;			
			firstMouse = false;			
			moveSpd = 0.05f;	
			sprintSpd = 0.1f;
		}
	};

	void moveCamera(GLFWwindow* window, Camera* cam, CameraControls* controls, float deltaTime) {
		// If RMB not held, release cursor
		if (!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2)) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			controls->firstMouse = true;
			return;
		}

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	// Hides cursor

		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);
		
		// Prevents bug where camera moves as soon as click
		if (controls->firstMouse) {
			controls->firstMouse = false;
			controls->prevMouseX = mouseX;
			controls->prevMouseY = mouseY;
		}

		// Add mouse delta to yaw and pitch, clamp pitch 
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
		ew::Vec3 right = ew::Normalize(ew::Cross(ew::Vec3{ 0,1,0 }, forward));
		ew::Vec3 up = ew::Normalize(ew::Cross(right, forward));

		// Keybord controls
		float movement = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) ? controls->sprintSpd : controls->moveSpd) * deltaTime;

		if (glfwGetKey(window, GLFW_KEY_W)) cam->position += forward * movement;
		if (glfwGetKey(window, GLFW_KEY_S)) cam->position -= forward * movement;

		if (glfwGetKey(window, GLFW_KEY_A)) cam->position += right * movement;
		if (glfwGetKey(window, GLFW_KEY_D)) cam->position -= right * movement;

		if (glfwGetKey(window, GLFW_KEY_Q)) cam->position += up * movement;
		if (glfwGetKey(window, GLFW_KEY_E)) cam->position -= up * movement;

		// TODO : More advanced controls


		cam->target = cam->position + forward;
	}

	// TODO : Maaaybe make it tween (smooth transiton)
	// Currently broekn it makes the camera really fast after focusing for some reason (somethingto do with setting the target I think)
	void cameraFocus(Camera* cam, CameraControls* controls, float deltaTime, ew::Vec3 newTarget) {
		// Target Set
		cam->target = newTarget;

		// Set camera position
		ew::Vec2 PY = { controls->yaw * ew::DEG2RAD, controls->pitch * ew::DEG2RAD };
		ew::Vec3 forward = {
			sin(PY.x) * cos(PY.y),
			sin(PY.y),
			-cos(PY.x) * cos(PY.y) };

		cam->position = cam->target - forward;
	}

	// TODO : Add a function to move the camera around an object (don't change the target, like the orbiting thing)

}