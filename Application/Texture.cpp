#include "stdafx.h"
#include <iostream>
#include <algorithm>
#include "Texture.h"
#include "stb_image.h"

#define STB_IMAGE_IMPLEMENTATION 

Texture::Texture() : m_textureId(0)
{
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_textureId);
}

void Texture::SetFilter(GLint filter)
{
	glBindTexture(GL_TEXTURE_2D, m_textureId);
	switch (filter)
	{
	case GL_NEAREST:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
		break;
	case GL_LINEAR:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
		break;
	default:
		std::cout << "Invalid filter mode\n";
		break;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::SetWrap(GLint wrapMode)
{
	glBindTexture(GL_TEXTURE_2D, m_textureId);
	switch (wrapMode)
	{
	case GL_CLAMP_TO_EDGE:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
		break;
	case GL_REPEAT:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
		break;
	default:
		std::cout << "Invalid wrap mode\n";
		break;
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLint Texture::LoadTexture(const std::string& filename)
{
	// check if file exist
	FILE* f;
	fopen_s(&f, filename.c_str(), "r");
	if (!f)
	{
		std::cerr << "Couldn't open file: " << filename << "\n";
		return -1;
	}
	fclose(f);

	GLint width;
	GLint height;
	GLint bpp;

	// load pixel data
	stbi_uc* pixelData = nullptr;
	pixelData = stbi_load(filename.c_str(), &width, &height, &bpp, 0);
	bpp *= 8;

	// bind texture
	glGenTextures(1, &m_textureId);
	glBindTexture(GL_TEXTURE_2D, m_textureId);

	// specify 2D texture
	switch (bpp)
	{
	case 24:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixelData);
		break;
	case 32:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
		break;
	}

	// set wrap filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// clean up
	glBindTexture(GL_TEXTURE_2D, 0);
	free(pixelData);

	return 0;
}
