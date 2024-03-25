#include "stdafx.h"
#include "Player.h"
#include "ResourcesManager.h"
#include "b2Utilities.h"
#include "ConfigClass.h"

Player::Player()
{

}

Player::Player(b2World* world)
{
	auto model = ResourcesManager::GetInstance()->GetModel(0);
	auto shader = ResourcesManager::GetInstance()->GetShader(1);
	auto texture = ResourcesManager::GetInstance()->GetTexture(12);
	m_topIdleAnimation = std::make_shared<SpriteAnimation>(0, model, shader, texture, 1, 9, 0.1f);

	texture = ResourcesManager::GetInstance()->GetTexture(16);
	m_topShootingAnimation = std::make_shared<SpriteAnimation>(0, model, shader, texture, 3, 8, 0.1f);

	texture = ResourcesManager::GetInstance()->GetTexture(13);
	m_botRunningAnimation = std::make_shared<SpriteAnimation>(0, model, shader, texture, 1, 8, 0.1f);

	shader = ResourcesManager::GetInstance()->GetShader(0);
	texture = ResourcesManager::GetInstance()->GetTexture(14);
	m_botIdleSprite = std::make_shared<Sprite2D>(0, model, shader, texture);

	texture = ResourcesManager::GetInstance()->GetTexture(15);
	m_botJumpingSprite = std::make_shared<Sprite2D>(0, model, shader, texture);

	texture = ResourcesManager::GetInstance()->GetTexture(46);
	m_topAimDown = std::make_shared<Sprite2D>(0, model, shader, texture);

	texture = ResourcesManager::GetInstance()->GetTexture(47);
	m_topAimUp = std::make_shared<Sprite2D>(0, model, shader, texture);

	m_iBotAction = m_iTopAction = Action::IDLE;
	m_iActionDirection = Direction::D_RIGHT;
	m_needUpdatePosition = true;
	m_needUpdateSize = true;
	m_flipped = false;
	// create player body
	b2BodyDef playerBodyDef;
	playerBodyDef.type = b2_dynamicBody;
	playerBodyDef.position.Set(3.0f, 5.0f);
	playerBodyDef.userData.pointer = (uintptr_t)this;
	m_playerBody = world->CreateBody(&playerBodyDef);

	// set player body properties
	b2PolygonShape playerShape;
	playerShape.SetAsBox(0.375f, 0.75f);
	b2FixtureDef playerFixtureDef;
	playerFixtureDef.shape = &playerShape;
	playerFixtureDef.density = 1.0f;
	playerFixtureDef.filter.categoryBits = FixtureTypes::FIXTURE_PLAYER;
	playerFixtureDef.filter.maskBits = FixtureTypes::FIXTURE_GROUND | FixtureTypes::FIXTURE_ENEMY_BULLET | FixtureTypes::FIXTURE_BOSS_BULLET | FixtureTypes::FIXTURE_ITEM;
	m_playerBodyFixture = m_playerBody->CreateFixture(&playerFixtureDef);
	m_playerBody->SetFixedRotation(true);

	// foot sensor for detect standing on ground
	b2PolygonShape sensorShape;
	b2FixtureDef sensorFixtureDef;
	b2FixtureUserData userData;
	sensorShape.SetAsBox(0.3f, 0.2f, b2Vec2(0, 1), 0);
	sensorFixtureDef.shape = &sensorShape;
	sensorFixtureDef.isSensor = true;
	sensorFixtureDef.filter.categoryBits = FixtureTypes::FIXTURE_PLAYER_FOOT;
	sensorFixtureDef.filter.maskBits = FixtureTypes::FIXTURE_GROUND;
	sensorFixtureDef.userData.pointer = (uintptr_t)this;
	m_footSensorFixture = m_playerBody->CreateFixture(&sensorFixtureDef);

	m_onGround = false;
	m_health = HP_MAX;
	m_armor = ARMOR_MAX;
	m_numberBullet = NUMBER_BULLET_MAX;
	m_keyItem = 0;
	m_coin = 0;
	m_isPLayerDie = false;
	m_timeDie = 5.0f;
	m_isLost = false;
	m_isVictory = false;
	// Bar
	shader = ResourcesManager::GetInstance()->GetShader(0);
	texture = ResourcesManager::GetInstance()->GetTexture(58);
	m_hpBar = std::make_shared<Sprite2D>(0, model, shader, texture);

	texture = ResourcesManager::GetInstance()->GetTexture(59);
	m_armorBar = std::make_shared<Sprite2D>(0, model, shader, texture);

}

