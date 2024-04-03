#include "stdafx.h"
#include "GSPlay.h"


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
	m_gravity = { 0.0f, 20.0f };
	// create world with gravity
	m_world = std::make_shared<b2World>(m_gravity);
	m_contactListener = new ContactListener();
	m_world->SetContactListener(m_contactListener);

	// Player
	m_player = std::make_shared<Player>(m_world.get());
	m_player->Set2DSize(44 * 9.0f / 4, 44 * 9.0f / 4);
	m_lastPlayerHorizontalDirection = D_RIGHT;
	m_lastJumpTime = 0.0f;

	// Get camera
	m_dynamicCamera = SceneManager::GetInstance()->GetCamera(0);
	m_staticCamera = SceneManager::GetInstance()->GetCamera(1);

	// Map Info
	m_map = SceneManager::GetInstance()->GetCurrentMapInfo();
	LoadMap();

	// init camera boundaries
	m_cameraPositionBoudaries.x = 0;
	m_cameraPositionBoudaries.y = 0;
	m_cameraPositionBoudaries.z = m_map.sizeByTile.x * TILESIZEBYPIXEL - (GLfloat)Globals::screenWidth;
	m_cameraPositionBoudaries.w = m_map.sizeByTile.y * TILESIZEBYPIXEL - (GLfloat)Globals::screenHeight;

	// button play
	auto button = std::make_shared<Button>("btn_back.png", BUTTON_BACK);
	button->Set2DSize(220, 70);
	button->Set2DPosition(Globals::screenWidth / 2.0 - 78, Globals::screenHeight / 2.0f - 194);
	button->AttachCamera(m_staticCamera);
	m_listButton.push_back(button);

}

void GSPlay::Update(float deltaTime)
{
	HandleEvent();
	m_world->Step(m_timeStep, VELOCITY_ITERATION, POSITION_ITERATION);
	m_player->Update(deltaTime);
	Update2DDrawPosition();
}

void GSPlay::Draw()
{
	m_background->Draw();
	for (auto& button : m_listButton)
	{
		button->Draw();
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

	if (m_key & (1 << 1))
	{
		// move left
		m_player->m_iActionDirection = D_LEFT;
		m_player->m_iBotAction = RUNNING;
		desiredVel = b2Max(currentVelocity.x - 0.25f, -MOVEMENT_SPEED);
		m_lastPlayerHorizontalDirection = D_LEFT;
	}
	if (m_key & (1 << 3))
	{
		// move right
		m_player->m_iActionDirection = D_RIGHT;
		m_player->m_iBotAction = RUNNING;
		desiredVel = b2Min(currentVelocity.x + 0.25f, MOVEMENT_SPEED);
		m_lastPlayerHorizontalDirection = D_RIGHT;
	}
	if (m_key & (1 << 0))
	{
		// key w
		m_player->m_iActionDirection = D_UP;
	}
	if (m_key & (1 << 2))
	{
		// key s
		m_player->m_iActionDirection = D_DOWN;
	}
	if (!(m_key & 0xF))
	{
		// none of movement key is pressed
		m_player->m_iBotAction = IDLE;
		m_player->m_iActionDirection = m_lastPlayerHorizontalDirection;
		desiredVel = currentVelocity.x * 0.8f;
	}
	if (m_key & (1 << 5))
	{
		b2Vec2 pos = m_player->GetPlayerBody()->GetPosition();
		b2Vec2 speed;
		switch (m_player->m_iActionDirection)
		{
		case D_UP:
			speed = b2Vec2(0.0f, -20.0f);
			break;
		case D_DOWN:
			speed = b2Vec2(0.0f, 20.0f);
			break;
		case D_LEFT:
			speed = b2Vec2(-20.0f, 0.0f);
			break;
		case D_RIGHT:
			speed = b2Vec2(20.0f, 0.0f);
			break;
		}
		GLint currentDirec = m_player->m_flipped ? -1 : 1;
	}
	else
	{
		m_player->m_iTopAction = IDLE;
	}
	// check if on air
	if (!m_player->m_onGround)
	{
		m_player->m_iBotAction = Action::JUMPING;
	}

	// apply force to move
	velChange = desiredVel - currentVelocity.x;
	impulse = m_player->GetPlayerBody()->GetMass() * velChange; //disregard time factor
	m_player->GetPlayerBody()->ApplyLinearImpulse(b2Vec2(impulse, 0), m_player->GetPlayerBody()->GetWorldCenter(), true);

	if (m_key & (1 << 4) && m_player->m_onGround)
	{
		// jump
		float jumpHeight = 3.0f;
		auto impulse = m_player->GetPlayerBody()->GetMass() * sqrt(2 * m_gravity.y * jumpHeight);
		m_player->GetPlayerBody()->ApplyLinearImpulseToCenter(b2Vec2(0.0f, -impulse), true);
		m_lastJumpTime = 0.0f;
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
			break;
		case KEY_S:
			m_key |= 1 << 2;
			break;
		case KEY_D:
			m_key |= 1 << 3;
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
			break;
		case KEY_A:
			m_key ^= 1 << 1;
			break;
		case KEY_S:
			m_key ^= 1 << 2;
			break;
		case KEY_D:
			m_key ^= 1 << 3;
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
}

void GSPlay::Update2DDrawPosition()
{
	// recalculate camera boundary
	m_cameraPositionBoudaries.z = m_map.sizeByTile.x * 44 - (GLfloat)Globals::screenWidth;
	m_cameraPositionBoudaries.w = m_map.sizeByTile.y * 44 - (GLfloat)Globals::screenHeight;

	// set camera position to follows player
	auto player_position = m_player->GetPlayerBody()->GetPosition();
	auto newCameraPos = Vector3(player_position.x * 44 - (GLfloat)Globals::screenWidth / 2, player_position.y * 44 - (GLfloat)Globals::screenHeight / 2, 0.0f);
	newCameraPos.x = newCameraPos.x <= m_cameraPositionBoudaries.x ? m_cameraPositionBoudaries.x : newCameraPos.x;
	newCameraPos.x = newCameraPos.x >= m_cameraPositionBoudaries.z ? m_cameraPositionBoudaries.z : newCameraPos.x;
	newCameraPos.y = newCameraPos.y <= m_cameraPositionBoudaries.y ? m_cameraPositionBoudaries.y : newCameraPos.y;
	newCameraPos.y = newCameraPos.y >= m_cameraPositionBoudaries.w ? m_cameraPositionBoudaries.w : newCameraPos.y;

	m_dynamicCamera->SetPosition(newCameraPos);
	m_dynamicCamera->SetTarget(Vector3(newCameraPos.x, newCameraPos.y, -1.0f));

	// set player draw position
	player_position.x -= 9.0f / 8;
	player_position.y -= 1.5f;
	m_player->Set2DPosition(player_position.x * 44, player_position.y * 44);


}

void GSPlay::LoadMap()
{
	// background
	m_background = std::make_shared<Sprite2D>(m_map.idTexture);
	m_background->Set2DSize(TILESIZEBYPIXEL * m_map.sizeByTile.x, TILESIZEBYPIXEL * m_map.sizeByTile.y);
	m_background->Set2DPosition(0, 0);

	// Load obs
	b2Body* obsBody;
	b2BodyDef obsBodyDef;
	b2PolygonShape obsShape;
	b2FixtureDef obsFixDef;
	for (auto it : m_map.plane)
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
