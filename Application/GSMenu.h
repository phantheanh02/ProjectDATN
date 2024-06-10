#pragma once
#include <memory>
#include "GameStateBase.h"
#include "Sprite2D.h"
#include "SpriteAnimation.h"
#include "Button.h"

class GSMenu final : public GameStateBase
{
public:
	using GameStateBase::GameStateBase;
	~GSMenu() override;

	void Init() override;
	void Update(float deltaTime) override;
	void Draw() override;
	void Pause() override;
	void Resume() override;
	void Exit() override;

	void HandleEvent();
	void OnKey(unsigned char key, bool pressed) override;
	void OnMouseClick(int x, int y, unsigned char key, bool pressed) override;
	void OnMouseMove(int x, int y) override;
	void OnMouseScroll(int x, int y, short delta) override;
private:
	unsigned int m_key;
	unsigned int m_mouse;

	std::vector<std::shared_ptr<Button>> m_buttonList;
	std::shared_ptr<Sprite2D> m_background;
};