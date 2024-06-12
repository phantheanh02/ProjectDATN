#include "stdafx.h"
#include "GSLocalWifi.h"

extern CharacterType currentCharacter;
CharacterType currentOpponentCharacter = C_BLACK;

GSLocalWifi::~GSLocalWifi()
{
}

void GSLocalWifi::Init()
{
	m_hasClient = false;
	m_isJoinRoom = false;
	m_isClientReady = false;

	// chacracter
	m_hostCharacter = std::make_shared<SpriteAnimation>(currentCharacter, 5, 0.1);
	m_hostCharacter->AttachCamera(SceneManager::GetInstance()->GetCamera(CameraType::STATIC_CAMERA));
	m_hostCharacter->Set2DPosition(245, 300);
	m_hostCharacter->Set2DSize(300, 300);

	m_clientCharacter = std::make_shared<SpriteAnimation>(CharacterType::C_BLACK, 5, 0.1);
	m_clientCharacter->SetModel(ResourcesManager::GetInstance()->GetModel(ModelType::L_RETANGLE_CENTER));
	m_clientCharacter->AttachCamera(SceneManager::GetInstance()->GetCamera(CameraType::STATIC_CAMERA));
	m_clientCharacter->Set2DPosition(715, 300);
	m_clientCharacter->Set2DSize(300, 300);

	// button
	auto button = std::make_shared<Button>("Button/btn_back.png", BUTTON_BACK);
	button->Set2DSize(220, 70);
	button->Set2DPosition(710, 30);
	m_buttonList.push_back(button);

	button = std::make_shared<Button>("Button/btn_character.png", BUTTON_CHOOSE_CHARACTER);
	button->Set2DSize(220, 70);
	button->Set2DPosition(460, 30);
	m_buttonList.push_back(button);

	button = std::make_shared<Button>("Button/btn_create.png", BUTTON_CREAT);
	button->Set2DSize(220, 70);
	button->Set2DPosition(605, 250);
	m_buttonList.push_back(button);

	button = std::make_shared<Button>("Button/btn_join.png", BUTTON_JOIN);
	button->Set2DSize(220, 70);
	button->Set2DPosition(605, 350);
	m_buttonList.push_back(button);

#if CHEAT
	button = std::make_shared<Button>("Button/btn_cancel.png", BUTTON_CANCEL);
	button->Set2DSize(220, 70);
	button->Set2DPosition(100, 30);
	m_buttonList.push_back(button);
#endif
	// bg
	m_background = std::make_shared<Sprite2D>("Background/bg_state_background.png");
	m_background->Set2DSize(Globals::screenWidth, Globals::screenHeight);
	m_background->AttachCamera(SceneManager::GetInstance()->GetCamera(CameraType::STATIC_CAMERA));

	// Net
	//SocketManager::GetInstance()->Init();

}

void GSLocalWifi::Update(float deltaTime)
{
	m_background->Update(deltaTime);
	if (SocketManager::GetInstance()->HasNewMsg())
	{
		m_hasClient = true;
		std::string msg = SocketManager::GetInstance()->GetDataMsg();
		std::cout << msg << "\n";
		RequestType request = (RequestType)std::stoi(msg);
		switch (request)
		{
		case CONNECT:
			SocketManager::GetInstance()->SendNewMessage("Accept connection!!");
			break;
		case READY_STATE:
			break;
		case CHARACTER_BLACK:
			if (SocketManager::GetInstance()->IsHost())
			{
				m_clientCharacter->SetTexture(ResourcesManager::GetInstance()->GetTexture(29));
			}
			else
			{
				m_hostCharacter->SetTexture(ResourcesManager::GetInstance()->GetTexture(29));
			}
			currentOpponentCharacter = C_BLACK;
			break;
		case CHARACTER_BLUE:
			if (SocketManager::GetInstance()->IsHost())
			{
				m_clientCharacter->SetTexture(ResourcesManager::GetInstance()->GetTexture(35));
			}
			else
			{
				m_hostCharacter->SetTexture(ResourcesManager::GetInstance()->GetTexture(35));
			}
			currentOpponentCharacter = C_BLUE;
			break;
		case CHARACTER_GREEN:
			if (SocketManager::GetInstance()->IsHost())
			{
				m_clientCharacter->SetTexture(ResourcesManager::GetInstance()->GetTexture(41));
			}
			else
			{
				m_hostCharacter->SetTexture(ResourcesManager::GetInstance()->GetTexture(41));
			}
			currentOpponentCharacter = C_GREEN;
			break;
		case CHARACTER_RED:
			if (SocketManager::GetInstance()->IsHost())
			{
				m_clientCharacter->SetTexture(ResourcesManager::GetInstance()->GetTexture(47));
			}
			else
			{
				m_hostCharacter->SetTexture(ResourcesManager::GetInstance()->GetTexture(47));
			}
			currentOpponentCharacter = C_RED;
			break;
		case CHARACTER_YELLOW:
			if (SocketManager::GetInstance()->IsHost())
			{
				m_clientCharacter->SetTexture(ResourcesManager::GetInstance()->GetTexture(53));
			}
			else
			{
				m_hostCharacter->SetTexture(ResourcesManager::GetInstance()->GetTexture(53));
			}
			currentOpponentCharacter = C_YELLOW;
			break;
		case START_PLAY:
			GameStateMachine::GetInstance()->PushState(StateType::STATE_SOLO);
			break;
		case REJOIN:
			break;
		case EXIT_ROOM:
			if (SocketManager::GetInstance()->IsHost())
			{
				m_hasClient = false;
			}
			else
			{
				GameStateMachine::GetInstance()->PopState();
			}
			break;
		case READY:
			m_isClientReady = true;
			break;
		case NO_READY:
			m_isClientReady = false;
			break;
		default:
			break;
		}
		SocketManager::GetInstance()->SetStatusMsg(false);
	}

	m_hostCharacter->Update(deltaTime);	
	if (m_hasClient)
	{
		m_clientCharacter->Update(deltaTime);
	}
}

