#include "stdafx.h"
#include "MapClass.h"
#include "SceneManager.h"

MapClass::MapClass(PlanetType type)
	: m_type(type)
{
	m_planeList.clear();
	m_itemsList.clear();
}

MapClass::~MapClass()
{
}

void MapClass::Init(GLint idTexture, GLint minTileSize, GLint maxTileSize, Vector2 sizeByTile)
{
	m_idTexture		= idTexture;
	m_minTileSize	= minTileSize;
	m_maxTileSize	= maxTileSize;
	m_sizeByTile	= sizeByTile;

	m_background = std::make_shared<Sprite2D>(m_idTexture);
	m_background->Set2DSizeByTile(m_sizeByTile.x, m_sizeByTile.y);
	m_background->Set2DPosition(0, 0);
}

void MapClass::Update(GLfloat deltaTime, b2Vec2 positionPlayer)
{

}

void MapClass::Draw()
{
	m_background->Draw();
}


void MapClass::OnMouseScroll()
{
	m_background->Set2DSizeByTile(m_sizeByTile.x, m_sizeByTile.y);
}
