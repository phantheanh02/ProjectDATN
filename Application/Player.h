#pragma once
#include <GLES2/gl2.h>
#include "../Utilities/Math.h"
#include <memory>
#include "SpriteAnimation.h"
#include <box2d.h>

#define HP_MAX 50
#define ARMOR_MAX 100
#define NUMBER_BULLET_MAX 100

enum Action
{
	IDLE = 0x1,
	RUNNING = 0x2,
	JUMPING = 0x4,
	SHOOTING = 0x8,
	ON_AIR = 0x10,
	DEAD = 0x200
};

enum Direction
{
	D_UP = 0x20,
	D_DOWN = 0x40,
	D_LEFT = 0x80,
	D_RIGHT = 0x100
};

class Player
{
public:
	Player();
	Player(b2World* world);
	~Player();

	void Update(float deltaTime);
	void Draw();

	void Set2DPosition(GLint x, GLint y);
	void Set2DSize(GLint width, GLint height);
	void GetItem(GLint typeItem);
	void TakeDamage(GLint damage);
	inline Vector2 Get2DPositon() { return m_pos; };
	Vector2 GetSize();
	inline b2Body* GetPlayerBody(                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            ) { return m_playerBody; };
	inline void SetTileSize(GLint tileSize) { m_tileSizeByPixel = tileSize; };
public:
	int		m_iTopAction, m_iBotAction, m_iActionDirection;
	bool	m_needUpdatePosition, m_needUpdateSize, m_flipped, m_onGround;
	GLint	m_keyItem;
	GLint	m_health;
	GLint	m_armor;
	GLint	m_numberBullet;
	GLint	m_coin;
	bool	m_isPLayerDie;
	bool	m_isLost;
	bool	m_isVictory;

private:
	Vector2 m_pos;
	Vector2 m_size;
	GLfloat m_lastShootTime;
	GLfloat m_timeDie;

	std::shared_ptr<SpriteAnimation> m_topIdleAnimation, m_botRunningAnimation, m_topShootingAnimation, m_DieAnimation;
	std::shared_ptr<Sprite2D> m_botIdleSprite, m_botJumpingSprite, m_topAimUp, m_topAimDown;
	b2Fixture* m_footSensorFixture;
	b2Fixture* m_playerBodyFixture;
	b2Body* m_playerBody;

	std::shared_ptr<Sprite2D> m_hpBar;
	std::shared_ptr<Sprite2D> m_armorBar;
	GLint m_tileSizeByPixel = 44;

private:
	void HandlePlayerDie(GLfloat deltaTime);
};