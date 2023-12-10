#include "texture.h"
#include "external/glad.h"
#include "external/stb_image.h"

static int getTextureFormat(int numComponents) {
	switch (numComponents) {
	default:
		return GL_RGBA;
	case 3:
		return GL_RGB;
	case 2:
		return GL_RG;
	}
}
namespace ew {
	unsigned int loadTexture(const char* filePath, int wrapMode, int filterMode) {
		int width, height, numComponents;
		unsigned char* data = stbi_load(filePath, &width, &height, &numComponents, 0);
		if (data == NULL) {
			printf("Failed to load image %s", filePath);
			stbi_image_free(data);
			return 0;
		}
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		int format = getTextureFormat(numComponents);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode);

		float borderColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, NULL);
		stbi_image_free(data);

		printf("Loaded Texture: ");
		printf(filePath);
		printf("\n");

		return texture;
	}

	// Added
	unsigned int loadCubemap(const char* faces[]) {
		int width, height, numComponents;
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

		for (unsigned int i = 0; i < 6; i++) {
			unsigned char* data = stbi_load(faces[i], &width, &height, &numComponents, 0);
			if (data == NULL) {
				printf("Failed to load image %s", faces[i]);
				stbi_image_free(data);
				return 0;
			}

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, getTextureFormat(numComponents), width, height, 
				0, getTextureFormat(numComponents), GL_UNSIGNED_BYTE, data);

			stbi_image_free(data);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		printf("Loaded CubeMap: ");
		for (unsigned int i = 0; i < 6; i++) {
			printf(faces[i]);
			printf(", ");
		}
		printf("\n");

		return texture;
	}

}

