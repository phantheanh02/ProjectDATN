#pragma once
#include <GLES2/gl2.h>
#include "../Utilities/Math.h"
#include <memory>
#include "SpriteAnimation.h"
#include <box2d.h>
#include "Globals.h"

enum PlayerDirection
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

struct PlayerDirectionStack
{
	std::vector<PlayerDirection> directionStack;
	PlayerDirectionStack(){};

	void PopDirection()
	{
		directionStack.pop_back();
	};
	bool HasDirection(PlayerDirection action)
	{
		for (int i = 0; i < directionStack.size(); i++)
		{
			if (directionStack[i] == action)
			{
				return true;
			}
		}
		return false;
	}
	void AddDirection(PlayerDirection action)
	{
		if (!HasDirection(action))
		{
			directionStack.push_back(action);
		}
	}
	bool EraseDirection(PlayerDirection action)
	{
		int i;
		for (i = 0; i < directionStack.size(); i++)
		{
			if (directionStack[i] == action)
			{
				directionStack.erase(directionStack.begin() + i);
				return true;
			}
		}
		return false;
	}
	PlayerDirection GetCurrentAction()
	{
		return directionStack.back();
	}
	void Clear()
	{
		directionStack.clear();
	}
};

class Player
{
public:
	Player();
	Player(b2World* world);
	~Player();

	void Update(float deltaTime);
	void Draw();

	void HandleEvent();
	void TakeDamage(GLint damage);

	void SetCurrentDirectionByPreDirection();
	void Set2DPosition(GLint x, GLint y);
	void Set2DPositionByTile(GLfloat x, GLfloat y);
	void Set2DSize(GLint width, GLint height);
	void Set2DSizeByTile(GLfloat width, GLfloat height);
	void SetTileSize(GLint tileSize);
	void SetDirection(PlayerDirection direction);
	void SetAction(PlayerAction action);
	void SetJumpingStatus(bool status);

	PlayerDirection			GetDirection();
	void					GetItem(GLint typeItem);
	inline Vector2			Get2DPositon()		{ return m_actionAnimation->GetPosition(); };
	inline Vector2			GetSize()			{ return m_actionAnimation->GetSize(); };
	inline b2Body*			GetPlayerBody()		{ return m_playerBody; };
	inline PlayerAction		GetCurrentAction()	{ return m_currentAction;};
	inline bool				IsJumping()			{ return m_isJumping; };
	inline bool				IsReadyAttack()		{ return m_bulletCooldown <= 0; };
	inline void				ResetCooldown()		{ m_bulletCooldown = BULLET_COOLDOWN; };
	inline PlayerDirectionStack	GetDirectionStack() { return m_directionStack; }
private:
	Vector2								m_pos;
	Vector2								m_size;

	PlayerDirection						m_currentDirection;
	PlayerDirection						m_preDirection;
	bool								m_isRightDirection;

	PlayerDirectionStack				m_directionStack;
	PlayerAction						m_currentAction;
	std::shared_ptr<SpriteAnimation>	m_actionAnimation;
	bool								m_isJumping;

	// box2d
	b2Body* m_playerBody;
	b2Fixture*							m_playerBodyFixture;
	b2Fixture*							m_playerFootSensorFixture;
	GLfloat								m_bulletCooldown;

private:
	void HandlePlayerDie(GLfloat deltaTime);
};