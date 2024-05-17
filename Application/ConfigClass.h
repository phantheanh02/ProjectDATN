#pragma once
#include <GLES2/gl2.h>
#include "../Utilities/Math.h"
#include "box2d.h"
#include <memory>
#include "Sprite2D.h"

enum TypeBullet
{
	PLAYER_BULLET,
	AR_BULLET,
	RPG_BULLET,
	SNIPER_BULLET,
	ENEMY_FLY_BULLET
};

enum TypeEnemyAction
{
	FIND_PLAYER			= 1,
	RUN					= 17,
	FIRE_DOWN,
	FIRE_UP,
	DIE,
	ARMOD_FIRE,
	RPGMOD_FIRE,
	SNIPERMOD_FIRE,
	ENEMY_01			= 38,
	ENEMY_02			= 39,
	ENEMY_03			= 40,
	ENEMY_EXPLOSION		= 41,
	ACTION_INVALID		= -1
};

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

struct MapInfo
{
	GLint id;
	GLint idTexture;
	GLint minTileSize;
	GLint maxTileSize;
	Vector2 sizeByTile;
	std::vector<Vector4>	plane;		// x, y: position; z, w : size
	std::vector<Vector3>	enemies;	// x: id texture; y, z: position
	std::vector<Vector3>	items;		// x" id texture; y, z: position
};

