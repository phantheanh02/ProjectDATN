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
#include "Text.h"
#include "Item.h"
#include "Bullet.h"

class Bullet;
class Sprite2D;
class Item;

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

struct CharacterStats
{
	CharacterType type;
	GLint hp, spd, atk, numberBullet;
	CharacterStats() : hp(0), spd(0), atk(0), numberBullet(0) {};
	CharacterStats(GLint _hp, GLint _spd, GLint _atk)
		: hp(_hp), spd(_spd), atk(_atk) {};

};

class Player
{
public:
	Player();
	~Player();

	void Update(float deltaTime);
	void Draw();

	void HandleEvent(int event);
	void TakeDamage(GLint damage);
	void ReCalculateWhenScroll();

	void SetCurrentDirectionByPreDirection();
	void Set2DPositionByTile(GLfloat x, GLfloat y);
	void Set2DSizeByTile(GLfloat width, GLfloat height);
	void SetDirection(DirectionType direction);
	void SetAction(PlayerAction action);
	void SetJumpingStatus(bool status);
	void SetCharacter(CharacterType type);
	void SetLoadingBullet(GLfloat time);
	inline void SetCoin(GLint number) { m_coin = number; };
	inline void SetArmor(GLint number) { m_armor = number; };
	inline void AddCoin(GLint number) { m_coin += number; };

	void					GetItem(ItemType typeItem);
	inline b2Body*			GetBody()			{ return m_body; };
	inline GLint			GetNumberBullet() { return m_numberBullet; };
	bool					IsReadyJump();
	inline bool				IsReadyToReset()	{ return m_resetAfterDieTime <= 0; }
	inline bool				IsDie()				{ return m_health <= 0; };
	inline bool				IsLoadingBullet() { return m_isLoadingBullet > 0; };
	inline GLint			GetCoin() { return m_coin; };
	inline GLint			GetArmor() { return m_armor; };

	GLint								m_contacCount;
private:
	// attribute
	Vector2								m_pos;
	Vector2								m_size;
	GLint								m_health;
	GLint								m_numberBullet;
	CharacterStats						m_stats;

	// HUB
	std::shared_ptr <Sprite2D>			m_HPBar;

	// Hanlde action
	GLint								m_playerbulletType;
	DirectionType						m_currentDirection;
	DirectionType						m_sprinningDirection;
	PlayerAction						m_currentAction;
	std::shared_ptr<SpriteAnimation>	m_actionAnimation;
	bool								m_isJumping;
	bool								m_isDie;
	GLfloat								m_resetAfterDieTime;
	CharacterType						m_currentCharacter;
	GLfloat								m_jumpCooldown;
	bool								m_isTakeDamage;

	// Bullet
	std::vector<std::shared_ptr<Bullet>>	m_bulletList;
	GLfloat								m_isLoadingBullet;

	// box2d
	b2Body*								m_body;
	GLfloat								m_bulletCooldown;

	// Effect
	std::shared_ptr<SpriteAnimation>	m_blood;

	// Item
	GLint		m_coin;
	GLint		m_armor;

private:
	void HandlePlayerDie(GLfloat deltaTime);
	void CreateBullet(b2Vec2 speed);

};