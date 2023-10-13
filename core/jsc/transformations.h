#pragma once
#include "../ew/ewMath/mat4.h"
#include "../ew/ewMath/vec3.h"
#include "../ew/ewMath/ewMath.h"

namespace jsc {
	// Identity Matrix
	inline ew::Mat4 Identity() {
		return ew::Mat4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		);
	};

	// Scaele on XYZ axes
	inline ew::Mat4 Scale(ew::Vec3 s) {
		return ew::Mat4(
			s.x, 0, 0, 0,
			0, s.y, 0, 0,
			0, 0, s.z, 0,
			0, 0, 0, 1
		);
	};

	// Rotation around X (pitch) in radians
	inline ew::Mat4 RotateX(float rad) {
		rad *= ew::DEG2RAD;
		return ew::Mat4(
			1        , 0        , 0         , 0,
			0        , cos(rad) , -sin(rad)	, 0,
			0        , sin(rad) , cos(rad)	, 0,
			0        , 0        , 0			, 1
		);
	};

	// Rotation around Y axis (yaw) in radians
	inline ew::Mat4 RotateY(float rad) {
		rad *= ew::DEG2RAD;
		return ew::Mat4(
			cos(rad) , 0        , sin(rad)  , 0,
			0        , 1        , 0        	, 0,
			-sin(rad), 0        , cos(rad)	, 0,
			0        , 0        , 0			, 1
		);
	};

	//Rotation around Z axis (roll) in radians
	inline ew::Mat4 RotateZ(float rad) {
		rad *= ew::DEG2RAD;
		return ew::Mat4(
			cos(rad) , -sin(rad), 0         , 0,
			sin(rad) , cos(rad) , 0        	, 0,
			0        , 0        , 1       	, 0,
			0        , 0        , 0			, 1
		);
	};

	//Translate x,y,z
	inline ew::Mat4 Translate(ew::Vec3 t) {
		return ew::Mat4(
			1  , 0  , 0  , t.x,
			0  , 1  , 0  , t.y,
			0  , 0  , 1  , t.z,
			0  , 0  , 0  , 1
		);
	};

	// Make right hand view space. 
	// eye = camera position
	// target = position to look at
	// up = up axis, usually (0, 1, 0)
	inline ew::Mat4 LookAt(ew::Vec3 eye, ew::Vec3 target, ew::Vec3 up) {
		ew::Vec3 f =	(eye - target)		/	(ew::Magnitude(eye - target));
		ew::Vec3 r =	ew::Cross(up, f)	/	ew::Magnitude(ew::Cross(up, f));
		up =			ew::Cross(f, r)		/	ew::Magnitude(ew::Cross(f, r));

		//ew::Mat4 rotation(
		//	r.x      , r.y      , r.z       , 0,
		//	up.x     , up.y     , up.z     	, 0,
		//	f.x      , f.y      , f.z     	, 0,
		//	0        , 0        , 0			, 1
		//);

		//ew::Mat4 translation(
		//	1        , 0        , 0         , -eye.x,
		//	up.x     , 1        , 0        	, -eye.y,
		//	0        , 0        , 1       	, -eye.z,
		//	0        , 0        , 0			, 1
		//);

		return ew::Mat4(
			r.x      , r.y      , r.z       , -ew::Dot(r, eye),
			up.x     , up.y     , up.z     	, -ew::Dot(up, eye),
			f.x      , f.y      , f.z     	, -ew::Dot(f, eye),
			0        , 0        , 0			, 1
		);


	}

	// Orthographic Projection
	inline ew::Mat4 Orthographic(float height, float aspect, float near, float far) {

	}

	// Perspective Projection
	// fov = vertical aspect ratio (radians)
	inline ew::Mat4 Perspective(float fov, float aspect, float near, float far) {

	}

	struct Transform {
		ew::Vec3 position = ew::Vec3(0.0, 0.0, 0.0);
		ew::Vec3 rotation = ew::Vec3(0.0, 0.0, 0.0);	// Degrees
		ew::Vec3 scale = ew::Vec3(1.0, 1.0, 1.0);

		ew::Mat4  getModelMatrix() const {
			return Translate(position) * RotateZ(rotation.z) * RotateX(rotation.x) * RotateY(rotation.y) *  Scale(scale);
		}
	};

	struct Camera {
		ew::Vec3 position;	// Camera body position
		ew::Vec3 target;	// Position to look at
		float fov;			// Vertical FOV (degrees)
		float aspectRatio;	// Screen Width / Screen Height
		float nearPlane;	// Near plane distance (+Z)
		float farPlane;		// Far plane distance (+Z)
		bool orthographic;	// Perspective or Ortho
		float orthoSize;	// Height of Ortho frustum
		
		// World -> View
		ew::Mat4 ViewMatrix() {			
			LookAt(position, target, ew::Vec3(0, 1, 0));
		}

		// View -> Clip
		ew::Mat4 ProjectionMatrix() {	
		
		}
	};
}