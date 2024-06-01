#pragma once

#ifndef __PLAYER__
#define __PLAYER__
#endif

#include <GLES2/gl2.h>
#include "../Utilities/Math.h"
#include <memory>
#include "SpriteAnimation.h"
#include <box2d.h>
#include "Globals.h"
#include "Bullet.h"

enum DirectionType
{
	TOP,
	BOTTOM,
	LEFT,
	RIGHT,
	NONE_DIRECT = -1
};

enum PlayerAction
{
	IDLE		= 0,
	RUNNING		= 1,
	JUMPING		= 2,
	CROUCH		= 3,
	DEAD		= 4,
	FIRE_TOP	= 5,
	FIRE_BOTTOM = 6,
	NONE_ACTION	= -1
};

class Player
{
public:
	Player();
	~Player();

	void Update(float deltaTime);
	void Draw();

	void HandleEvent();
	void TakeDamage(GLint damage);
	void ReCalculateWhenScroll();

	void SetCurrentDirectionByPreDirection();
	void Set2DPosition(GLint x, GLint y);
	void Set2DPositionByTile(GLfloat x, GLfloat y);
	void Set2DSize(GLint width, GLint height);
	void Set2DSizeByTile(GLfloat width, GLfloat height);
	void SetTileSize(GLint tileSize);
	void SetDirection(DirectionType direction);
	void SetAction(PlayerAction action);
	void SetJumpingStatus(bool status);

	DirectionType			GetDirection();
	void					GetItem(GLint typeItem);
	inline Vector2			Get2DPositon()		{ return m_actionAnimation->GetPosition(); };
	inline Vector2			GetSize()			{ return m_actionAnimation->GetSize(); };
	inline b2Body*			GetBody()			{ return m_body; };
	inline PlayerAction		GetCurrentAction()	{ return m_currentAction;};
	inline DirectionType	GetSprinningDirection() { return m_sprinningDirection; };
	inline GLint			GetPlayerBulletType() { return m_playerbulletType; };
	inline bool				IsJumping()			{ return m_isJumping; };
	inline bool				IsReadyAttack()		{ return m_bulletCooldown <= 0; };
	inline void				ResetCooldown()		{ m_bulletCooldown = BULLET_COOLDOWN; };
	inline bool				IsReadyToReset()	{ return m_resetAfterDieTime <= 0; }
	inline bool				IsDie()				{ return m_health <= 0; };
private:
	Vector2								m_pos;
	Vector2								m_size;
	GLint								m_health;
	GLint								m_playerbulletType;
	DirectionType						m_currentDirection;
	DirectionType						m_sprinningDirection;
	PlayerAction						m_currentAction;
	std::shared_ptr<SpriteAnimation>	m_actionAnimation;
	bool								m_isJumping;
	bool								m_isDie;
	GLfloat								m_resetAfterDieTime;

	// box2d
	b2Body*								m_body;
	GLfloat								m_bulletCooldown;

private:
	void HandlePlayerDie(GLfloat deltaTime);
};