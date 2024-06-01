#pragma once
#include <memory>
#include "GameStateBase.h"
#include "Texture.h"
#include "Camera.h"
#include "Shaders.h"
#include "Model.h"
#include "Sprite2D.h"
#include "Text.h"
#include "Button.h"
#include "SpriteAnimation.h"

enum CharacterType
{
	C_BLACK = 29,
	C_BLUE	= 35,
	C_GREEN	= 41,
	C_RED	= 47,
	C_YELLOW=53
};
class GSChooseCharacter final : public GameStateBase
{
public:
	using GameStateBase::GameStateBase;
	~GSChooseCharacter() override;

	void Init() override;
	void Update(float deltaTime) override;
	void Draw() override;
	void Pause() override;
	void Resume() override;
	void Exit() override;

	void HandleEvent() override;
	void OnKey(unsigned char key, bool pressed) override;
	void OnMouseClick(int x, int y, unsigned char key, bool pressed) override;
	void OnMouseMove(int x, int y) override;
	void OnMouseScroll(int x, int y, short delta) override;
private:
	float m_time;

	std::vector<std::shared_ptr<Sprite2D>>	m_sprite2DList;
	std::vector<std::shared_ptr<Button>>	m_buttonList;
	std::shared_ptr<SpriteAnimation>		m_currentCharacter;
	std::shared_ptr<SpriteAnimation>		m_bulletAnimation;
};