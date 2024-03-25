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

	auto model = ResourcesManager::GetInstance()->GetModel(0);
	auto shader = ResourcesManager::GetInstance()->GetShader(0);
	auto texture = ResourcesManager::GetInstance()->GetTexture("btn_play.png");
	auto staticCamera = SceneManager::GetInstance()->GetCamera(1);

	// button play
	std::shared_ptr<Button> buttonPlay = std::make_shared<Button>(model, shader, texture, BUTTON_PLAY);
	buttonPlay->Set2DSize(220, 70);
	buttonPlay->Set2DPosition(Globals::screenWidth / 2.0 - 78, Globals::screenHeight / 2.0f - 194);
	buttonPlay->AttachCamera(staticCamera);

	// button exit
	texture = ResourcesManager::GetInstance()->GetTexture(4);
	std::shared_ptr<Button> buttonExit = std::make_shared<Button>(model, shader, texture, BUTTON_EXIT);
	buttonExit->Set2DSize(220, 70);
	buttonExit->Set2DPosition(Globals::screenWidth / 2.0 - 78, Globals::screenHeight / 2.0f + 138);
	buttonExit->AttachCamera(staticCamera);

	// button setting
	texture = ResourcesManager::GetInstance()->GetTexture(61);
	auto buttonSetting = std::make_shared<Button>(model, shader, texture, BUTTON_SETTING);
	buttonSetting->Set2DSize(220, 70);
	buttonSetting->Set2DPosition(Globals::screenWidth / 2.0 - 78, Globals::screenHeight / 2.0f - 111);
	buttonSetting->AttachCamera(staticCamera);

	// button highscore
	texture = ResourcesManager::GetInstance()->GetTexture(63);
	auto buttonHighscore = std::make_shared<Button>(model, shader, texture, BUTTON_HIGHSCORE);
	buttonHighscore->Set2DSize(220, 70);
	buttonHighscore->Set2DPosition(Globals::screenWidth / 2.0 - 78, Globals::screenHeight / 2.0f - 28);
	buttonHighscore->AttachCamera(staticCamera);

	// button credit
	texture = ResourcesManager::GetInstance()->GetTexture(62);
	auto buttonCredit = std::make_shared<Button>(model, shader, texture, BUTTON_CREDIT);
	buttonCredit->Set2DSize(220, 70);
	buttonCredit->Set2DPosition(Globals::screenWidth / 2.0 - 78, Globals::screenHeight / 2.0f + 55);
	buttonCredit->AttachCamera(staticCamera);

	m_listButton.push_back(buttonPlay);
	m_listButton.push_back(buttonExit);
	m_listButton.push_back(buttonSetting);
	m_listButton.push_back(buttonHighscore);
	m_listButton.push_back(buttonCredit);

	// background
	texture = ResourcesManager::GetInstance()->GetTexture("bg_menu.png");
	m_background = std::make_shared<Sprite2D>(0, model, shader, texture);
	m_background->AttachCamera(staticCamera);
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
