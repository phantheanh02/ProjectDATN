#include "stdafx.h"
#include "GSCredit.h"
#include "ResourcesManager.h"
#include "SceneManager.h"
#include "GameStateMachine.h"
GSCredit::~GSCredit()
{
}

void GSCredit::Init()
{
	// button
	m_button = std::make_shared<Button>("Button/btn_back.png", BUTTON_BACK);
	m_button->Set2DSize(110 * 960 / 608.0f, 35 * 960 / 608.0f);
	m_button->Set2DPosition(479 * 960 / 608.0f, 18 * 960 / 608.0f);

	// bg
	m_background = std::make_shared<Sprite2D>("Background/bg_help.png");
	m_background->Set2DPosition(0, 0);
	m_background->Set2DSize(Globals::screenWidth, Globals::screenHeight);
	m_background->AttachCamera(SceneManager::GetInstance()->GetCamera(CameraType::STATIC_CAMERA));
}

void GSCredit::Update(float deltaTime)
{
}

void GSCredit::Draw()
{
	m_background->Draw();
	m_button->Draw();
}

void GSCredit::Pause()
{
}

void GSCredit::Resume()
{
}

void GSCredit::Exit()
{
}

void GSCredit::HandleEvent()
{
}

void GSCredit::OnKey(unsigned char key, bool pressed)
{
}

void GSCredit::OnMouseClick(int x, int y, unsigned char key, bool pressed)
{
	if (m_button->HandleTouchMouse(x, y, pressed))
	{
		ResourcesManager::GetInstance()->GetSound(1)->Play();
		GameStateMachine::GetInstance()->PopState();
	};
}

void GSCredit::OnMouseMove(int x, int y)
{

	m_button->HandleMoveMouse(x, y);
	
}

void GSCredit::OnMouseScroll(int x, int y, short delta)
{
}
