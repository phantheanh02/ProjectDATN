#include "stdafx.h"
#include "Player.h"
#include "ResourcesManager.h"
#include "b2Utilities.h"
#include "ConfigClass.h"
#include "Globals.h"

extern int tileSizeByPixel;
extern CharacterType currentCharacter;
extern std::shared_ptr<b2World> world;

Player::Player()
	: m_currentDirection(DirectionType::RIGHT)
	, m_sprinningDirection(DirectionType::RIGHT)
	, m_currentAction(PlayerAction::IDLE)
	, m_isJumping(false)
	, m_bulletCooldown(0)
	, m_isDie(false)
	, m_resetAfterDieTime(5)
	, m_health(100)
{
	// create player body
	b2BodyDef playerBodyDef;
	playerBodyDef.type = b2_dynamicBody;
	playerBodyDef.position.Set(9.0f, 18.0f);
	playerBodyDef.userData.pointer = (uintptr_t)this;
	m_body = world->CreateBody(&playerBodyDef);

	// set player body properties
	b2PolygonShape playerShape;
	playerShape.SetAsBox(0.5 * SCALE_SIZE / 2, 2 / 3.0f * SCALE_SIZE / 2); // size box = size img / 2
	b2FixtureDef playerFixtureDef;
	playerFixtureDef.shape = &playerShape;
	playerFixtureDef.density = 1.0f; 
	playerFixtureDef.filter.categoryBits = FixtureTypes::FIXTURE_PLAYER;
	playerFixtureDef.filter.maskBits = FixtureTypes::FIXTURE_GROUND | FixtureTypes::FIXTURE_ENEMY_BULLET | FixtureTypes::FIXTURE_ENEMY | FixtureTypes::FIXTURE_ITEM;
	playerFixtureDef.userData.pointer = (uintptr_t)this;
	m_body->CreateFixture(&playerFixtureDef);
	m_body->SetFixedRotation(true); 

	// Create sensor foot to check in ground
	b2PolygonShape sensorBox;
	b2FixtureDef sensorDef;
	sensorBox.SetAsBox(0.5 * SCALE_SIZE / 2, 0.1f, b2Vec2(0, 1), 0); // sensor foot
	sensorDef.shape = &sensorBox;
	sensorDef.isSensor = true;
	sensorDef.filter.categoryBits = FixtureTypes::FIXTURE_PLAYER_FOOT;
	sensorDef.filter.maskBits = FixtureTypes::FIXTURE_GROUND;
	sensorDef.userData.pointer = (uintptr_t)this;
	m_body->CreateFixture(&sensorDef);

	// animation
	m_actionAnimation = std::make_shared<SpriteAnimation>(currentCharacter, 5, 0.1);
	m_actionAnimation->Set2DPositionByTile(3, 5);
	m_actionAnimation->Set2DSizeByTile(SCALE_SIZE, SCALE_SIZE);

	switch (currentCharacter)
	{
	case C_BLACK:
		m_playerbulletType = 1;
		break;
	case C_BLUE:
		m_playerbulletType = 2;
		break;
	case C_GREEN:
		m_playerbulletType = 3;
		break;
	case C_RED:
		m_playerbulletType = 4;
		break;
	case C_YELLOW:
		m_playerbulletType = 5;
		break;
	default:
		break;
	}
}

Player::~Player()
{
}

void Player::Update(float deltaTime)
{
	if (m_isDie)
	{
		m_resetAfterDieTime -= deltaTime;
		SetAction(PlayerAction::DEAD);
		//m_body->SetEnabled(false);
		if (m_actionAnimation->GetCurrentFrame() != m_actionAnimation->GetNumFrames() - 1)
		{
			m_actionAnimation->Set2DPositionByTile(m_body->GetPosition().x, m_body->GetPosition().y);
			m_actionAnimation->Update(deltaTime);
		}
		return;
	}
	if (!m_isJumping)
	{
		m_actionAnimation->Update(deltaTime);
	}
	m_actionAnimation->Set2DPositionByTile(m_body->GetPosition().x, m_body->GetPosition().y);
	if (m_bulletCooldown > 0)
	{
		m_bulletCooldown -= deltaTime;
	}
}

