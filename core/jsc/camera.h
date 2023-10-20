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
			aspectRatio = 1080 / 720;
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
	};

	struct CameraControls {
		double prevMouseX, prevMouseY;	// Mouse position from previous frame
		float yaw = 0, pitch = 0;		// Degrees
		float mouseSens = 0.1f;			// Mouse speed
		bool firstMouse = true;			// Flag for to store initial mouse position
		float moveSpd = 5.0f;			// its in the name
	};

	void moveCamera(GLFWwindow* window, Camera* cam, CameraControls* controls) {

	}
}