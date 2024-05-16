#include "stdafx.h"
#include "Text.h"
#include "ResourcesManager.h"
#include <iostream>
#include "Vertex.h"

Text::Text(GLuint id)
{
	m_id = id;
	m_model = ResourcesManager::GetInstance()->GetModel(ModelType::R_RETANGLE_TOPRIGHT);
	m_shader = ResourcesManager::GetInstance()->GetShader(0);
	m_textTextureId = 0;
	m_texture = nullptr;
}

Text::~Text()
{
	glDeleteTextures(1, &m_textTextureId);
}

void Text::Init(TTF_Font* font, const std::string& text, int wrapLength)
{
	m_font = font;
	m_text = text;
	m_color = { 0, 0, 0, 255 };
	m_changed = false;
	m_wvpMatrix = Matrix(0.0f);
	//RenderFont();
	//CalculateWVPMatrix();
}

void Text::SetFont(TTF_Font* font)
{
	m_font = font;
	m_changed = true;
}

void Text::SetText(const std::string& text)
{
	if (m_text == text)
	{
		return;
	}
	m_text = text;
	m_changed = true;
}

void Text::SetTextColor(Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha)
{
	m_color = { blue, green, red, alpha };
	m_changed = true;
}

void Text::Set2DSize(GLint x, GLint y)
{
	auto scale = Vector3(x, y, 1.0f);
	SetScale(scale);
	m_changed = true;
}

void Text::Set2DScale(GLfloat x, GLfloat y)
{
	m_scaleX = x;
	m_scaleY = y;
	SetScale(Vector3(m_scaleX * m_scale.x, m_scaleY * m_scale.y, 1.0f));
	m_changed = true;
}

void Text::Set2DPosition(GLint x, GLint y)
{
	auto pos = Vector3(x, y, 0.0f);
	m_pos = m_translation = pos;
	m_changed = true;
}

void Text::Draw()
{
	glUseProgram(m_shader->program);
	glBindBuffer(GL_ARRAY_BUFFER, m_model->GetVBOid());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_model->GetIBOid());
	glBindTexture(GL_TEXTURE_2D, m_textTextureId);

	if (m_changed || m_camera->m_changed)
	{
		// TODO: re-render text
		RenderFont();
		CalculateWVPMatrix();
		m_changed = false;
	}

	if (m_shader->positionAttribute != -1)
	{
		glEnableVertexAttribArray(m_shader->positionAttribute);
		glVertexAttribPointer(m_shader->positionAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	}

	if (m_shader->uvAttribute != -1)
	{
		glEnableVertexAttribArray(m_shader->uvAttribute);
		glVertexAttribPointer(m_shader->uvAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vector3) * 4));
	}

	if (m_shader->uniformWVP != -1)
	{
		glUniformMatrix4fv(m_shader->uniformWVP, 1, GL_FALSE, m_wvpMatrix.m[0]);
	}

	if (m_shader->iTextureLoc != -1)
	{
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(m_shader->iTextureLoc, 0);
	}

	if (m_shader->uniformAlpha != -1)
	{
		glUniform1f(m_shader->uniformAlpha, m_alpha);
	}

	glDrawElements(GL_TRIANGLES, m_model->GetNumIndices(), GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Text::Update(GLfloat deltaTime)
{

}

void Text::RenderFont()
{
	//SDL_Surface* surface = TTF_RenderUTF8_Blended(m_font, m_text.c_str(), m_color);
	SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(m_font, m_text.c_str(), m_color, 0);
	if (!surface)
	{
		std::cerr << "Error when rendering text: " << TTF_GetError() << "\n";
		return;
	}

	GLint mode = GL_RGB;
	if (surface->format->BytesPerPixel == 4)
	{
		mode = GL_RGBA;
	}
	m_scale.x = surface->w * m_scaleX;
	m_scale.y = surface->h * m_scaleY;

	if (m_textTextureId)
	{
		glDeleteTextures(1, &m_textTextureId);
		m_textTextureId = 0;
		//std::cout << glGetError() << "\n";
	}

	glGenTextures(1, &m_textTextureId);
	glBindTexture(GL_TEXTURE_2D, m_textTextureId);

	// code ma gi�o
	Uint32 len = surface->w * surface->format->BytesPerPixel;
	Uint8* src = static_cast<Uint8*>(surface->pixels);
	Uint8* dst = static_cast<Uint8*>(surface->pixels);
	for (int i = 0; i < surface->h; i++) {
		SDL_memmove(dst, src, len);
		dst += len;
		src += surface->pitch;
	}
	surface->pitch = len;
	//

	glTexImage2D(GL_TEXTURE_2D, 0, mode, surface->w, surface->h, 0, mode, GL_UNSIGNED_BYTE, surface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	glBindTexture(GL_TEXTURE_2D, 0);
	SDL_FreeSurface(surface);
	// Clean up
	//delete[] convertedPixels;
}