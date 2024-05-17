#include "stdafx.h"
#include "Bullet.h"
#include <iostream>
#include "ResourcesManager.h"
#include "Globals.h"

Bullet::Bullet(b2World* world, FixtureTypes bulletType)
{
	m_disabled = false;
	m_damage = 0;
	m_currentDirection = 1; // RIGHT
	m_isCollision = false;

	b2BodyDef bulletBodyDef;
	b2FixtureDef bulletFixtureDef;
	b2PolygonShape bulletShape;

	bulletBodyDef.type = b2_dynamicBody;
	bulletBodyDef.userData.pointer = (uintptr_t)this;
	bulletBodyDef.bullet = true;
	m_bulletBody = world->CreateBody(&bulletBodyDef);
	m_bulletBody->SetGravityScale(0.0f);
	auto model = ResourcesManager::GetInstance()->GetModel(ModelType::R_RETANGLE_TOPRIGHT);
	auto shader = ResourcesManager::GetInstance()->GetShader(0);
	auto texture = ResourcesManager::GetInstance()->GetTexture(26);
	m_bulletSprite = std::make_shared<Sprite2D>(0, model, shader, texture);
	switch (bulletType)
	{
	case FIXTURE_PLAYER_BULLET:
		bulletFixtureDef.filter.categoryBits = bulletType;
		bulletFixtureDef.filter.maskBits = FIXTURE_ENEMY | FIXTURE_GROUND | FIXTURE_BOSS;
		bulletShape.SetAsBox(0.09375f, 0.09375f);
		break;

	case FIXTURE_ENEMY_BULLET:
		bulletFixtureDef.filter.categoryBits = bulletType;
		bulletFixtureDef.filter.maskBits = FIXTURE_PLAYER | FIXTURE_GROUND;
		bulletShape.SetAsBox(0.09375f, 0.09375f);
		break;

	default:

		std::cout << "Invalid bullet fixture type: " << bulletType << "\n";
		break;
	}
	bulletFixtureDef.shape = &bulletShape;
	m_bulletFixture = m_bulletBody->CreateFixture(&bulletFixtureDef);
}

Bullet::Bullet(b2World* world)
{
	m_disabled = true;
	m_damage = 0;
	m_bulletFixture = nullptr;
	m_currentDirection = -1; // LEFT
	m_isCollision = false;
	b2BodyDef bulletBodyDef;

	bulletBodyDef.type = b2_dynamicBody;
	bulletBodyDef.userData.pointer = (uintptr_t)this;
	bulletBodyDef.bullet = true;
	m_bulletBody = world->CreateBody(&bulletBodyDef);
	m_bulletBody->SetGravityScale(0.0f);

	auto model = ResourcesManager::GetInstance()->GetModel(ModelType::R_RETANGLE_TOPRIGHT);
	auto shader = ResourcesManager::GetInstance()->GetShader(0);
	auto texture = ResourcesManager::GetInstance()->GetTexture(26);
	m_bulletSprite = std::make_shared<Sprite2D>(0, model, shader, texture);

	shader = ResourcesManager::GetInstance()->GetShader(1);
	texture = ResourcesManager::GetInstance()->GetTexture(30);
	m_bulletAnimation = std::make_shared<SpriteAnimation>(0, model, shader, texture, 9, 0.1f);
	m_bulletAnimation->Set2DSize(1.0f * m_tileSizeByPixel, 1.0f * m_tileSizeByPixel);
}

Bullet::~Bullet()
{

}

void Bullet::Set2DPosition(float x, float y)
{
	m_bulletBody->SetTransform(b2Vec2(x, y), 0.0f);
	float posX = x * m_tileSizeByPixel - m_bulletSprite->Get2DSize().x / 2.0f;
	float posY = y * m_tileSizeByPixel - m_bulletSprite->Get2DSize().y / 2.0f;
	m_bulletSprite->Set2DPosition(posX, posY);
	if (m_isCollision)
	{
		m_bulletAnimation->Set2DPosition(posX + 0.5f * m_currentDirection * m_tileSizeByPixel, posY);
		m_bulletBody->SetEnabled(false);
	}
}

void Bullet::Set2DSize(int x, int y)
{
	m_bulletSprite->Set2DSize(x, y);
}

void Bullet::Update(float deltaTime)
{
	if (m_isCollision && m_typeBullet == RPG_BULLET)
	{
		if (!m_bulletAnimation->IsLastFrame())
		{
			m_bulletAnimation->Update(deltaTime);
		}
		else
		{
			m_disabled = true;
		}
	}
}

