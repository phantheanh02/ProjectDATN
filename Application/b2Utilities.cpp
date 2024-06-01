#include "stdafx.h"
#include "b2Utilities.h"
#include "Bullet.h"
#include "Enemies.h"
#include <iostream>
#include "Boss.h"
#include "Item.h"
#include "box2d.h"
#include "Player.h"

void ContactListener::BeginContact(b2Contact* contact)
{
	auto fixtureA = contact->GetFixtureA();
	auto fixtureB = contact->GetFixtureB();
	auto collisionType = fixtureA->GetFilterData().categoryBits | fixtureB->GetFilterData().categoryBits;

	switch (collisionType)
	{
	case CollionTypes::PLAYER_FOOT_GROUND:
		PlayerOnGround(fixtureA, fixtureB);
		break;
	case CollionTypes::PLAYER_BULLET_ENEMY:
		PlayerBulletEnemy(fixtureA, fixtureB);
		break;
	case CollionTypes::PLAYER_BULLET_GROUND:
		PlayerBulletGround(fixtureA, fixtureB);
		break;
	case CollionTypes::ENEMY_BULLET_PLAYER:
		EnemyBulletPlayer(fixtureA, fixtureB);
		break;
	case CollionTypes::ENEMY_BULLET_GROUND:
		EnemyBulletGround(fixtureA, fixtureB);
		break;
	case CollionTypes::PLAYER_BULLET_BOSS:
		PlayerBulletBoss(fixtureA, fixtureB);
		break;
	case CollionTypes::BOSS_BULLET_PLAYER:
		BossBulletPlayer(fixtureA, fixtureB);
		break;
	case CollionTypes::PLAYER_ITEM:
		PLayerLootItem(fixtureA, fixtureB);
		break;
	default:
		break;
	}
}

void ContactListener::EndContact(b2Contact* contact)
{
	auto fixtureA = contact->GetFixtureA();
	auto fixtureB = contact->GetFixtureB();
	auto collisionType = fixtureA->GetFilterData().categoryBits | fixtureB->GetFilterData().categoryBits;

	switch (collisionType)
	{
	case CollionTypes::PLAYER_FOOT_GROUND:
		PlayerOnAir(fixtureA, fixtureB);
		break;
	default:
		break;
	}
}

void ContactListener::PlayerOnGround(b2Fixture* fixtureA, b2Fixture* fixtureB)
{
	b2Fixture* sensor;
	sensor = fixtureA->GetFilterData().categoryBits == FIXTURE_PLAYER_FOOT ? fixtureA : fixtureB;
	auto player = (Player*)sensor->GetUserData().pointer;
	player->SetJumpingStatus(false);;
	player = nullptr;
}

void ContactListener::PlayerOnAir(b2Fixture* fixtureA, b2Fixture* fixtureB)
{
	b2Fixture* sensor;
	sensor = fixtureA->GetFilterData().categoryBits == FIXTURE_PLAYER_FOOT ? fixtureA : fixtureB;
	auto player = (Player*)sensor->GetUserData().pointer;
	player->SetJumpingStatus(true);
	player = nullptr;
}

GLfloat MyRayCastCallback::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, GLfloat fraction)
{
	m_fixture = fixture;
	m_point = point;
	m_normal = normal;
	m_fraction = fraction;

	// Check if collision with player
	if (fixture->GetFilterData().categoryBits == FIXTURE_PLAYER)
	{
		m_isDetectObject = true;
	}
	else
	{
		m_isDetectObject = false;
	}
	return fraction;
	
}

void ContactListener::PlayerBulletEnemy(b2Fixture* fixtureA, b2Fixture* fixtureB)
{
	auto playerBulletFixture = fixtureA->GetFilterData().categoryBits == FIXTURE_PLAYER_BULLET ? fixtureA : fixtureB;
	auto pBullet = (Bullet*)playerBulletFixture->GetBody()->GetUserData().pointer;
	pBullet->SetActiveStatus(false);

	// TODO: mark enemy as dead
	auto enemyFixture = fixtureA->GetFilterData().categoryBits == FIXTURE_ENEMY ? fixtureA : fixtureB;
	auto pEnenmy = (Enemies*)enemyFixture->GetBody()->GetUserData().pointer;
	if (!pEnenmy->IsDie())
	{
		pEnenmy->TakeDamage(pBullet->GetDamage());
		ResourcesManager::GetInstance()->GetSound(8)->Play();
	}
}

