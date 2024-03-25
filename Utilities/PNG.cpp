#include "stdafx.h"
#include "PNG.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "../Application/stb_image.h"

char* LoadPNG(const std::string& filename, int* width, int* height, int* bpp)
{
	int channel;
	GLubyte * pixelData = stbi_load(filename.c_str(), width, height, &channel, 0);
	if (!pixelData)
	{
		std::cerr << "Couldn't load PNG file: " << filename << "\n";
		return nullptr;
	}
	*bpp = channel * 8;
	char* returnData = (char*)pixelData;
	return returnData;
}