void Player::Draw()
{
	m_actionAnimation->Draw();
}

void Player::SetCurrentDirectionByPreDirection()
{
	m_currentDirection = m_sprinningDirection;
}

void Player::Set2DPosition(GLint x, GLint y)
{
	m_pos.x = x;
	m_pos.y = y;
}

void Player::Set2DPositionByTile(GLfloat x, GLfloat y)
{
	m_pos.x = x * tileSizeByPixel;
	m_pos.y = y * tileSizeByPixel;
}

void Player::Set2DSize(GLint width, GLint height)
{
	m_size.x = (GLfloat)width;
	m_size.y = (GLfloat)height;
}

void Player::Set2DSizeByTile(GLfloat width, GLfloat height)
{
	m_size.x = tileSizeByPixel * width;
	m_size.y = tileSizeByPixel * height;
}

void Player::SetTileSize(GLint tileSize)
{
}

void Player::SetDirection(DirectionType direction)
{
	if (m_currentDirection != direction)
	{
		if (direction != DirectionType::TOP && direction != m_sprinningDirection)
		{
			m_actionAnimation->FlipVertical();
			m_sprinningDirection = direction;
		}
		m_currentDirection = direction;
	}
}

void Player::SetAction(PlayerAction action)
{
	if (m_currentAction != action && m_currentAction != DEAD)
	{
		auto pos = m_actionAnimation->GetPosition();
		auto size = m_actionAnimation->GetSize();
		auto model = m_actionAnimation->GetModel();
		switch (action)
		{
		case IDLE:
			m_actionAnimation = std::make_shared<SpriteAnimation>(action + currentCharacter, 5, 0.1f);
			break;
		case RUNNING:
			m_actionAnimation = std::make_shared<SpriteAnimation>(action + currentCharacter, 6, 0.1f);
			break;
		case JUMPING:
			m_actionAnimation = std::make_shared<SpriteAnimation>(action + currentCharacter, 2, 0.1f);
			break;
		case CROUCH:
			m_actionAnimation = std::make_shared<SpriteAnimation>(action + currentCharacter, 3, 0.1f);
			break;
		case DEAD:
			m_actionAnimation = std::make_shared<SpriteAnimation>(action + currentCharacter, 8, 0.1f);
			m_actionAnimation->SetCurrentFrame(0);
			break;
		case FIRE_TOP:
			m_actionAnimation = std::make_shared<SpriteAnimation>(action + currentCharacter, 1, 0.1f);
			break;
		case NONE_ACTION:
			break;
		default:
			break;
		}
		m_actionAnimation->Set2DPosition(pos.x, pos.y);
		m_actionAnimation->Set2DSize(size.x, size.y);
		m_actionAnimation->SetModel(model);
		m_currentAction = action;
		if (action == PlayerAction::JUMPING)
		{
			m_actionAnimation->SetCurrentFrame(m_actionAnimation->GetNumFrames() - 1); // Set is final frame
			m_isJumping = true;
		}
	}
}

void Player::SetJumpingStatus(bool status)
{
	m_isJumping = status;
}

void Player::GetItem(GLint typeItem)
{
	
}

DirectionType Player::GetDirection()
{
	return m_currentDirection;
}

void Player::TakeDamage(GLint damage)
{
	m_health -= damage;
	if (m_health <= 0)
	{
		m_isDie = true;
	}
}

void Player::ReCalculateWhenScroll()
{
	Set2DSizeByTile(SCALE_SIZE, SCALE_SIZE);
	m_actionAnimation->Set2DSizeByTile(SCALE_SIZE, SCALE_SIZE);
}

void Player::HandlePlayerDie(GLfloat deltaTime)
{
}