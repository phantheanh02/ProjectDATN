#pragma once
#include "box2d.h"
#include "Player.h"

enum FixtureTypes
{
	FIXTURE_PLAYER			= 1 << 0,
	FIXTURE_PLAYER_FOOT		= 1 << 1,
	FIXTURE_GROUND			= 1 << 2,
	FIXTURE_PLAYER_BULLET	= 1 << 3,
	FIXTURE_ENEMY			= 1 << 4,
	FIXTURE_ENEMY_BULLET	= 1 << 5,
	FIXTURE_BOSS			= 1 << 6,
	FIXTURE_BOSS_BULLET		= 1 << 7,
	FIXTURE_ITEM			= 1 << 8
};

enum CollionTypes
{
	PLAYER_FOOT_GROUND		= FIXTURE_PLAYER_FOOT | FIXTURE_GROUND,
	PLAYER_GROUND			= FIXTURE_PLAYER | FIXTURE_GROUND,
	PLAYER_BULLET_ENEMY		= FIXTURE_PLAYER_BULLET | FIXTURE_ENEMY,
	PLAYER_BULLET_GROUND	= FIXTURE_PLAYER_BULLET | FIXTURE_GROUND,
	ENEMY_BULLET_GROUND		= FIXTURE_ENEMY_BULLET | FIXTURE_GROUND,
	ENEMY_BULLET_PLAYER		= FIXTURE_ENEMY_BULLET | FIXTURE_PLAYER,
	PLAYER_BULLET_BOSS		= FIXTURE_PLAYER_BULLET | FIXTURE_BOSS,
	BOSS_BULLET_PLAYER		= FIXTURE_BOSS_BULLET | FIXTURE_PLAYER,
	PLAYER_ITEM				= FIXTURE_PLAYER | FIXTURE_ITEM
};

class ContactListener : public b2ContactListener
{
public:
	void BeginContact(b2Contact* contact);
	void EndContact(b2Contact* contact);
private:
	void PlayerOnGround(b2Fixture* fixtureA, b2Fixture* fixtureB);
	void PlayerOnAir(b2Fixture* fixtureA, b2Fixture* fixtureB);
	void PlayerBulletEnemy(b2Fixture* fixtureA, b2Fixture* fixtureB);
	void PlayerBulletGround(b2Fixture* fixtureA, b2Fixture* fixtureB);
	void EnemyBulletPlayer(b2Fixture* fixtureA, b2Fixture* fixtureB);
	void EnemyBulletGround(b2Fixture* fixtureA, b2Fixture* fixtureB);
	void PlayerBulletBoss(b2Fixture* fixtureA, b2Fixture* fixtureB);
	void BossBulletPlayer(b2Fixture* fixtureA, b2Fixture* fixtureB);
	void PLayerLootItem(b2Fixture* fixtureA, b2Fixture* fixtureB);
};

class MyRayCastCallback : public b2RayCastCallback {
public:
	MyRayCastCallback()
	{
		m_fixture = NULL;
	}

	GLfloat ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, GLfloat fraction) override;

	b2Fixture*	m_fixture;
	b2Vec2		m_point;
	b2Vec2		m_normal;
	bool		m_playerHit = false; 
	float		m_fraction = 1.0f;
};
