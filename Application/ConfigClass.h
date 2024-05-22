#pragma once
#include <GLES2/gl2.h>
#include "../Utilities/Math.h"
#include "box2d.h"
#include <memory>
#include "Sprite2D.h"


enum TypeBossAction
{
	DASH_BOSS = 31,
	DEATH_BOSS,
	GET_HIT_BOSS,
	IDLE_BOSS,
	JUMP_BOSS,
	RUN_BOSS,
	SLASH_BOSS
};

enum TypeItem
{
	HEALING_ITEM = 48,
	KEY_ITEM = 49,
	CUP_ITEM = 80
};

struct ObsDynamic
{
	std::shared_ptr<Sprite2D> sprite;
	b2Body* body;
	b2Vec2 sizeBox;
	GLfloat top, down;
	Vector2 position;
	GLfloat height;
	bool* isLock;
};

struct BoxEnemy
{
	GLint id;
	Vector2 boxSize, imgSize;
};

struct BoxBullet
{
	GLint id;
	Vector2 imgSize;
};



