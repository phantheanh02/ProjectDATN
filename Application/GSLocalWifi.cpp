#include "stdafx.h"
#include "GSLocalWifi.h"

extern CharacterType currentCharacter;
CharacterType currentOpponentCharacter = C_BLACK;

GSLocalWifi::~GSLocalWifi()
{
	SocketManager::GetInstance()->CloseSocket();
}

void GSLocalWifi::Init()
{
	ResetLobby();
	m_preCharacter = currentCharacter;
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

	if (m_buttonList.size() > 0)
	{
		m_buttonList.clear();
	}
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
	HandleRequest();
	
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
		if (button->m_type == BUTTON_START && !m_isClientReady)
		{
			continue;
		}
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
	m_preCharacter = currentCharacter;
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

	if (m_preCharacter != currentCharacter)
	{
		m_preCharacter = currentCharacter;
		SendCharacterType();
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
				SocketManager::GetInstance()->SendNewMessage("action15");

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
					LOG("Create room fail!!\n");
				}
				break;
			case BUTTON_JOIN:
				if (SocketManager::GetInstance()->CreateSocket(false, SERVER_PORT))
				{
					m_hasClient = true;
					isCreatedSocket = true;
					SocketManager::GetInstance()->SendNewMessage("action00");
					SendCharacterType();
					m_clientCharacter->SetTexture(ResourcesManager::GetInstance()->GetTexture(currentCharacter));
				}
				else
				{
					// noti error
					LOG("Join room fail!!\n");
				}
				break;
			case BUTTON_CANCEL:
				SocketManager::GetInstance()->SendNewMessage("actionok");
				break;
			case BUTTON_CHOOSE_CHARACTER:
				GameStateMachine::GetInstance()->PushState(StateType::STATE_CHOOSECHARACTER);
				break;
			case BUTTON_READY:
				if (m_isClientReady)
				{
					m_isClientReady = false;
					button->SetTexture(ResourcesManager::GetInstance()->GetTexture("Button/btn_ready.png"));
					SocketManager::GetInstance()->SendNewMessage("action13");
				}
				else
				{
					m_isClientReady = true;
					button->SetTexture(ResourcesManager::GetInstance()->GetTexture("Button/btn_ready_b.png"));
					SocketManager::GetInstance()->SendNewMessage("action12");
				}
				break;
			case BUTTON_START:
				if (m_isClientReady)
				{
					//SocketManager::GetInstance()->SendNewMessage("action11");
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

void GSLocalWifi::HandleRequest()
{
	std::string msg = SocketManager::GetInstance()->GetDataMsg();
	if (msg != "")
	{
		m_hasClient = true;
		std::string header = msg.substr(0, 6);
		std::string body = msg.substr(6, 8);
		RequestType request = (RequestType)std::stoi(body);
		if (header == "action")
		{
			switch (request)
			{
			case CONNECT:
				if (SocketManager::GetInstance()->IsHost())
				{
					SendCharacterType();
				}
				break;
			case READY_STATE:
				break;
			case RETRY_STATE:
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
				//if (!SocketManager::GetInstance()->IsHost())
				//{
				//	SocketManager::GetInstance()->SendNewMessage("action11");
				//}
				GameStateMachine::GetInstance()->PushState(StateType::STATE_SOLO);
				break;
			case JOIN_BATTLE:
				break;
			case EXIT_ROOM:
				if (SocketManager::GetInstance()->IsHost())
				{
					m_hasClient = false;
					m_isClientReady = false;
					//SocketManager::GetInstance()->CloseClientSocket();
				}
				else
				{
					m_hasClient = false;					
					SocketManager::GetInstance()->CloseSocket();
					GameStateMachine::GetInstance()->PopState();
				}
				break;
			case READY:
				m_isClientReady = true;
				break;
			case NO_READY:
				m_isClientReady = false;
				break;
			case DISCONNECT:
				if (!SocketManager::GetInstance()->IsHost())
				{
					m_isClientReady = false;
					m_hasClient = false;
					SocketManager::GetInstance()->CloseSocket();
					Init();
					//GameStateMachine::GetInstance()->PopState();
					//GameStateMachine::GetInstance()->PushState(StateType::STATE_HIGHSCORE);

				}
				break;
			default:
				break;
			}

		}
		//SocketManager::GetInstance()->SetStatusMsg(false);
	}

}

void GSLocalWifi::ResetLobby()
{
	m_isClientReady = false;
	m_hasClient = false;
}

void GSLocalWifi::SendCharacterType()
{
	switch (currentCharacter)
	{
	case C_BLACK:
		SocketManager::GetInstance()->SendNewMessage("action20");
		break;
	case C_BLUE:
		SocketManager::GetInstance()->SendNewMessage("action21");
		break;
	case C_GREEN:
		SocketManager::GetInstance()->SendNewMessage("action22");
		break;
	case C_RED:
		SocketManager::GetInstance()->SendNewMessage("action23");
		break;
	case C_YELLOW:
		SocketManager::GetInstance()->SendNewMessage("action24");
		break;
	default:
		break;
	}
}
