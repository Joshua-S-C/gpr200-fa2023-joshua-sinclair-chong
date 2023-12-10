#pragma once
#include "../ew/ewMath/mat4.h"
#include "../ew/ewMath/vec3.h"
#include "../ew/ewMath/ewMath.h"

#include "../ew/external/glad.h"
#include "../ew/transform.h"
#include "../ew/mesh.h"
#include "../ew/texture.h"
#include "../ew/shader.h"
#include "../ew/procGen.h"


namespace jsc {
	struct Light {
		ew::Transform transform; // World Space
		ew::Vec3 clr; // RBG
	};

	struct Material {
		// Coefficients. 0-1
		float ambientK = .05;
		float diffuseK = .2;
		float specularK = .7;

		float shininess = 32;

		Material::Material(float ambient, float diffuse, float specular, float _shininess) {
			ambientK = ambient;
			diffuseK = diffuse;
			specularK = specular;
			shininess = _shininess;
		};
	};

	struct Skybox {
		//ew::Shader shader;
		ew::Mesh mesh;
		ew::Transform transform;
		unsigned int texture;
		const char* faces[6] = {
			"assets/skybox/right.jpg",	"assets/skybox/left.jpg", 
			"assets/skybox/top.jpg",	"assets/skybox/bottom.jpg", 
			"assets/skybox/front.jpg",	"assets/skybox/back.jpg"
			};

		Skybox::Skybox() {
			//shader.reload("assets/skybox.vert", "assets/skybox.frag");
			texture = ew::loadCubemap(faces);
			mesh.load(ew::createCube(100.0f));
		};

		void UpdatePosition(ew::Vec3 newPos) {
			transform.position = newPos;
		}
	};
}