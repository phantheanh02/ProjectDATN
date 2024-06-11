#include "stdafx.h"
#include "GSHighScore.h"
#include "ResourcesManager.h"
#include "SceneManager.h"
#include "GameStateMachine.h"
#include "SocketManager.h"

GSHighScore::~GSHighScore()
{
}

void GSHighScore::Init()
{
	// button
	m_button = std::make_shared<Button>("btn_back.png", BUTTON_BACK);
	m_button->Set2DSize(110 * 960 / 608.0f, 35 * 960 / 608.0f);
	m_button->Set2DPosition(479 * 960 / 608.0f, 18 * 960 / 608.0f);

	auto button = std::make_shared<Button>("btn_play.png", BUTTON_PLAY);
	button->Set2DSize(220, 70);
	button->Set2DPosition(200, 500);
	m_buttonList.push_back(button);

	button = std::make_shared<Button>("btn_save.png", BUTTON_SAVE);
	button->Set2DSize(220, 70);
	button->Set2DPosition(500, 500);
	m_buttonList.push_back(button);

	button = std::make_shared<Button>("btn_cancel.png", BUTTON_CANCEL);
	button->Set2DSize(220, 70);
	button->Set2DPosition(300, 100);
	m_buttonList.push_back(button);

	// bg
	m_background = std::make_shared<Sprite2D>("state_background.png");
	m_background->Set2DSize(Globals::screenWidth, Globals::screenHeight);
	m_background->AttachCamera(SceneManager::GetInstance()->GetCamera(CameraType::STATIC_CAMERA));

	// Net
	SocketManager::GetInstance()->Init();

}

void GSHighScore::Update(float deltaTime)
{
	m_background->Update(deltaTime);
	if (SocketManager::GetInstance()->HasNewMsg())
	{
		std::cout << SocketManager::GetInstance()->GetDataMsg() << "\n";
		SocketManager::GetInstance()->SetStatusMsg(false);
	}
}

void GSHighScore::Draw()
{
	m_background->Draw();
	//m_button->Draw();
	for (auto button : m_buttonList)
	{
		button->Draw();
	}
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
	//if (m_button->HandleTouchMouse(x, y, pressed))
	//{
	//	ResourcesManager::GetInstance()->GetSound(1)->Play();
	//	GameStateMachine::GetInstance()->PopState();
	//};

	for (auto button : m_buttonList)
	{
		if (button->HandleTouchMouse(x, y, pressed))
		{
			switch (button->m_type)
			{
			case BUTTON_PLAY:
				SocketManager::GetInstance()->CreateSocket(true, SERVER_PORT);
				break;
			case BUTTON_SAVE:
				SocketManager::GetInstance()->CreateSocket(false, SERVER_PORT);
				break;
			case BUTTON_CANCEL:
				SocketManager::GetInstance()->SendMessage("Hello guys\n");
				break;
			default:
				break;
			}
		}
	}
}

void GSHighScore::OnMouseMove(int x, int y)
{
	m_button->HandleMoveMouse(x, y);
}

void GSHighScore::OnMouseScroll(int x, int y, short delta)
{
}