Player::~Player()
{
}

void Player::Update(float deltaTime)
{
	if (!m_isPLayerDie)
	{
		m_armorBar->Set2DSize(1 * m_tileSizeByPixel * m_armor / ARMOR_MAX, 3);
		m_armorBar->Set2DPosition((m_playerBody->GetPosition().x - 0.5) * m_tileSizeByPixel,
			(m_playerBody->GetPosition().y - 1) * m_tileSizeByPixel);

		m_hpBar->Set2DSize(1 * m_tileSizeByPixel * m_health / HP_MAX, 3);
		m_hpBar->Set2DPosition((m_playerBody->GetPosition().x - 0.5) * m_tileSizeByPixel,
			(m_playerBody->GetPosition().y - 1.2) * m_tileSizeByPixel);

		if (m_needUpdatePosition)
		{
			m_topIdleAnimation->Set2DPosition(m_pos.x, m_pos.y);
			m_topShootingAnimation->Set2DPosition(m_pos.x, m_pos.y);
			m_botRunningAnimation->Set2DPosition(m_pos.x, m_pos.y);
			m_botIdleSprite->Set2DPosition(m_pos.x, m_pos.y);
			m_botJumpingSprite->Set2DPosition(m_pos.x, m_pos.y);
			m_topAimDown->Set2DPosition(m_pos.x, m_pos.y);
			m_topAimUp->Set2DPosition(m_pos.x, m_pos.y);
			m_needUpdatePosition = false;
		}

		if (m_needUpdateSize)
		{
			m_topIdleAnimation->Set2DSize(m_size.x, m_size.y);
			m_topShootingAnimation->Set2DSize(m_size.x, m_size.y);
			m_botRunningAnimation->Set2DSize(m_size.x, m_size.y);
			m_botIdleSprite->Set2DSize(m_size.x, m_size.y);
			m_botJumpingSprite->Set2DSize(m_size.x, m_size.y);
			m_topAimDown->Set2DSize(m_size.x, m_size.y);
			m_topAimUp->Set2DSize(m_size.x, m_size.y);
			m_needUpdateSize = false;
		}

		switch (m_iActionDirection)
		{
		case D_LEFT:
			m_topShootingAnimation->SetAction(0);
			if (m_flipped)
			{
				break;
			}
			m_topIdleAnimation->FlipVertical();
			m_topShootingAnimation->FlipVertical();
			m_botIdleSprite->FlipVertical();
			m_botJumpingSprite->FlipVertical();
			m_botRunningAnimation->FlipVertical();
			m_topAimDown->FlipVertical();
			m_topAimUp->FlipVertical();
			m_flipped = true;
			break;

		case D_RIGHT:
			m_topShootingAnimation->SetAction(0);
			if (!m_flipped)
			{
				break;
			}
			m_topIdleAnimation->FlipVertical();
			m_topShootingAnimation->FlipVertical();
			m_botIdleSprite->FlipVertical();
			m_botJumpingSprite->FlipVertical();
			m_botRunningAnimation->FlipVertical();
			m_topAimDown->FlipVertical();
			m_topAimUp->FlipVertical();
			m_flipped = false;
			break;

		case D_UP:
			m_topShootingAnimation->SetAction(1);
			break;

		case D_DOWN:
			m_topShootingAnimation->SetAction(2);
			break;

		default:
			break;
		}

		m_topIdleAnimation->Update(deltaTime);
		m_topShootingAnimation->Update(deltaTime);
		m_botRunningAnimation->Update(deltaTime);
	}
	else
	{
		HandlePlayerDie(deltaTime);
	}
}

