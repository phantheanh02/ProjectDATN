#pragma once

#ifndef __ITEM__
#define __ITEM__
#endif

#include <GLES2/gl2.h>
#include <memory>
#include "Sprite2D.h"
#include "box2d.h"
#include "Globals.h"

enum ItemType
{
	HEALING = 0,
	COIN	= 1	,
	ARMOR	= 2
};

class Item
{
public:
	Item(ItemType type, GLfloat posX, GLfloat posY);

	~Item();

	void Update(GLfloat deltaTime);
	void Draw();

	void Set2DSize(GLint w, GLint h);

	inline bool IsLoot() { return m_isLoot; };
	inline void	Loot() { m_isLoot = true; };
	inline ItemType GetType() { return m_type; };

private:
	ItemType	m_type;
	std::shared_ptr<Sprite2D> m_sprite;
	b2Body* m_body;
	Vector2 m_position;
	bool	m_isLoot;
};