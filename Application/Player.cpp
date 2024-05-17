#include "stdafx.h"
#include "Player.h"
#include "ResourcesManager.h"
#include "b2Utilities.h"
#include "ConfigClass.h"
#include "Globals.h"

Player::Player()
{

}

Player::Player(b2World* world)
	: m_tileSizeByPixel(tileSizeByPixel)
	, m_direction(PlayerDirection::RIGHT)
	, m_isRightDirection(true)
	, m_currentAction(PlayerAction::IDLE)
{
	// create player body
	b2BodyDef playerBodyDef;
	playerBodyDef.type = b2_dynamicBody;
	playerBodyDef.position.Set(3.0f, 5.0f);
	playerBodyDef.userData.pointer = (uintptr_t)this;
	m_playerBody = world->CreateBody(&playerBodyDef);

	// set player body properties
	b2PolygonShape playerShape;
	playerShape.SetAsBox(0.5 * 3 / 2, 0.625 * 3 / 2); // size box = size img / 2
	b2FixtureDef playerFixtureDef;
	playerFixtureDef.shape = &playerShape;
	playerFixtureDef.density = 1.0f;
	playerFixtureDef.filter.categoryBits = FixtureTypes::FIXTURE_PLAYER;
	playerFixtureDef.filter.maskBits = FixtureTypes::FIXTURE_GROUND | FixtureTypes::FIXTURE_ENEMY_BULLET | FixtureTypes::FIXTURE_BOSS_BULLET | FixtureTypes::FIXTURE_ITEM;
	m_playerBodyFixture = m_playerBody->CreateFixture(&playerFixtureDef);
	m_playerBody->SetFixedRotation(true);

	// animation
	m_actionAnimation = ResourcesManager::GetInstance()->GetAnimation(29);
	m_actionAnimation->SetModel(ResourcesManager::GetInstance()->GetModel(ModelType::R_RETANGLE_CENTER));
	m_actionAnimation->Set2DPositionByTile(3, 5);
	m_actionAnimation->Set2DSizeByTile(3, 3);
}

Player::~Player()
{
}

void Player::Update(float deltaTime)
{
	m_actionAnimation->Update(deltaTime);
	m_actionAnimation->Set2DPositionByTile(m_playerBody->GetPosition().x, m_playerBody->GetPosition().y);
}

void Player::Draw()
{
	m_actionAnimation->Draw();
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
	m_tileSizeByPixel = tileSize;
}

void Player::SetDirection(PlayerDirection direction)
{
	if (m_direction != direction)
	{
		m_direction = direction;
		m_actionAnimation->FlipVertical();
	}
}

void Player::SetAction(PlayerAction action)
{
	if (m_currentAction != action)
	{
		auto pos = m_actionAnimation->GetPosition();
		auto size = m_actionAnimation->GetSize();
		auto model = m_actionAnimation->GetModel();
		m_actionAnimation = ResourcesManager::GetInstance()->GetAnimation(action);
		m_actionAnimation->SetModel(model);
		m_actionAnimation->Set2DPosition(pos.x, pos.y);
		m_actionAnimation->Set2DSize(size.x, size.y);
		m_currentAction = action;
	}
}

void Player::GetItem(GLint typeItem)
{
	
}

void Player::TakeDamage(GLint damage)
{

}

void Player::HandlePlayerDie(GLfloat deltaTime)
{
}