#pragma once

#ifndef __ENEMY__
#define __ENEMY__
#endif

#include <GLES2/gl2.h>
#include "../Utilities/Math.h"
#include <memory>
#include "SpriteAnimation.h"
#include <unordered_map>
#include "box2d.h"
#include "b2Utilities.h"
#include "Sprite2D.h"
#include "Player.h"
#include "Bullet.h"

enum EnemyAction
{
	E_IDLE = 0,
	E_RUN,
	E_ATTACK,
	E_DIE,
	E_NONE = -1
};

enum EnemyType
{
	AR_MOD = 0,
	RPG_MOD,
	Sniper_MOD,
	PATREON,
	MEGAMAN,
	YUME
};

class Enemies
{
public:
	Enemies(EnemyType type, Vector2 sizeImg, Vector2 sizeBox, GLint hp, GLint damage);
	~Enemies();

	void Update(float deltaTime, b2Vec2 positionPlayer);
	void Draw();
	void Move();
	void Init(GLfloat posX, GLfloat posY);
	void OnMouseScroll();

	void CreateEnemyBox(std::shared_ptr<b2World> world, float x, float y);
	void CalculateSize();
	void TakeDamage(GLint damage);
	std::shared_ptr<Enemies> Clone();

	void Set2DPosition(GLint x, GLint y);
	void Set2DPositionByTile(GLfloat x, GLfloat y);
	void Set2DSize(GLint width, GLint height);
	void Set2DSizeByTile(GLfloat x, GLfloat y);
	void SetTexture(GLint typeAction);
	void SetActive(bool isActive);
	void SetAction(EnemyAction action);
	void SetAttack(bool isAttack);

	Vector2								Get2DSize();
	std::shared_ptr<SpriteAnimation>	GetAnimation() { return m_animation; };
	b2Body*								GetBody();
	inline DirectionType				GetCurrentDirection() { return m_currentDirection; }
	inline DirectionType				GetSprinningDirection() { return m_sprinningDirection; }
	inline EnemyType					GetType() { return m_type; };
	inline BulletType					GetEnemyBulletType() { return m_enemyBulletType; };
	inline GLint						GetDamage() { return m_damage;	};
	inline bool							IsActive() { return m_isActive; };
	inline bool							IsDie() { return m_isDie; };
	inline bool							IsReadyAttack() { return m_isReadyAttack; };

private:
	// Attribute
	GLint			m_id; // ID texture
	EnemyType		m_type;
	EnemyAction		m_currentAction;
	GLint			m_health;
	GLint			m_damage;
	b2Vec2			m_speed;
	GLfloat			m_coolDown;
	GLfloat			m_timeFinding;

	DirectionType	m_currentDirection;
	Vector2			m_imgSize;
	Vector2			m_boxSize;
	Vector2			m_sizeByTile;

	DirectionType	m_sprinningDirection;
	BulletType		m_enemyBulletType;
	bool			m_isTakeDamage;

	// Sprite
	std::shared_ptr<SpriteAnimation> m_animation;
	std::shared_ptr<Sprite2D> m_hpBar;
	GLfloat		m_timeBarDisplay;

	// Status
	bool		m_isActive;
	bool		m_isDie;
	bool		m_isChange;
	bool		m_isFire;
	bool		m_isReadyAttack;

	// box2d
	b2Body*		m_body;
	MyRayCastCallback m_rayCallback;

	// Effect
	std::shared_ptr<SpriteAnimation>	m_blood;
	
	// Bullet
	std::vector<std::shared_ptr<Bullet>>	m_bulletList;

private:
	void		PerformRayCasting(b2Vec2 positionPlayer);
	void		RunModUpdate(std::shared_ptr<Player> player);
	void		FindPlayerUpdate(std::shared_ptr<Player> player);
	void		AttackUpdate(std::shared_ptr<Player> player);
	void		EnemyFlyUpdate(std::shared_ptr<Player> player);
	void		CreateBullet(b2Vec2 speed);

};