#include "stdafx.h"
#include "GSMenu.h"
#include "Globals.h"
#include "ResourcesManager.h"
#include "SceneManager.h"
#include "GameStateMachine.h"

GSMenu::~GSMenu()
{
}

void GSMenu::Init()
{
	m_key = 0;
	m_mouse = 0;

	// button play
	std::shared_ptr<Button> button = std::make_shared<Button>("btn_play.png", BUTTON_PLAY);
	button->Set2DSize(220, 70);
	button->Set2DPosition(Globals::screenWidth / 2.0 - 78, Globals::screenHeight / 2.0f - 194);
	m_listButton.push_back(button);

	// button quit
	button = std::make_shared<Button>("btn_quit.png", BUTTON_EXIT);
	button->Set2DSize(220, 70);
	button->Set2DPosition(Globals::screenWidth / 2.0 - 78, Globals::screenHeight / 2.0f + 138);
	m_listButton.push_back(button);

	// button setting
	button = std::make_shared<Button>("btn_setting.png", BUTTON_SETTING);
	button->Set2DSize(220, 70);
	button->Set2DPosition(Globals::screenWidth / 2.0 - 78, Globals::screenHeight / 2.0f - 111);
	m_listButton.push_back(button);

	// button highscore
	button = std::make_shared<Button>("btn_highscore.png", BUTTON_HIGHSCORE);
	button->Set2DSize(220, 70);
	button->Set2DPosition(Globals::screenWidth / 2.0 - 78, Globals::screenHeight / 2.0f - 28);
	m_listButton.push_back(button);

	// button credit
	button = std::make_shared<Button>("btn_credit.png", BUTTON_CREDIT);
	button->Set2DSize(220, 70);
	button->Set2DPosition(Globals::screenWidth / 2.0 - 78, Globals::screenHeight / 2.0f + 55);
	m_listButton.push_back(button);

	// background
	m_background = std::make_shared<Sprite2D>("bg_menu.png");
	m_background->AttachCamera(SceneManager::GetInstance()->GetCamera(CameraType::STATIC_CAMERA));
	m_background->Set2DSize(Globals::screenWidth, Globals::screenHeight);
	m_background->Set2DPosition(0, 0);
}

void GSMenu::Update(float deltaTime)
{
	for (auto& button : m_listButton)
	{
		button->Update(deltaTime);
	}
}

void GSMenu::Draw()
{
	m_background->Draw();
	for (auto& button : m_listButton)
	{
		button->Draw();
	}
}

void GSMenu::Pause()
{
}

void GSMenu::Resume()
{
}

void GSMenu::Exit()
{

}

void GSMenu::HandleEvent()
{
}

void GSMenu::OnKey(unsigned char key, bool pressed)
{
	if (pressed)
	{
		switch (key)
		{
		case KEY_SPACE:
			m_key |= 1 << 0;
			break;
		default:
			break;
		}
	}
	else
	{
		switch (key)
		{
		case KEY_SPACE:
			m_key ^= 1 << 0;
			GameStateMachine::GetInstance()->PopState();
			break;
		default:
			break;
		}
	}
}

void GSMenu::OnMouseClick(int x, int y, unsigned char key, bool pressed)
{
	for (auto& button : m_listButton)
	{
		if (button->HandleTouchMouse(x, y, pressed))
		{
			ResourcesManager::GetInstance()->GetSound(1)->Play();
			switch (button->m_type)
			{
			case BUTTON_PLAY:
				GameStateMachine::GetInstance()->PushState(StateType::STATE_MAP);
				break;
			case BUTTON_SETTING:
				GameStateMachine::GetInstance()->PushState(StateType::STATE_SETTING);
				break;
			case BUTTON_HIGHSCORE:
				GameStateMachine::GetInstance()->PushState(StateType::STATE_HIGHSCORE);
				break;
			case BUTTON_CREDIT:
				GameStateMachine::GetInstance()->PushState(StateType::STATE_CREDIT);
				break;
			case BUTTON_EXIT:
				GameStateMachine::GetInstance()->Exit();
				break;
			default:
				break;
			}
		};
	}
}

void GSMenu::OnMouseMove(int x, int y)
{
	for (auto button : m_listButton)
	{
		button->HandleMoveMouse(x, y);
	}
}

void GSMenu::OnMouseScroll(int x, int y, short delta)
{

}