void Bullet::SetBulletLinearVelocity(b2Vec2 speed)
{
	m_bulletBody->SetLinearVelocity(speed);

}

void Bullet::Draw()
{
	if (m_isCollision && m_typeBullet == RPG_BULLET)
	{
		m_bulletAnimation->Draw();
	}
	else
	{
		m_bulletSprite->Draw();
	}
}

void Bullet::CreateNewFixture(FixtureTypes bulletType)
{
	if (m_bulletFixture != nullptr)
	{
		m_bulletBody->DestroyFixture(m_bulletFixture);
	}

	b2FixtureDef bulletFixtureDef;
	b2PolygonShape bulletShape;

	Vector2 sizeBullet = Vector2(BULLET_SIZE * boxBullet.imgSize.x / HEIGHT_BULLET, BULLET_SIZE * boxBullet.imgSize.y / HEIGHT_BULLET);
	bulletFixtureDef.filter.categoryBits = bulletType;
	bulletShape.SetAsBox(sizeBullet.x, sizeBullet.y);

	switch (bulletType)
	{
	case FIXTURE_PLAYER_BULLET:
		bulletFixtureDef.filter.maskBits = FIXTURE_ENEMY | FIXTURE_GROUND | FIXTURE_BOSS;
		break;

	case FIXTURE_ENEMY_BULLET:
		bulletFixtureDef.filter.maskBits = FIXTURE_PLAYER | FIXTURE_GROUND;
		break;

	default:

		std::cout << "Invalid bullet fixture type: " << bulletType << "\n";
		break;
	}
	bulletFixtureDef.shape = &bulletShape;
	m_bulletFixture = m_bulletBody->CreateFixture(&bulletFixtureDef);

	size2D.x = 2 * sizeBullet.x;
	size2D.y = 2 * sizeBullet.y;

	this->Set2DSize(size2D.x * m_tileSizeByPixel, size2D.y * m_tileSizeByPixel);
}

void Bullet::CreateNewBullet(TypeBullet typeBullet, FixtureTypes typeFixture, b2Vec2 speed, GLfloat posX, GLfloat posY, GLint currentDirection)
{
	m_isCollision = false;
	m_typeBullet = typeBullet;

	if (m_currentDirection != currentDirection)
	{
		m_currentDirection = currentDirection;
		m_bulletSprite->FlipVertical();
	}

	switch (typeBullet)
	{
	case PLAYER_BULLET: // Bullet player
		boxBullet = SceneManager::GetInstance()->GetBoxBullet(26);
		this->Set2DPosition(posX, posY - 0.1875f);
		m_damage = 1;
		break;
	case AR_BULLET:
		boxBullet = SceneManager::GetInstance()->GetBoxBullet(27);
		this->Set2DPosition(posX + m_currentDirection * 0.5, posY);
		m_damage = 5;
		break;
	case RPG_BULLET:
		boxBullet = SceneManager::GetInstance()->GetBoxBullet(28);
		this->Set2DPosition(posX + m_currentDirection * 1.0f, posY - 0.2f); 
		m_bulletAnimation->SetCurrentFrame(0);
		m_damage = 10;
		break;
	case SNIPER_BULLET:
		boxBullet = SceneManager::GetInstance()->GetBoxBullet(29);
		this->Set2DPosition(posX + m_currentDirection * 1.5f, posY - 0.1f);
		m_damage = 8;
		break;
	case ENEMY_FLY_BULLET:
		boxBullet = SceneManager::GetInstance()->GetBoxBullet(42);
		this->Set2DPosition(posX , posY);
		m_damage = 3;
		break;
	default:
		break;
	}

	m_bulletSprite->SetTexture(ResourcesManager::GetInstance()->GetTexture(boxBullet.id));
	this->SetBulletLinearVelocity(speed);
	this->GetBulletBody()->SetEnabled(true);
	this->CreateNewFixture(typeFixture);
	this->m_disabled = false;
	
}

void Bullet::Set2DSizeScroll()
{
	this->Set2DSize(size2D.x * m_tileSizeByPixel, size2D.y * m_tileSizeByPixel);
}

void Bullet::SetTileSize(GLint tileSize)
{
	m_tileSizeByPixel = tileSize;
	m_bulletAnimation->Set2DSize(1.0f * m_tileSizeByPixel, 1.0f * m_tileSizeByPixel);
}




