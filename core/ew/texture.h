#pragma once


namespace ew {
	unsigned int loadTexture(const char* filePath, int wrapMode, int filterMode);
	unsigned int loadCubemap(const char* faces[]);
}
