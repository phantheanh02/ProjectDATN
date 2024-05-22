#include "stdafx.h"
#include "Bullet.h"
#include <iostream>
#include "ResourcesManager.h"
#include "Globals.h"

Bullet::Bullet(b2World* world)
	: m_bulletFixture(nullptr)
	, m_isActive(false)
{
	b2BodyDef bulletBodyDef;
	b2FixtureDef bulletFixtureDef;
	b2PolygonShape bulletShape;

	bulletBodyDef.type = b2_dynamicBody;
	bulletBodyDef.userData.pointer = (uintptr_t)this;
	bulletBodyDef.bullet = true;
	m_bulletBody = world->CreateBody(&bulletBodyDef);
	m_bulletBody->SetGravityScale(0.0f);
	m_bulletBody->SetEnabled(false);
}

Bullet::~Bullet()
{

}

void Bullet::Set2DPosition(float x, float y)
{
	
}

void Bullet::Set2DPositionByTile(GLfloat x, GLfloat y)
{
}

void Bullet::Set2DSize(int x, int y)
{
}

void Bullet::Update(float deltaTime)
{
	if (m_isActive)
	{
		auto deltaPos = m_bulletBody->GetLinearVelocity();
		deltaPos.Normalize(); // x = 0 or y =0
		if (deltaPos.y == 0)
		{
			//if (deltaPos.x == 1)
			//{
			//	deltaPos.x += 0.0;
			//}
			//else
			//{
			//	deltaPos.x -= 0.0;
			//}
			deltaPos.y -= 0.15;
		}
		else
		{
			deltaPos.x -= 0.15;
			deltaPos.y -= 0.4;
		}
		m_bulletAnimation->Set2DPositionByTile(m_bulletBody->GetPosition().x + deltaPos.x, m_bulletBody->GetPosition().y + deltaPos.y);
		m_bulletAnimation->Update(deltaTime);
	}
}

void Bullet::SetBulletLinearVelocity(b2Vec2 speed)
{
	m_bulletBody->SetLinearVelocity(speed);

}

void Bullet::Draw()
{
	if (m_isActive)
	{
		m_bulletAnimation->Draw();
	}
}

void Bullet::CreateNewBullet(TypeBullet type, b2Vec2 speed, Vector2 position)
{
	m_isActive = true;
	// First create fixture
	b2FixtureDef bulletFixtureDef;
	b2PolygonShape bulletShape;
	switch (type)
	{
	case PLAYER_BULLET:
		bulletFixtureDef.filter.categoryBits = FIXTURE_PLAYER_BULLET;
		bulletFixtureDef.filter.maskBits = FIXTURE_ENEMY | FIXTURE_GROUND | FIXTURE_BOSS;
		m_bulletAnimation = std::make_shared<SpriteAnimation>(59, 4, 0.1);
		m_bulletAnimation->SetModel(ResourcesManager::GetInstance()->GetModel(ModelType::R_RETANGLE_CENTER));
		m_bulletAnimation->Set2DPositionByTile(position.x, position.y);
		m_bulletAnimation->Set2DSizeByTile(0.5, 0.5);
		break;
	case FIXTURE_ENEMY_BULLET:
		bulletFixtureDef.filter.categoryBits = FIXTURE_ENEMY_BULLET;
		bulletFixtureDef.filter.maskBits = FIXTURE_PLAYER | FIXTURE_GROUND;
		break;
	default:
		std::cout << "Invalid bullet fixture type: " << type << "\n";
		break;
	}
	bulletShape.SetAsBox(0.5, 0.5);
	bulletFixtureDef.shape = &bulletShape;
	if (m_bulletFixture)
	{
		m_bulletBody->DestroyFixture(m_bulletFixture);
	}

	m_bulletBody->SetTransform(b2Vec2(position.x, position.y), 0);
	m_bulletFixture = m_bulletBody->CreateFixture(&bulletFixtureDef);
	m_bulletBody->SetLinearVelocity(speed);
	m_bulletBody->SetEnabled(true);
}


void Bullet::SetDamage(GLint damage)
{
	m_damage = damage;
}

void Bullet::SetActiveStatus(bool status)
{
	if (m_isActive != status)
	{
		m_isActive = status;
	}
}




