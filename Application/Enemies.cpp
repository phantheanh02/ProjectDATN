#include "stdafx.h"
#include "Enemies.h"
#include "ResourcesManager.h"
#include "GSPlay.h"
#include "Globals.h"

extern int tileSizeByPixel;

Enemies::Enemies(EnemyType type)
	: m_type(type)
{
		
}

Enemies::Enemies(GLint id, GLint idTexture): m_id(id)
{
	Init();
}

Enemies::~Enemies()
{
}

void Enemies::Init()
{

}

void Enemies::Update(float deltaTime, std::shared_ptr<Player> player)
{

}

void Enemies::Draw()
{
}

void Enemies::Move()
{
}


void Enemies::Set2DPosition(GLint x, GLint y)
{
	m_animation->Set2DPosition(x, y);
}

void Enemies::Set2DSize(GLint width, GLint height)
{
	m_animation->Set2DSize(width, height);
}

void Enemies::Set2DSizeScroll()
{
}

Vector2 Enemies::Get2DSize()
{
	return m_animation->GetSize();
}


void Enemies::SetTexture(GLint typeAction)
{

}

void Enemies::Set2DPositionFromBox2D(GLfloat x, GLfloat y)
{

}

void Enemies::CreateEnemyBox(std::shared_ptr<b2World> world, float x, float y)
{
}

void Enemies::PerformRayCasting()
{
	b2RayCastInput rayInput;
	rayInput.p1 = m_body->GetPosition();
	// LEFT
	rayInput.p2 = rayInput.p1 + 100.0f * b2Vec2(-1.0f, 0.0f);
	m_body->GetWorld()->RayCast(&m_rayCallback, rayInput.p1, rayInput.p2);
	if (m_rayCallback.m_playerHit)
	{
		SetTexture(ARMOD_FIRE);
		m_isFire = true;
	}
	//RIGHT
	rayInput.p2 = rayInput.p1 + 100.0f * b2Vec2(1.0f, 0.0f);
	m_body->GetWorld()->RayCast(&m_rayCallback, rayInput.p1, rayInput.p2);
	if (m_rayCallback.m_playerHit)
	{
		SetTexture(ARMOD_FIRE);
		m_isFire = true;
	}
	////TOP
	//rayInput.p2 = rayInput.p1 + 100.0f * b2Vec2(0.0f, -1.0f);
	//m_enemyBody->GetWorld()->RayCast(&m_rayCallback, rayInput.p1, rayInput.p2);
	//if (m_rayCallback.m_playerHit)
	//{
	//	SetTexture(FIRE_UP);
	//	m_isFire = true;
	//}
	////DOWN
	//rayInput.p2 = rayInput.p1 + 100.0f * b2Vec2(0.0f, 1.0f);
	//m_enemyBody->GetWorld()->RayCast(&m_rayCallback, rayInput.p1, rayInput.p2);
	//if (m_rayCallback.m_playerHit)
	//{
	//	
	//	SetTexture(FIRE_DOWN);
	//	m_isFire = true;
	//}
}


b2Body* Enemies::GetEnemyBody()
{
	return m_body;
}

b2Fixture* Enemies::GetEnemyFixture()
{
	return m_fixture;
}

void Enemies::CalculateSize()
{
	
}

void Enemies::SetBoundaryMove(GLfloat x, GLfloat y)
{
}

void Enemies::TakeDamage(GLint damage)
{
	m_health -= damage;
	if (m_health <= 0)
	{
		Kill();
	}
	m_timeBarDisplay = 2.0f;
}

void Enemies::Kill()
{
	
}

void Enemies::RunModUpdate(std::shared_ptr<Player> player)
{
}

void Enemies::FindPlayerUpdate(std::shared_ptr<Player> player)
{
	
}

void Enemies::AttackUpdate(std::shared_ptr<Player> player)
{
	
}

void Enemies::EnemyFlyUpdate(std::shared_ptr<Player> player)
{
	
}

