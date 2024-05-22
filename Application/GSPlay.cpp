﻿#include "stdafx.h"
#include "GSPlay.h"

extern int tileSizeByPixel;
extern PlanetType currentMap;

GSPlay::~GSPlay()
{
	for (auto& it : m_BodyList)
	{
		m_world->DestroyBody(it);
	}
	m_world.reset();
	delete m_contactListener;

}

void GSPlay::Init()
{
	m_key = 0;
	unsigned int	m_mouse;

	// get refresh rate and set time step
	DEVMODE devMode;
	EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &devMode);
	m_timeStep = 1.0f / devMode.dmDisplayFrequency;

	// Box2d
	m_gravity = { 0.0f, 30.0f };
	// create world with gravity
	m_world = std::make_shared<b2World>(m_gravity);
	m_contactListener = new ContactListener();
	m_world->SetContactListener(m_contactListener);

	// Player
	m_player = std::make_shared<Player>(m_world.get());

	// Get camera
	m_dynamicCamera = SceneManager::GetInstance()->GetCamera(CameraType::DYNAMIC_CAMERA);
	m_staticCamera = SceneManager::GetInstance()->GetCamera(CameraType::STATIC_CAMERA);

	// Map Info
	m_map = SceneManager::GetInstance()->GetMap(currentMap);
	LoadMap();

	// init camera boundaries
	auto size = m_map->GetSizeByTile();
	m_cameraPositionBoudaries.x = 0;
	m_cameraPositionBoudaries.y = 0;
	m_cameraPositionBoudaries.z = size.x * tileSizeByPixel - (GLfloat)Globals::screenWidth;
	m_cameraPositionBoudaries.w = size.y * tileSizeByPixel - (GLfloat)Globals::screenHeight;

	// button play
	auto button = std::make_shared<Button>("btn_back.png", BUTTON_BACK);
	button->Set2DSize(220, 70);
	button->Set2DPosition(Globals::screenWidth / 2.0 - 78, Globals::screenHeight / 2.0f - 194);
	button->AttachCamera(m_staticCamera);
	m_listButton.push_back(button);

	// Create bullet pooling
	for (int i = 0; i < 100; i++)
	{
		m_bulletList.push_back(std::make_shared<Bullet>(m_world.get()));
	}
}

void GSPlay::Update(float deltaTime)
{
	HandleEvent();
	m_world->Step(m_timeStep, VELOCITY_ITERATION, POSITION_ITERATION);
	Update2DDrawPosition();

	
	for (auto it : m_bulletList)
	{
		it->Update(deltaTime);
	}

	m_player->Update(deltaTime);
}

void GSPlay::Draw()
{
	m_background->Draw();
	for (auto& button : m_listButton)
	{
		button->Draw();
	}
	for (auto it : m_bulletList)
	{
		it->Draw();
	}
	
	m_player->Draw();
}

void GSPlay::Pause()
{
}

void GSPlay::Resume()
{
}

void GSPlay::Exit()
{
}

void GSPlay::HandleEvent()
{
	auto currentVelocity = m_player->GetPlayerBody()->GetLinearVelocity();
	float desiredVel = 0, velChange, impulse;

	if (m_key & (1 << 1) && m_keyStack.back() == "A")
	{
		// move left
		desiredVel = b2Max(currentVelocity.x - 0.25f, -MOVEMENT_SPEED);
		m_player->SetDirection(DirectionType::LEFT);
	}
	if (m_key & (1 << 3) && m_keyStack.back() == "D")
	{
		// move right
		desiredVel = b2Min(currentVelocity.x + 0.25f, MOVEMENT_SPEED);
		m_player->SetDirection(DirectionType::RIGHT);
	}
	if (desiredVel != 0 && !m_player->IsJumping())
	{
		m_player->SetAction(PlayerAction::RUNNING);
	}
	// apply force to move
	velChange = desiredVel - currentVelocity.x;
	impulse = m_player->GetPlayerBody()->GetMass() * velChange; //disregard time factor
	m_player->GetPlayerBody()->ApplyLinearImpulse(b2Vec2(impulse, 0), m_player->GetPlayerBody()->GetWorldCenter(), true);

	if (m_key & (1 << 0) && velChange == 0 && !m_player->IsJumping())
	{
		// key w
		m_player->SetAction(PlayerAction::FIRE_TOP);
		m_player->SetDirection(DirectionType::TOP);
	}
	else
	{
		m_player->SetCurrentDirectionByPreDirection();
	}

	if (m_key & (1 << 2))
	{
		// key s
		m_player->SetAction(PlayerAction::CROUCH);
	}

	// JUMP: key space
	if (m_key & (1 << 4) && !m_player->IsJumping() && m_player->GetCurrentAction() != PlayerAction::JUMPING)
	{
		m_player->SetAction(PlayerAction::JUMPING);
		desiredVel = b2Min(currentVelocity.x, currentVelocity.y - 3.0f);
		float jumpHeight = 5.0f;
		auto impulse = m_player->GetPlayerBody()->GetMass() * sqrt(2 * m_gravity.y * jumpHeight);
		m_player->GetPlayerBody()->ApplyLinearImpulseToCenter(b2Vec2(0.0f, -impulse), true);
	}

	if (!(m_key & 0xF))
	{
		// none of movement key is pressed
		m_player->SetAction(PlayerAction::IDLE);
	}
	if (m_player->IsJumping())
	{
		m_player->SetAction(PlayerAction::JUMPING);
	}

	// Fire
	if (m_key & (1 << 5) && m_player->IsReadyAttack())
	{
		m_player->ResetCooldown();
		b2Vec2 pos = m_player->GetPlayerBody()->GetPosition();
		b2Vec2 speed;

		switch (m_player->GetDirection())
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
		ActiveABullet(TypeBullet::PLAYER_BULLET, speed, Vector2(m_player->GetPlayerBody()->GetPosition().x, m_player->GetPlayerBody()->GetPosition().y));
	}
}

