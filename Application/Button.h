#pragma once

#include "Sprite2D.h"
enum ButtonType
{
	BUTTON_PLAY,
	BUTTON_EXIT,
	BUTTON_RESUM,
	BUTTON_BACK,
	BUTTON_NEXT,
	BUTTON_SETTING,
	BUTTON_CREDIT,
	BUTTON_EXIT_UPGRADE,
	BUTTON_BUY_BULLET_UPGRADE,
	BUTTON_BUY_ARMOR_UPGRADE,
	BUTTON_BUY_HP_UPGRADE,
	BUTTON_PLUS_ON_BULLET_UPGRADE,
	BUTTON_PLUS_OFF_BULLET_UPGRADE,
	BUTTON_PLUS_ON_ARMOR_UPGRADE,
	BUTTON_PLUS_OFF_ARMOR_UPGRADE,
	BUTTON_PLUS_ON_HP_UPGRADE,
	BUTTON_PLUS_OFF_HP_UPGRADE,
	BUTTON_HIGHSCORE,
	BUTTON_SAVE,
	BUTTON_CANCEL,
	BUTTON_SOUND,
	BUTTON_RETRY,
	BUTTON_MENU,
	BUTTON_CHOOSE_CHARACTER,
	BUTTON_NEXT_LEFT,
	BUTTON_NEXT_RIGHT,
	BUTTON_CREAT,
	BUTTON_JOIN,
	BUTTON_READY,
	BUTTON_START
};
class Button : public Sprite2D
{
public:
	Button(std::shared_ptr<Model> model, std::shared_ptr<Shaders> shader, std::shared_ptr<Texture> texture, ButtonType type);
	Button(const char* filename, ButtonType type);
	Button();
	~Button();

	bool HandleTouchMouse(GLfloat x, GLfloat y, bool bIsPressed);
	void HandleMoveMouse(GLfloat x, GLfloat y);
	void Set2DSize(GLint x, GLint y);
	ButtonType m_type;
private:

	GLint m_fWidth;
	GLint m_fHeight;
};