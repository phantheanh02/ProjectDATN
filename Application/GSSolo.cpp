#include "stdafx.h"
#include "GSSolo.h"
#include "ResourcesManager.h"
#include "SceneManager.h"
#include "GameStateMachine.h"
#include "Globals.h"
#include "SocketManager.h"
#include <sstream>
#include <cstring>

extern std::shared_ptr<b2World> world;
extern CharacterType currentOpponentCharacter;
extern int tileSizeByPixel;

GSSolo::~GSSolo()
{
	for (auto& it : m_BodyList)
	{
		world->DestroyBody(it);
	}

	world.reset();
	delete m_contactListener;
}

void GSSolo::Init()
{
	m_key = 0;
	m_opponentKey = 0;
	m_isReadyState = false;
	m_isShowPopup = false;
	m_isEndMatch = false;
	m_isWin = false;

	if (world)
	{
		world.reset();
	}

	// get refresh rate and set time step
	DEVMODE devMode;
	EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &devMode);
	m_timeStep = 1.0f / devMode.dmDisplayFrequency;

	// create world with gravity
	world = std::make_shared<b2World>(b2Vec2(0, 30));
	m_contactListener = new ContactListener();
	world->SetContactListener(m_contactListener);

	// background
	m_background = std::make_shared<Sprite2D>("Map/map_solo.png");
	m_background->Set2DSize(Globals::screenWidth , Globals::screenHeight);
	m_background->Set2DPosition(0, 0);
	
	// map
	m_map = SceneManager::GetInstance()->GetMap(PlanetType::MAP_SOLO);
	LoadMap();
	tileSizeByPixel = m_map->GetMinTileSize();

	// Init player
	m_ownerPlayer = std::make_shared<Player>();
	m_opponentPlayer = std::make_shared<Player>(currentOpponentCharacter);
	m_opponentPlayer->SetAction(PlayerAction::JUMPING);

	// set new position
	if (SocketManager::GetInstance()->IsHost())
	{
		m_ownerPlayer->GetBody()->SetTransform(b2Vec2(10, 8), 0);
		m_opponentPlayer->GetBody()->SetTransform(b2Vec2(44, 8), 0);
		m_opponentPlayer->SetDirection(DirectionType::RIGHT);
	}
	else
	{
		m_opponentPlayer->GetBody()->SetTransform(b2Vec2(10, 8), 0);
		m_ownerPlayer->GetBody()->SetTransform(b2Vec2(44, 8), 0);
		m_ownerPlayer->SetDirection(DirectionType::RIGHT);
	}

	// Create bullet pooling
	for (int i = 0; i < 100; i++)
	{
		m_bulletList.push_back(std::make_shared<Bullet>(world.get()));
	}

	// button
	auto button = std::make_shared<Button>("Button/btn_back.png", BUTTON_BACK);
	button->Set2DSize(220, 70);
	button->Set2DPosition(710, 30);
	m_buttonList.push_back(button);

}

void GSSolo::Update(float deltaTime)
{
	if (m_isShowPopup)
	{
		UpdatePopup(deltaTime);
		return;
	}

	if (!m_isReadyState)
	{
		SocketManager::GetInstance()->SendNewMessage("action11");
	}
	HandleRequest();
	HandleEvent();

	world->Step(m_timeStep, VELOCITY_ITERATION, POSITION_ITERATION);

	for (auto it : m_bulletList)
	{
		it->Update(deltaTime);
	}

	m_ownerPlayer->Update(deltaTime);
	m_opponentPlayer->Update(deltaTime);

	CheckWin();
}

void GSSolo::Draw()
{
	m_background->Draw();
	for (auto button : m_buttonList)
	{
		button->Draw();
	}

	for (auto it : m_bulletList)
	{
		it->Draw();
	}

	m_ownerPlayer->Draw();
	m_opponentPlayer->Draw();

	if (m_isShowPopup)
	{
		DrawPopup();
	}
}

void GSSolo::Pause()
{
}

void GSSolo::Resume()
{
}

void GSSolo::Exit()
{

}

