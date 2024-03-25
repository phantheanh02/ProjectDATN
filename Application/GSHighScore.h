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

class GSHighScore final : public GameStateBase
{
public:
	using GameStateBase::GameStateBase;
	~GSHighScore() override;

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
	std::shared_ptr<Sprite2D>	m_intro;
	std::shared_ptr<Sprite2D>	m_background;
	std::shared_ptr<Button>  m_button;
};