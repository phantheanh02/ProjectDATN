#pragma once

#ifndef __BULLET__
#define __BULLET__
#endif

#include <cstdint>
#include <memory>
#include "../Utilities/Math.h"
#include <box2d.h>
#include "b2Utilities.h"

enum BulletType
{
	PLAYER_BULLET = 0 ,
	BLACK_BULLET,
	BLUE_BULLET,
	GREEN_BULLET,
	RED_BULLET,
	YELLOW_BULLET,
	AR_BULLET,
	RPG_BULLET,
	SNIPER_BULLET,
	PATREON_BULLET,
	MEGAMAN_BULLET,
	YUME_BULLET,
	NONE_BULLET = -1
};

struct BoxBullet
{
	GLint id;
	Vector2 imgSize;
};

class Bullet
{
public:
	Bullet(b2World* world);
	~Bullet();

	void Update(float deltaTime);
	void Draw();

	void CreateNewBullet(BulletType type, b2Vec2 speed, Vector2 position, int damage = 1);
	void OnMouseScroll();
	void SetBulletLinearVelocity(b2Vec2 speed);
	void Set2DPosition(float x, float y);
	void Set2DPositionByTile(GLfloat x, GLfloat y);
	void Set2DSize(int x, int y);
	void SetDamage(GLint damage);
	void SetActiveStatus(bool status);

	inline std::shared_ptr<SpriteAnimation>	GetAnimation() { return m_animation; };
	inline GLint							GetDamage() { return m_damage; };
	inline b2Body*							GetBody() { return m_body; };
	inline bool								IsActive() { return m_isActive; };
private:
	GLint		m_damage;
	GLint		m_currentDirection;
	Vector2		m_size2D;
	BulletType	m_typeBullet;
	std::shared_ptr<SpriteAnimation> m_animation;

	b2Body*		m_body;
	b2Fixture*	m_bulletFixture;

	bool		m_isActive;
	
};
