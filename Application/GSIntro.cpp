#include "stdafx.h"
#include "GSIntro.h"
#include "ResourcesManager.h"
#include "SceneManager.h"
#include "GameStateMachine.h"
#include "Globals.h"

GSIntro::~GSIntro()
{
}

void GSIntro::Init()
{
	m_time = 2.0f;

	m_intro = std::make_shared<Sprite2D>("intro.png");
	m_intro->Set2DSize(864, 42);
	m_intro->Set2DPosition(Globals::screenWidth / 2 - 864 / 2.0f, Globals::screenHeight / 2 - 21);

	m_bgIntro = std::make_shared<Sprite2D>("bg_intro.png");
	m_bgIntro->Set2DSize(Globals::screenWidth , Globals::screenHeight);
	m_bgIntro->Set2DPosition(0, 0);
}

void GSIntro::Update(float deltaTime)
{
	m_time -= deltaTime;
	if (m_time< 0) // space pressed
	{
		GameStateMachine::GetInstance()->PushState(StateType::STATE_MENU);
	}
	m_intro->Update(deltaTime);


}

void GSIntro::Draw()
{
	m_bgIntro->Draw();
	m_intro->Draw();
}

void GSIntro::Pause()
{
}

void GSIntro::Resume()
{
}

void GSIntro::Exit()
{
	m_intro.reset();
}

void GSIntro::HandleEvent()
{
}

void GSIntro::OnKey(unsigned char key, bool pressed)
{
	
}

void GSIntro::OnMouseClick(int x, int y, unsigned char key, bool pressed)
{
}

void GSIntro::OnMouseMove(int x, int y)
{
}

void GSIntro::OnMouseScroll(int x, int y, short delta)
{
}
