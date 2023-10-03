#include "texture.h"
#include "../ew/external/stb_image.h"
#include "../ew/external/glad.h"

unsigned int loadTexture(const char* filePath, int wrapMode, int filterMode) {
	stbi_set_flip_vertically_on_load(true);
	
	int width, height, numComponents;
	unsigned char* data = stbi_load(filePath, &width, &height, &numComponents, 0);

	//Automatically set format and internal format based on number of components (check table in docs and type)
	int colorFormat;
	switch (numComponents) {
	case 1: colorFormat = GL_RED;
		break;
	case 2: colorFormat = GL_RG;
		break;
	case 3: colorFormat = GL_RGB;
		break;
	case 4: colorFormat = GL_RGBA;
		break;
	default:
		break;
	}

	if (data == NULL) {
		printf("Failed to load image %s", filePath);
		stbi_image_free(data);
		return 0;
	}

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, colorFormat, width, height, 0, colorFormat, GL_UNSIGNED_BYTE, data);

	// Set texture properties
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode);


	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(data);
	return texture;
}
