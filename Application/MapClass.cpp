#include "stdafx.h"
#include "MapClass.h"

MapClass::MapClass(PlanetType type)
	: m_type(type)
{
	m_planeList.clear();
	m_enemiesList.clear();
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
}

void MapClass::Update(GLfloat deltaTime)
{
}

void MapClass::Draw()
{
}
