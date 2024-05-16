#pragma once
#include <GLES2/gl2.h>
#include "../Utilities/Math.h"
#include <memory>
#include "SpriteAnimation.h"
#include <box2d.h>

enum PlayerDirection
{
	TOP,
	BOTTOM,
	LEFT,
	RIGHT,
	NONE = -1
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

	void Set2DPosition(GLint x, GLint y);
	void Set2DPositionByTile(GLfloat x, GLfloat y);
	void Set2DSize(GLint width, GLint height);
	void Set2DSizeByTile(GLfloat width, GLfloat height);
	void SetTileSize(GLint tileSize);
	void SetDirection(PlayerDirection direction);

	void			GetItem(GLint typeItem);
	inline Vector2	Get2DPositon() { return m_pos; };
	inline Vector2	GetSize() {return m_size; };
	inline b2Body*	GetPlayerBody() { return m_playerBody; };

private:
	Vector2								m_pos;
	Vector2								m_size;
	GLint								m_tileSizeByPixel;

	PlayerDirection						m_direction;
	bool								m_isRightDirection;
	std::shared_ptr<SpriteAnimation>	m_actionAnimation;

	// box2d
	b2Fixture*							m_playerBodyFixture;
	b2Body*								m_playerBody;

private:
	void HandlePlayerDie(GLfloat deltaTime);
};