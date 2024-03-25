#pragma once
#include "stdafx.h"
#include "../Utilities/utilities.h"
#include <string>

class Texture
{
public:
	Texture();
	~Texture();
	GLint LoadTexture(const std::string& filename);

	void SetFilter(GLint filter);
	void SetWrap(GLint wrapMode);
	void SetID(GLint id) { m_id = id; };

	GLint  GetID() { return m_id; };
	GLuint GetTextureId() { return m_textureId; };

private:
	GLuint	m_textureId;
	GLint	m_id;
};