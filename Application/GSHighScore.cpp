#include "stdafx.h"
#include "GSHighScore.h"
#include "ResourcesManager.h"
#include "SceneManager.h"
#include "GameStateMachine.h"

GSHighScore::~GSHighScore()
{
}

void GSHighScore::Init()
{
	// button
	m_button = std::make_shared<Button>("btn_back.png", BUTTON_BACK);
	m_button->Set2DSize(110 * 960 / 608.0f, 35 * 960 / 608.0f);
	m_button->Set2DPosition(479 * 960 / 608.0f, 18 * 960 / 608.0f);

	// bg
	m_background = std::make_shared<Sprite2D>("state_background.png");
	m_background->Set2DSize(Globals::screenWidth, Globals::screenHeight);
	m_background->AttachCamera(SceneManager::GetInstance()->GetCamera(CameraType::STATIC_CAMERA));
}

void GSHighScore::Update(float deltaTime)
{
	m_background->Update(deltaTime);
	m_button->Update(deltaTime);
}

void GSHighScore::Draw()
{
	m_background->Draw();
	m_button->Draw();
}

void GSHighScore::Pause()
{
}

void GSHighScore::Resume()
{
}

void GSHighScore::Exit()
{
}

void GSHighScore::HandleEvent()
{
}

void GSHighScore::OnKey(unsigned char key, bool pressed)
{
}

void GSHighScore::OnMouseClick(int x, int y, unsigned char key, bool pressed)
{
	if (m_button->HandleTouchMouse(x, y, pressed))
	{
		ResourcesManager::GetInstance()->GetSound(1)->Play();
		GameStateMachine::GetInstance()->PopState();
	};
}

void GSHighScore::OnMouseMove(int x, int y)
{
	m_button->HandleMoveMouse(x, y);
}

void GSHighScore::OnMouseScroll(int x, int y, short delta)
{
}