void GSLocalWifi::Draw()
{
	m_background->Draw();

	for (auto button : m_buttonList)
	{
		button->Draw();
	}

	m_hostCharacter->Draw();
	if (m_hasClient)
	{
		m_clientCharacter->Draw();
	}
}

void GSLocalWifi::Pause()
{
}

void GSLocalWifi::Resume()
{
	// chacracter
	if (SocketManager::GetInstance()->IsHost())
	{
		m_hostCharacter->SetTexture(ResourcesManager::GetInstance()->GetTexture(currentCharacter));
	}
	else
	{
		m_clientCharacter->SetTexture(ResourcesManager::GetInstance()->GetTexture(currentCharacter));
	}
	switch (currentCharacter)
	{
	case C_BLACK:
		SocketManager::GetInstance()->SendNewMessage("20");
		break;
	case C_BLUE:
		SocketManager::GetInstance()->SendNewMessage("21");
		break;
	case C_GREEN:
		SocketManager::GetInstance()->SendNewMessage("22");
		break;
	case C_RED:
		SocketManager::GetInstance()->SendNewMessage("23");
		break;
	case C_YELLOW:
		SocketManager::GetInstance()->SendNewMessage("24");
		break;
	default:
		break;
	}
}

void GSLocalWifi::Exit()
{
}

void GSLocalWifi::HandleEvent()
{
}

void GSLocalWifi::OnKey(unsigned char key, bool pressed)
{
}

void GSLocalWifi::OnMouseClick(int x, int y, unsigned char key, bool pressed)
{
	bool isCreatedSocket = false;
	for (auto button : m_buttonList)
	{
		if (button->HandleTouchMouse(x, y, pressed))
		{
			switch (button->m_type)
			{
			case BUTTON_BACK:
				SocketManager::GetInstance()->SendNewMessage("50");
				SocketManager::GetInstance()->CloseSocket();

				ResourcesManager::GetInstance()->GetSound(1)->Play();
				GameStateMachine::GetInstance()->PopState();
				break;
			case BUTTON_CREAT:
				if (SocketManager::GetInstance()->CreateSocket(true, SERVER_PORT))
				{
					m_hasClient = false;
					isCreatedSocket = true;
				}
				else
				{
					// noti error
					printf("Create room fail!!\n");
				}
				break;
			case BUTTON_JOIN:
				if (SocketManager::GetInstance()->CreateSocket(false, SERVER_PORT))
				{
					m_hasClient = true;
					isCreatedSocket = true;
				}
				else
				{
					// noti error
					printf("Join room fail!!\n");
				}
				break;
			case BUTTON_CANCEL:
				SocketManager::GetInstance()->SendNewMessage("ok");
				break;
			case BUTTON_CHOOSE_CHARACTER:
				GameStateMachine::GetInstance()->PushState(StateType::STATE_CHOOSECHARACTER);
				break;
			case BUTTON_READY:
				if (m_isClientReady)
				{
					m_isClientReady = false;
					SocketManager::GetInstance()->SendNewMessage("13");
				}
				else
				{
					m_isClientReady = true;
					SocketManager::GetInstance()->SendNewMessage("12");
				}
				break;
			case BUTTON_START:
				if (m_isClientReady)
				{
					SocketManager::GetInstance()->SendNewMessage("11");
					GameStateMachine::GetInstance()->PushState(StateType::STATE_SOLO);
				}
				else
				{
					// noti client not ready
					printf("Client hasn't ready\n");

				}
				break;
			default:
				break;
			}
		}
	}
	if (isCreatedSocket)
	{
		PopButton(BUTTON_CREAT);
		PopButton(BUTTON_JOIN);
		if (SocketManager::GetInstance()->IsHost())
		{
			auto button = std::make_shared<Button>("Button/btn_start.png", BUTTON_START);
			button->Set2DSize(220, 70);
			button->Set2DPosition(710, 620);
			m_buttonList.push_back(button);
		}
		else
		{
			auto button = std::make_shared<Button>("Button/btn_ready.png", BUTTON_READY);
			button->Set2DSize(220, 70);
			button->Set2DPosition(710, 620);
			m_buttonList.push_back(button);
		}

	}
}

void GSLocalWifi::OnMouseMove(int x, int y)
{
}

void GSLocalWifi::OnMouseScroll(int x, int y, short delta)
{
}

void GSLocalWifi::PopButton(ButtonType type)
{
	for (int button = 0; button < m_buttonList.size(); button++)
	{
		if (m_buttonList[button]->m_type == type)
		{
			m_buttonList.erase(m_buttonList.begin() + button);
			break;
		}
	}
}
