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
	m_opponentPlayer = std::make_shared<Player>();
	m_opponentPlayer->SetCharacter(currentOpponentCharacter);
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
	auto currentVelocity = m_ownerPlayer->GetBody()->GetLinearVelocity();
	float desiredVel = 0, velChange, impulse;

	int processKeyAAndDPressed = 0;
	if (m_key & (1 << 1) && m_keyStack.back() == "A")
	{
		// move left
		desiredVel = b2Max(currentVelocity.x - 0.25f, -MOVEMENT_SPEED);
		m_ownerPlayer->SetDirection(DirectionType::LEFT);
		processKeyAAndDPressed = 1;
	}
	if (m_key & (1 << 3) && m_keyStack.back() == "D")
	{
		// move right
		desiredVel = b2Min(currentVelocity.x + 0.25f, MOVEMENT_SPEED);
		m_ownerPlayer->SetDirection(DirectionType::RIGHT);
		processKeyAAndDPressed = 3;
	}
	if (desiredVel != 0 && !m_ownerPlayer->IsJumping())
	{
		m_ownerPlayer->SetAction(PlayerAction::RUNNING);
	}
	// apply force to move
	velChange = desiredVel - currentVelocity.x;
	impulse = m_ownerPlayer->GetBody()->GetMass() * velChange; //disregard time factor
	m_ownerPlayer->GetBody()->ApplyLinearImpulse(b2Vec2(impulse, 0), m_ownerPlayer->GetBody()->GetWorldCenter(), true);

	if (m_key & (1 << 0) && velChange == 0 && !m_ownerPlayer->IsJumping())
	{
		// key w
		m_ownerPlayer->SetAction(PlayerAction::FIRE_TOP);
		m_ownerPlayer->SetDirection(DirectionType::TOP);
	}
	else if (m_ownerPlayer->GetDirection() != m_ownerPlayer->GetSprinningDirection())
	{
		m_ownerPlayer->SetCurrentDirectionByPreDirection();
	}


	if (m_key & (1 << 2))
	{
		// key s
		m_ownerPlayer->SetAction(PlayerAction::CROUCH);
	}

	// JUMP: key space
	if (m_key & (1 << 4) && !m_ownerPlayer->IsJumping() && m_ownerPlayer->GetCurrentAction() != PlayerAction::JUMPING)
	{
		m_ownerPlayer->SetAction(PlayerAction::JUMPING);
		desiredVel = b2Min(currentVelocity.x, currentVelocity.y - 3.0f);
		float jumpHeight = 5.0f;
		auto impulse = m_ownerPlayer->GetBody()->GetMass() * sqrt(2 * world->GetGravity().y * jumpHeight);
		m_ownerPlayer->GetBody()->ApplyLinearImpulseToCenter(b2Vec2(0.0f, -impulse), true);
	}

	if (!(m_key & 0xF))
	{
		// none of movement key is pressed
		m_ownerPlayer->SetAction(PlayerAction::IDLE);
	}
	if (m_ownerPlayer->IsJumping())
	{
		m_ownerPlayer->SetAction(PlayerAction::JUMPING);
	}

	// Fire
	if (m_key & (1 << 5) && m_ownerPlayer->IsReadyAttack())
	{
		m_ownerPlayer->ResetCooldown();
		b2Vec2 pos = m_ownerPlayer->GetBody()->GetPosition();
		b2Vec2 speed;

		switch (m_ownerPlayer->GetDirection())
		{
		case DirectionType::TOP:
			speed = b2Vec2(0.0f, -20.0f);
			break;
		case DirectionType::LEFT:
			speed = b2Vec2(-20.0f, 0.0f);
			break;
		case DirectionType::RIGHT:
			speed = b2Vec2(20.0f, 0.0f);
			break;
		}
		CreateBullet((BulletType)m_ownerPlayer->GetPlayerBulletType(), speed, Vector2(m_ownerPlayer->GetBody()->GetPosition().x, m_ownerPlayer->GetBody()->GetPosition().y));
	}

	// Send information to opponent
	if (m_isReadyState)
	{
		int keySend = m_key;
		if (keySend & (1 << 1) && keySend & (1 << 3))
		{
			if (processKeyAAndDPressed == 1)
			{
				keySend ^= 1 << 3; // pop key "D"
			}
			else if (processKeyAAndDPressed == 3)
			{
				keySend ^= 1 << 1; // pop key "A"
			}
		}

		std::string msg = std::to_string(keySend);
		while (msg.size() < 2)
		{
			printf("%s\n", msg);
			msg = "0" + msg;
		}
		msg = "events" + msg;
		if (!SocketManager::GetInstance()->SendNewMessage(msg.c_str()))
		{
			printf("Opponent disconnect!!\n");
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
			m_ownerPlayer->SetCurrentDirectionByPreDirection();
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
	b2Body* obsBody;
	b2BodyDef obsBodyDef;
	b2PolygonShape obsShape;
	b2FixtureDef obsFixDef;
	auto listPlane = m_map->GetPlaneList();
	for (auto it : listPlane)
	{
		obsBody = nullptr;
		obsBodyDef.position.Set(it.x, it.y);
		obsShape.SetAsBox(it.z, it.w);
		obsBody = world->CreateBody(&obsBodyDef);
		obsFixDef.shape = &obsShape;
		obsFixDef.filter.categoryBits = FixtureTypes::FIXTURE_GROUND;
		obsFixDef.filter.maskBits = 0xFFFF;
		obsBody->CreateFixture(&obsFixDef);
		m_BodyList.push_back(obsBody);
	}
}

void GSSolo::HandleRequest()
{
	if (SocketManager::GetInstance()->HasNewMsg())
	{
		std::string msg = SocketManager::GetInstance()->GetDataMsg();
		std::string header = msg.substr(0, 6);
		std::string body = msg.substr(6, 8);
		int check = std::stoi(body);

		if (header == "action")
		{
			switch (check)
			{
			case START_PLAY:
				m_isReadyState = true;
				SocketManager::GetInstance()->SendNewMessage("action11");
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
	auto currentVelocity = m_opponentPlayer->GetBody()->GetLinearVelocity();
	float desiredVel = 0, velChange, impulse;

	if (m_opponentKey & (1 << 1) )
	{
		// move left
		desiredVel = b2Max(currentVelocity.x - 0.25f, -MOVEMENT_SPEED);
		m_opponentPlayer->SetDirection(DirectionType::LEFT);
	}
	if (m_opponentKey & (1 << 3))
	{
		// move right
		desiredVel = b2Min(currentVelocity.x + 0.25f, MOVEMENT_SPEED);
		m_opponentPlayer->SetDirection(DirectionType::RIGHT);
	}
	if (desiredVel != 0 && !m_opponentPlayer->IsJumping())
	{
		m_opponentPlayer->SetAction(PlayerAction::RUNNING);
	}
	// apply force to move
	velChange = desiredVel - currentVelocity.x;
	impulse = m_opponentPlayer->GetBody()->GetMass() * velChange; //disregard time factor
	m_opponentPlayer->GetBody()->ApplyLinearImpulse(b2Vec2(impulse, 0), m_opponentPlayer->GetBody()->GetWorldCenter(), true);

	if (m_opponentKey & (1 << 0) && velChange == 0 && !m_opponentPlayer->IsJumping())
	{
		// key w
		m_opponentPlayer->SetAction(PlayerAction::FIRE_TOP);
		m_opponentPlayer->SetDirection(DirectionType::TOP);
	}
	else if (m_opponentPlayer->GetDirection() != m_opponentPlayer->GetSprinningDirection())
	{
		m_opponentPlayer->SetCurrentDirectionByPreDirection();
	}


	if (m_opponentKey & (1 << 2))
	{
		// key s
		m_opponentPlayer->SetAction(PlayerAction::CROUCH);
	}

	// JUMP: key space
	if (m_opponentKey & (1 << 4) && !m_opponentPlayer->IsJumping() && m_opponentPlayer->GetCurrentAction() != PlayerAction::JUMPING)
	{
		m_opponentPlayer->SetAction(PlayerAction::JUMPING);
		desiredVel = b2Min(currentVelocity.x, currentVelocity.y - 3.0f);
		float jumpHeight = 5.0f;
		auto impulse = m_opponentPlayer->GetBody()->GetMass() * sqrt(2 * world->GetGravity().y * jumpHeight);
		m_opponentPlayer->GetBody()->ApplyLinearImpulseToCenter(b2Vec2(0.0f, -impulse), true);
	}

	if (!(m_opponentKey & 0xF))
	{
		// none of movement key is pressed
		m_opponentPlayer->SetAction(PlayerAction::IDLE);
	}
	if (m_opponentPlayer->IsJumping())
	{
		m_opponentPlayer->SetAction(PlayerAction::JUMPING);
	}

	// Fire
	if (m_opponentKey & (1 << 5) && m_opponentPlayer->IsReadyAttack())
	{
		m_opponentPlayer->ResetCooldown();
		b2Vec2 pos = m_opponentPlayer->GetBody()->GetPosition();
		b2Vec2 speed;

		switch (m_opponentPlayer->GetDirection())
		{
		case DirectionType::TOP:
			speed = b2Vec2(0.0f, -20.0f);
			break;
		case DirectionType::LEFT:
			speed = b2Vec2(-20.0f, 0.0f);
			break;
		case DirectionType::RIGHT:
			speed = b2Vec2(20.0f, 0.0f);
			break;
		}
		CreateBullet((BulletType)m_opponentPlayer->GetPlayerBulletType(), speed, Vector2(m_opponentPlayer->GetBody()->GetPosition().x, m_opponentPlayer->GetBody()->GetPosition().y));
	}
}

void GSSolo::CreateBullet(BulletType type, b2Vec2 speed, Vector2 position)
{
	for (auto bullet : m_bulletList)
	{
		if (!bullet->IsActive())
		{
			bullet->CreateNewBullet(type, speed, position);
			break;
		}
	}
}