void ContactListener::PlayerBulletGround(b2Fixture* fixtureA, b2Fixture* fixtureB)
{
	auto playerBulletFixture = fixtureA->GetFilterData().categoryBits == FIXTURE_PLAYER_BULLET ? fixtureA : fixtureB;
	auto pBullet = (Bullet*)playerBulletFixture->GetBody()->GetUserData().pointer;
	pBullet->SetActiveStatus(false);
}

void ContactListener::EnemyBulletPlayer(b2Fixture* fixtureA, b2Fixture* fixtureB)
{
	auto enemyBulletFixture = fixtureA->GetFilterData().categoryBits == FIXTURE_ENEMY_BULLET ? fixtureA : fixtureB;
	auto pBullet = (Bullet*)enemyBulletFixture->GetBody()->GetUserData().pointer;
	pBullet->SetActiveStatus(false);

	// TODO: mark player as dead
	auto playerFixture = fixtureA->GetFilterData().categoryBits == FIXTURE_PLAYER ? fixtureA : fixtureB;
	auto pPlayer = (Player*)playerFixture->GetBody()->GetUserData().pointer;
	pPlayer->TakeDamage(pBullet->GetDamage());
	ResourcesManager::GetInstance()->GetSound(7)->Play();
}

void ContactListener::EnemyBulletGround(b2Fixture* fixtureA, b2Fixture* fixtureB)
{
	auto enemyBulletFixture = fixtureA->GetFilterData().categoryBits == FIXTURE_ENEMY_BULLET ? fixtureA : fixtureB;
	auto pBullet = (Bullet*)enemyBulletFixture->GetBody()->GetUserData().pointer;
	pBullet->SetActiveStatus(false);
}

void ContactListener::PlayerBulletBoss(b2Fixture* fixtureA, b2Fixture* fixtureB)
{
	auto playerBulletFixture = fixtureA->GetFilterData().categoryBits == FIXTURE_PLAYER_BULLET ? fixtureA : fixtureB;
	auto pBullet = (Bullet*)playerBulletFixture->GetBody()->GetUserData().pointer;
	//pBullet->m_disabled = true;
	// TODO: mark boss as dead
	auto bossFixture = fixtureA->GetFilterData().categoryBits == FIXTURE_BOSS ? fixtureA : fixtureB;
	auto pBoss = (Boss*)bossFixture->GetBody()->GetUserData().pointer;
	pBoss->TakeDamage(pBullet->GetDamage());

	pBullet->SetBulletLinearVelocity(b2Vec2(0.0f, 0.0f));
}

void ContactListener::BossBulletPlayer(b2Fixture* fixtureA, b2Fixture* fixtureB)
{
	auto bossBulletFixture = fixtureA->GetFilterData().categoryBits == FIXTURE_BOSS_BULLET ? fixtureA : fixtureB;
	auto pBullet = (Boss*)bossBulletFixture->GetBody()->GetUserData().pointer;

	// TODO: mark player as dead
	auto playerFixture = fixtureA->GetFilterData().categoryBits == FIXTURE_PLAYER ? fixtureA : fixtureB;
	auto pPlayer = (Player*)playerFixture->GetBody()->GetUserData().pointer;
	pPlayer->TakeDamage(pBullet->GetDamage());
}

void ContactListener::PLayerLootItem(b2Fixture* fixtureA, b2Fixture* fixtureB)
{
	// Item
	auto item = fixtureA->GetFilterData().categoryBits == FixtureTypes::FIXTURE_ITEM ? fixtureA : fixtureB;
	auto pItem = (Item*)item->GetUserData().pointer;
	pItem->Loot();

	// PLayer
	auto player = fixtureA->GetFilterData().categoryBits == FixtureTypes::FIXTURE_PLAYER ? fixtureA : fixtureB;
	auto pPlayer = (Player*)player->GetBody()->GetUserData().pointer;
	// TODO: get item
	pPlayer->GetItem(pItem->m_typeItem);
}