void GSSolo::HandleEvent()
{
	int processKeyAAndDPressed = 0;
	if (m_key & (1 << 1) && m_keyStack.back() == "A")
	{
		// move left
		processKeyAAndDPressed = 1;
	}
	if (m_key & (1 << 3) && m_keyStack.back() == "D")
	{
		// move right
		processKeyAAndDPressed = 3;
	}

	int event = m_key;
	if (event & (1 << 1) && event & (1 << 3))
	{
		if (processKeyAAndDPressed == 1)
		{
			event ^= 1 << 3; // pop key "D"
		}
		else if (processKeyAAndDPressed == 3)
		{
			event ^= 1 << 1; // pop key "A"
		}
	}

	m_ownerPlayer->HandleEvent(event);

	// Send information to opponent
	if (m_isReadyState)
	{

		std::string msg = std::to_string(event);
		while (msg.size() < 2)
		{
			msg = "0" + msg;
		}
		msg = "events" + msg;
		if (!SocketManager::GetInstance()->SendNewMessage(msg.c_str()))
		{
			LOG("Opponent disconnect!!");
		}
	}
}

void GSSolo::OnKey(unsigned char key, bool pressed)
{
	if (pressed)
	{
		switch (key)
		{
		case KEY_W:
			m_key |= 1 << 0;
			break;
		case KEY_A:
			m_key |= 1 << 1;
			if (std::find(m_keyStack.begin(), m_keyStack.end(), "A") == m_keyStack.end())
				m_keyStack.push_back("A");
			break;
		case KEY_S:
			m_key |= 1 << 2;
			break;
		case KEY_D:
			m_key |= 1 << 3;
			if (std::find(m_keyStack.begin(), m_keyStack.end(), "D") == m_keyStack.end())
				m_keyStack.push_back("D");
			break;
		case KEY_SPACE:
			m_key |= 1 << 4;
			break;
		case KEY_J:
			m_key |= 1 << 5;
			break;
		default:
			break;
		}
	}
	else
	{
		switch (key)
		{
		case KEY_W:
			m_key ^= 1 << 0;
			//m_ownerPlayer->SetCurrentDirectionByPreDirection();
			break;
		case KEY_A:
			m_key ^= 1 << 1;
			if (std::find(m_keyStack.begin(), m_keyStack.end(), "A") != m_keyStack.end())
			{
				m_keyStack.erase(std::find(m_keyStack.begin(), m_keyStack.end(), "A"));
			}
			break;
		case KEY_S:
			m_key ^= 1 << 2;
			break;
		case KEY_D:
			m_key ^= 1 << 3;
			if (std::find(m_keyStack.begin(), m_keyStack.end(), "D") != m_keyStack.end())
			{
				m_keyStack.erase(std::find(m_keyStack.begin(), m_keyStack.end(), "D"));
			}
			break;
		case KEY_SPACE:
			m_key ^= 1 << 4;
			break;
		case KEY_J:
			m_key ^= 1 << 5;
			break;
		default:
			break;
		}
	}
}

void GSSolo::OnMouseClick(int x, int y, unsigned char key, bool pressed)
{
	for (auto& button : m_buttonList)
	{
		if (button->HandleTouchMouse(x, y, pressed))
		{
			ResourcesManager::GetInstance()->GetSound(1)->Play();
			switch (button->m_type)
			{
			case BUTTON_BACK:
				SocketManager::GetInstance()->SendNewMessage("action16"); // exit battle
				SocketManager::GetInstance()->ClearData();
				GameStateMachine::GetInstance()->PopState();
				break;
			case BUTTON_MENU:
				SocketManager::GetInstance()->ClearData();
				GameStateMachine::GetInstance()->PopState();
				GameStateMachine::GetInstance()->PopState();
				break;
			default:
				break;
			}
		};
	}
}

void GSSolo::OnMouseMove(int x, int y)
{
}

void GSSolo::OnMouseScroll(int x, int y, short delta)
{
}