void GSPlay::OnKey(unsigned char key, bool pressed)
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
			m_player->SetCurrentDirectionByPreDirection();
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

void GSPlay::OnMouseClick(int x, int y, unsigned char key, bool pressed)
{
	for (auto& button : m_listButton)
	{
		if (button->HandleTouchMouse(x, y, pressed))
		{
			ResourcesManager::GetInstance()->GetSound(1)->Play();
			switch (button->m_type)
			{
			case BUTTON_BACK:
				GameStateMachine::GetInstance()->PopState();
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

void GSPlay::OnMouseMove(int x, int y)
{
}

void GSPlay::OnMouseScroll(int x, int y, short delta)
{
	// Calculate tile size
	auto min = m_map->GetMinTileSize();
	auto max = m_map->GetMaxTileSize();
	tileSizeByPixel = delta > 0 ? tileSizeByPixel + 2 : tileSizeByPixel - 2;
	tileSizeByPixel = tileSizeByPixel < min ? min : tileSizeByPixel;
	tileSizeByPixel = tileSizeByPixel > max ? max : tileSizeByPixel;

	// Calculate size
	auto size = m_map->GetSizeByTile();
	m_background->Set2DSizeByTile(size.x, size.y);
	m_player->ReCalculateWhenScroll();
}

void GSPlay::Update2DDrawPosition()
{
	// recalculate camera boundary
	auto size = m_map->GetSizeByTile();
	m_cameraPositionBoudaries.z = size.x * tileSizeByPixel - (GLfloat)Globals::screenWidth;
	m_cameraPositionBoudaries.w = size.y * tileSizeByPixel - (GLfloat)Globals::screenHeight;

	// set camera position to follows player
	auto player_position = m_player->GetPlayerBody()->GetPosition();
	auto newCameraPos = Vector3(player_position.x * tileSizeByPixel - (GLfloat)Globals::screenWidth / 2, player_position.y * tileSizeByPixel - (GLfloat)Globals::screenHeight / 2, 0.0f);
	newCameraPos.x = newCameraPos.x <= m_cameraPositionBoudaries.x ? m_cameraPositionBoudaries.x : newCameraPos.x;
	newCameraPos.x = newCameraPos.x >= m_cameraPositionBoudaries.z ? m_cameraPositionBoudaries.z : newCameraPos.x;
	newCameraPos.y = newCameraPos.y <= m_cameraPositionBoudaries.y ? m_cameraPositionBoudaries.y : newCameraPos.y;
	newCameraPos.y = newCameraPos.y >= m_cameraPositionBoudaries.w ? m_cameraPositionBoudaries.w : newCameraPos.y;

	m_dynamicCamera->SetPosition(newCameraPos);
	m_dynamicCamera->SetTarget(Vector3(newCameraPos.x, newCameraPos.y, -1.0f));

	// set player draw position
	player_position.x -= 9.0f / 8;
	player_position.y -= 1.5f;
	m_player->Set2DPositionByTile(player_position.x, player_position.y );


}

void GSPlay::LoadMap()
{
	// background
	auto size = m_map->GetSizeByTile();
	m_background = std::make_shared<Sprite2D>(m_map->GetIdTexture());
	m_background->Set2DSizeByTile(size.x, size.y);
	m_background->Set2DPosition(0, 0);

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
		obsBody = m_world->CreateBody(&obsBodyDef);
		obsFixDef.shape = &obsShape;
		obsFixDef.filter.categoryBits = FixtureTypes::FIXTURE_GROUND;
		obsFixDef.filter.maskBits = 0xFFFF;
		obsBody->CreateFixture(&obsFixDef);
		m_BodyList.push_back(obsBody);
	}
}

void GSPlay::ActiveABullet(TypeBullet type, b2Vec2 speed, Vector2 position)
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
