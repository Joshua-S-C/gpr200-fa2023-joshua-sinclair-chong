#pragma once
#include "../ew/ewMath/mat4.h"
#include "../ew/ewMath/vec3.h"

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

	struct Transform {
		ew::Vec3 position = ew::Vec3(0.0, 0.0, 0.0);
		ew::Vec3 rotation = ew::Vec3(0.0, 0.0, 0.0);	// Degrees
		ew::Vec3 scale = ew::Vec3(1.0, 1.0, 1.0);

		ew::Mat4  getModelMatrix() const {
			return Translate(position) * RotateZ(rotation.z) * RotateX(rotation.x) * RotateY(rotation.y) *  Scale(scale);
		}
	};

}