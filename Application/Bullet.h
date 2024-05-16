#pragma once
#include <memory>
#include "../Utilities/Math.h"
#include <box2d.h>
#include "Sprite2D.h"
#include "b2Utilities.h"
#include "ConfigClass.h"
#include "SceneManager.h"
#include "Globals.h"

class Bullet
{
public:
	Bullet(b2World* world, FixtureTypes bulletType);
	Bullet(b2World* world);
	~Bullet();
	inline b2Body* GetBulletBody() { return m_bulletBody; };
	void Set2DPosition(float x, float y);
	void Set2DSize(int x, int y);
	void Update(float deltaTime);
	void SetBulletLinearVelocity(b2Vec2 speed);
	void Draw();
	void CreateNewFixture(FixtureTypes bulletType);
	void CreateNewBullet(TypeBullet typeBullet, FixtureTypes typeFixture, b2Vec2 speed, GLfloat posX, GLfloat posY, GLint currentDirection);
	void Set2DSizeScroll();
	void SetTileSize(GLint tileSize);
	inline std::shared_ptr<SpriteAnimation> GetAnimation() { return m_bulletAnimation; };
	inline std::shared_ptr<Sprite2D> GetPrite2D() { return m_bulletSprite; };
	inline void SetDamage(GLint damage) { m_damage = damage; };
	inline GLint GetDamage() { return m_damage; };
public:
	bool		m_disabled;
	bool		m_isCollision;
	BoxBullet	boxBullet;
	TypeBullet	m_typeBullet;
private:
	Vector2		m_position;
	GLint		m_damage;
	b2Body*		m_bulletBody;
	b2Fixture*	m_bulletFixture;
	std::shared_ptr<Sprite2D> m_bulletSprite;
	std::shared_ptr<SpriteAnimation> m_bulletAnimation;
	GLint		m_currentDirection;
	Vector2		size2D;
	GLint		m_tileSizeByPixel = tileSizeByPixel;
};