void Player::Draw()
{
	if (!m_isPLayerDie)
	{
		switch (m_iTopAction | m_iActionDirection)
		{
		case Action::IDLE | D_LEFT:
		case Action::IDLE | D_RIGHT:
			m_topIdleAnimation->Draw();
			break;
		case Action::IDLE | D_UP:
			m_topAimUp->Draw();
			break;
		case Action::IDLE | D_DOWN:
			m_topAimDown->Draw();
			break;
		case Action::SHOOTING | D_UP:
		case Action::SHOOTING | D_DOWN:
		case Action::SHOOTING | D_LEFT:
		case Action::SHOOTING | D_RIGHT:
			m_topShootingAnimation->Draw();
			break;
		}

		switch (m_iBotAction)
		{
		case Action::IDLE:
			m_botIdleSprite->Draw();
			break;
		case Action::RUNNING:
			m_botRunningAnimation->Draw();
			break;
		case Action::JUMPING:
			m_botJumpingSprite->Draw();
			break;
		}

		m_hpBar->Draw();
		m_armorBar->Draw();
	}
	else 
	{
		if (m_timeDie >= 0)
		{
			m_DieAnimation->Draw();
		}
		else
		{
			m_isLost = true;
		}
	}
}

void Player::Set2DPosition(GLint x, GLint y)
{
	m_pos.x = x;
	m_pos.y = y;
	m_needUpdatePosition = true;
}

Vector2 Player::GetSize()
{
	return m_pos;
}

void Player::Set2DSize(GLint width, GLint height)
{
	m_size.x = (GLfloat)width;
	m_size.y = (GLfloat)height;
	m_needUpdateSize = true;
}

void Player::GetItem(GLint typeItem)
{
	switch (typeItem)
	{
	case TypeItem::HEALING_ITEM:
		m_health = m_health + 10 > HP_MAX ? HP_MAX : m_health + 10;
		break;
	case TypeItem::KEY_ITEM:
		m_keyItem = 1;
		break;
	case TypeItem::CUP_ITEM:
		m_isVictory = true;
		break;
	default:
		break;
	}
}

void Player::TakeDamage(GLint damage)
{
	if (m_armor > 0)
	{
		m_armor = m_armor - damage > 0 ? m_armor - damage : 0;
	}
	else
	{
		m_health = m_health - (damage + 1) > 0 ? m_health - (damage + 1) : 0;
	}
	if (m_health <= 0)
	{
		// TODO: player die
		printf("Player die\n");
		ResourcesManager::GetInstance()->GetSound(5)->Play();
		m_isPLayerDie = true;
		auto model = ResourcesManager::GetInstance()->GetModel(0);
		auto shader = ResourcesManager::GetInstance()->GetShader(1);
		auto texture = ResourcesManager::GetInstance()->GetTexture(71);

		m_DieAnimation = std::make_shared<SpriteAnimation>(0, model, shader, texture, 1, 5, 0.1f);
		m_DieAnimation->SetCurrentFrame(0);
		m_DieAnimation->Set2DSize(m_size.x, m_size.y);
		m_DieAnimation->Set2DPosition(m_pos.x, m_pos.y);
		if (m_flipped)
		{
			m_DieAnimation->FlipVertical();
		}
	}
}

void Player::HandlePlayerDie(GLfloat deltaTime)
{
	m_playerBody->SetEnabled(false);
	m_DieAnimation->Set2DPosition(m_pos.x, m_pos.y);
	if (!m_DieAnimation->IsLastFrame())
	{
		m_DieAnimation->Update(deltaTime);
	}
	m_timeDie -= deltaTime;
}