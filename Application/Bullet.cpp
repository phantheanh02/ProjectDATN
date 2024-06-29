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
	m_body = world->CreateBody(&bulletBodyDef);
	m_body->SetGravityScale(0.0f);
	m_body->SetEnabled(false);
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
		auto deltaPos = m_body->GetLinearVelocity();
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
		m_animation->Set2DPositionByTile(m_body->GetPosition().x + deltaPos.x, m_body->GetPosition().y + deltaPos.y);
		m_animation->Update(deltaTime);
	}
	else
	{
		m_body->SetEnabled(false);
	}
}

void Bullet::SetBulletLinearVelocity(b2Vec2 speed)
{
	m_body->SetLinearVelocity(speed);

}

void Bullet::Draw()
{
	if (m_isActive)
	{
		m_animation->Draw();
	}
}

void Bullet::CreateNewBullet(BulletType type, b2Vec2 speed, Vector2 position, int damage, bool isOpponentCharacter)
{
	m_isActive = true;
	m_damage = damage;

	// First create fixture
	b2FixtureDef bulletFixtureDef;
	b2PolygonShape bulletShape;

	bulletFixtureDef.isSensor = true;

	switch (type)
	{
	case PLAYER_BULLET:
		m_animation = std::make_shared<SpriteAnimation>("Bullet/BlackBullet.png", 4, 0.1);
		bulletFixtureDef.filter.categoryBits = FIXTURE_PLAYER_BULLET;
		bulletFixtureDef.filter.maskBits = FIXTURE_ENEMY | FIXTURE_GROUND | FIXTURE_BOSS;
		break;
	case BLACK_BULLET:
		m_animation = std::make_shared<SpriteAnimation>("Bullet/BlackBullet.png", 4, 0.1);
		bulletFixtureDef.filter.categoryBits = FIXTURE_PLAYER_BULLET;
		bulletFixtureDef.filter.maskBits = FIXTURE_ENEMY | FIXTURE_GROUND | FIXTURE_BOSS;
		break;
	case BLUE_BULLET:
		m_animation = std::make_shared<SpriteAnimation>("Bullet/BlueBullet.png", 4, 0.1);
		bulletFixtureDef.filter.categoryBits = FIXTURE_PLAYER_BULLET;
		bulletFixtureDef.filter.maskBits = FIXTURE_ENEMY | FIXTURE_GROUND | FIXTURE_BOSS;
		break;
	case GREEN_BULLET:
		m_animation = std::make_shared<SpriteAnimation>("Bullet/GreenBullet.png", 4, 0.1);
		bulletFixtureDef.filter.categoryBits = FIXTURE_PLAYER_BULLET;
		bulletFixtureDef.filter.maskBits = FIXTURE_ENEMY | FIXTURE_GROUND | FIXTURE_BOSS;
		break;
	case RED_BULLET:
		m_animation = std::make_shared<SpriteAnimation>("Bullet/RedBullet.png", 4, 0.1);
		bulletFixtureDef.filter.categoryBits = FIXTURE_PLAYER_BULLET;
		bulletFixtureDef.filter.maskBits = FIXTURE_ENEMY | FIXTURE_GROUND | FIXTURE_BOSS;
		break;
	case YELLOW_BULLET:
		m_animation = std::make_shared<SpriteAnimation>("Bullet/YellowBullet.png", 4, 0.1);
		bulletFixtureDef.filter.categoryBits = FIXTURE_PLAYER_BULLET;
		bulletFixtureDef.filter.maskBits = FIXTURE_ENEMY | FIXTURE_GROUND | FIXTURE_BOSS;
		break;
	case AR_BULLET:
		m_animation = std::make_shared<SpriteAnimation>("Bullet/ARMBullet.png", 4, 0.1);
		bulletFixtureDef.filter.categoryBits = FIXTURE_ENEMY_BULLET;
		bulletFixtureDef.filter.maskBits = FIXTURE_PLAYER | FIXTURE_GROUND;
		break;
	case RPG_BULLET:
		m_animation = std::make_shared<SpriteAnimation>("Bullet/RPGBullet.png", 4, 0.1);
		bulletFixtureDef.filter.categoryBits = FIXTURE_ENEMY_BULLET;
		bulletFixtureDef.filter.maskBits = FIXTURE_PLAYER | FIXTURE_GROUND;
		break;
	case SNIPER_BULLET:
		m_animation = std::make_shared<SpriteAnimation>("Bullet/SniperBullet.png", 4, 0.1);
		bulletFixtureDef.filter.categoryBits = FIXTURE_ENEMY_BULLET;
		bulletFixtureDef.filter.maskBits = FIXTURE_PLAYER | FIXTURE_GROUND;
		break;
	case PATREON_BULLET:
		m_animation = std::make_shared<SpriteAnimation>("Bullet/PatreonBullet.png", 4, 0.1);
		bulletFixtureDef.filter.categoryBits = FIXTURE_ENEMY_BULLET;
		bulletFixtureDef.filter.maskBits = FIXTURE_PLAYER | FIXTURE_GROUND;
		break;
	case MEGAMAN_BULLET:
		m_animation = std::make_shared<SpriteAnimation>("Bullet/MegamanBullet.png", 4, 0.1);
		bulletFixtureDef.filter.categoryBits = FIXTURE_ENEMY_BULLET;
		bulletFixtureDef.filter.maskBits = FIXTURE_PLAYER | FIXTURE_GROUND;
		break;
	case YUME_BULLET:
		m_animation = std::make_shared<SpriteAnimation>("Bullet/YumeBullet.png", 4, 0.1);
		bulletFixtureDef.filter.categoryBits = FIXTURE_ENEMY_BULLET;
		bulletFixtureDef.filter.maskBits = FIXTURE_PLAYER | FIXTURE_GROUND;
		break;
	case NONE_BULLET:
		return;
	default:
		std::cout << "Invalid bullet fixture type: " << type << "\n";
		break;
	}

	if (isOpponentCharacter)
	{
		bulletFixtureDef.filter.categoryBits = FIXTURE_ENEMY_BULLET;
		bulletFixtureDef.filter.maskBits = FIXTURE_PLAYER | FIXTURE_GROUND;
	}
	bulletShape.SetAsBox(0.5, 0.5);
	bulletFixtureDef.shape = &bulletShape;
	if (m_bulletFixture)
	{
		m_body->DestroyFixture(m_bulletFixture);
	}
	m_body->SetTransform(b2Vec2(position.x, position.y), 0);
	m_bulletFixture = m_body->CreateFixture(&bulletFixtureDef);
	m_body->SetLinearVelocity(speed);
	m_body->SetEnabled(true);

	m_animation->SetModel(ResourcesManager::GetInstance()->GetModel(ModelType::R_RETANGLE_CENTER));
	m_animation->Set2DPositionByTile(position.x, position.y);
	m_animation->Set2DSizeByTile(1, 1);
	if (speed.x < 0)
	{
		m_animation->FlipVertical();
	}
	if (speed.y < 0)
	{
		m_animation->FlipHorizontal(false);
	}
	else if (speed.y > 0)
	{
		m_animation->FlipHorizontal(true);
	}
}

void Bullet::OnMouseScroll()
{
	auto pos = m_body->GetPosition();
	m_animation->Set2DPositionByTile(pos.x, pos.y);
	m_animation->Set2DSizeByTile(1, 1);
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




