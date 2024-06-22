#include "stdafx.h"
#include "MapClass.h"
#include "SceneManager.h"

MapClass::MapClass(PlanetType type)
	: m_type(type)
{
	m_data = std::make_shared<MapData>();
}

MapClass::~MapClass()
{
}

void MapClass::Init(GLint idTexture, GLint minTileSize, GLint maxTileSize, Vector2 sizeByTile)
{
	m_idTexture		= idTexture;
	m_data->minTileSize = minTileSize;
	m_data->maxTileSize	= maxTileSize;
	m_data->width	= sizeByTile.x;
	m_data->height = sizeByTile.y;

	m_background = std::make_shared<Sprite2D>(m_idTexture);
	m_background->Set2DSizeByTile(m_data->width, m_data->height);
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
	m_background->Set2DSizeByTile(m_data->width, m_data->height);
}