void GSSolo::LoadMap()
{
	// Load obs	
	auto listPlane = m_map->GetData()->tiles;

	for (const auto& tile : listPlane)
	{
		b2BodyDef bodyDef;
		b2FixtureDef obsFixDef;
		bodyDef.type = b2_staticBody;
		bodyDef.position.Set(tile.x + 0.5, tile.y + 0.5);
		b2Body* body = world->CreateBody(&bodyDef);

		b2PolygonShape shape;
		shape.SetAsBox(0.5, 0.5);
		obsFixDef.filter.categoryBits = FixtureTypes::FIXTURE_GROUND;
		obsFixDef.filter.maskBits = 0xFFFF;
		obsFixDef.shape = &shape;
		body->CreateFixture(&obsFixDef);
		m_BodyList.push_back(body);
	}
}

void GSSolo::HandleRequest()
{
	std::string msg = SocketManager::GetInstance()->GetDataMsg();
	if (msg != "")
	{
		 
		std::string header = msg.substr(0, 6);
		std::string body = msg.substr(6, 8);
		int check = std::stoi(body);

		if (header == "action")
		{
			switch (check)
			{
			case START_PLAY:
				if (!m_isReadyState)
				{
					m_isReadyState = true;
					SocketManager::GetInstance()->SendNewMessage("action11");
				}
				break;
			case EXIT_BATTLE:
				GameStateMachine::GetInstance()->PopState();
				return;
			default:
				break;
			}
		}
		else
		{
			m_opponentKey = check;
		}

		SocketManager::GetInstance()->SetStatusMsg(false);
	}

	// Handle Key
	m_opponentPlayer->HandleEvent(m_opponentKey);
}

void GSSolo::CreateButton(const char* filename, GLfloat width, GLfloat height, GLfloat posX, GLfloat posY, ButtonType buttonType)
{
	auto button = std::make_shared<Button>(filename, buttonType);
	button->Set2DSize(width, height);
	button->Set2DPosition(posX, posY);
	m_buttonList.push_back(button);
}

void GSSolo::CreatePopUp()
{
	m_isShowPopup = true;
	m_listPopupSprite.clear();
	m_buttonList.clear();

	// add new button
	Vector2 size = Vector2(137, 64);
	GLfloat posX = 430;
	GLfloat posY = 233;

	// Background
	auto sprite = std::make_shared<Sprite2D>("Background/bg_black.png");
	sprite->Set2DSize(Globals::screenWidth, Globals::screenHeight);
	sprite->Set2DPosition(0, 0);
	sprite->SetTransparency(0.5f);
	sprite->AttachCamera(SceneManager::GetInstance()->GetCamera(CameraType::STATIC_CAMERA));
	m_listPopupSprite.push_back(sprite);

	if (m_isEndMatch)
	{
		if (m_isWin)
		{

			sprite = std::make_shared<Sprite2D>("Background/bg_victory3star.png");
		}
		else
		{
			sprite = std::make_shared<Sprite2D>("Background/bg_lost.png");
		}

		sprite->Set2DSize(640, 480);
		sprite->Set2DPosition(480, 360);
		sprite->SetModel(ResourcesManager::GetInstance()->GetModel(ModelType::R_RETANGLE_CENTER));
		sprite->AttachCamera(SceneManager::GetInstance()->GetCamera(CameraType::STATIC_CAMERA));
		m_listPopupSprite.push_back(sprite);

		CreateButton("Button/btn_menu.png", 220, 70, 370, 462, BUTTON_MENU);
	}
}

void GSSolo::DrawPopup()
{
	for (auto sprite : m_listPopupSprite)
	{
		sprite->Draw();
	}
	for (auto& button : m_buttonList)
	{
		button->Draw();
	}
}

void GSSolo::UpdatePopup(float deltaTime)
{

}

void GSSolo::CheckWin()
{
	if (m_ownerPlayer->IsDie())
	{
		m_isEndMatch = true;
		m_isWin = false;
		CreatePopUp();
	}
	else if (m_opponentPlayer->IsDie())
	{
		m_isEndMatch = true;
		m_isWin = true;
		CreatePopUp();
	}
}

