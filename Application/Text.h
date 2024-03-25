#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include "BaseObject.h"

class Text : public BaseObject
{
public:
	using BaseObject::BaseObject;
	Text(GLuint id);
	~Text();

	void Init(TTF_Font* font, const std::string& text, int wrapLength = 0);
	void SetFont(TTF_Font* font);
	void SetText(const std::string& text);
	void SetTextColor(Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha = 255);
	void Set2DSize(GLint x, GLint y);
	void Set2DScale(GLfloat x, GLfloat y);
	void Set2DPosition(GLint x, GLint y);
	void Draw() override;
	void Update(GLfloat deltaTime) override;
private:
	TTF_Font* m_font;
	std::string m_text;
	SDL_Color m_color;
	GLuint m_textTextureId;
	GLfloat m_scaleX, m_scaleY;

	void RenderFont();
};