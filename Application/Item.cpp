#include "stdafx.h"
#include "Item.h"
#include "ResourcesManager.h"
#include "b2Utilities.h"

Item::Item(b2World* world, GLint idTexture, GLfloat posX, GLfloat posY)
{
	m_tileSizeByPixel = tileSizeByPixel;

	auto model = ResourcesManager::GetInstance()->GetModel(ModelType::R_RETANGLE_TOPRIGHT);
	auto shader = ResourcesManager::GetInstance()->GetShader(0);
	auto texture = ResourcesManager::GetInstance()->GetTexture(idTexture);

	m_sprite = std::make_shared<Sprite2D>(0, model, shader, texture);
	m_sprite->Set2DSize(m_tileSizeByPixel, m_tileSizeByPixel);
	m_sprite->Set2DPosition(m_tileSizeByPixel * posX, m_tileSizeByPixel * posY);

	m_isLoot = false;
	m_position = Vector2(posX, posY);
	m_typeItem = idTexture;

	// Box
	b2Body* obsBody;
	b2BodyDef obsBodyDef;
	b2FixtureDef obsFixDef;

	obsBodyDef.position.Set(posX, posY + 0.5f);
	obsBodyDef.type = b2_staticBody;
	obsBody = world->CreateBody(&obsBodyDef);

	// Fixture
	b2PolygonShape sensorShape;
	b2FixtureDef sensorFixtureDef;
	b2FixtureUserData userData;
	sensorShape.SetAsBox(0.5f, 0.5f);
	sensorFixtureDef.shape = &sensorShape;
	sensorFixtureDef.isSensor = true;
	sensorFixtureDef.filter.categoryBits = FixtureTypes::FIXTURE_ITEM;
	sensorFixtureDef.filter.maskBits = FixtureTypes::FIXTURE_PLAYER;
	sensorFixtureDef.userData.pointer = (uintptr_t)this;
	obsBody->CreateFixture(&sensorFixtureDef);

	m_body = obsBody;
}

Item::~Item()
{
}

void Item::Update(GLfloat deltaTime)
{
	if (m_isLoot)
	{
		m_body->SetEnabled(false);
	}
	else
	{
		m_sprite->Set2DPosition(m_tileSizeByPixel * m_position.x, m_tileSizeByPixel * m_position.y);
	}
}

void Item::Draw()
{
	if (!m_isLoot)
	{
		m_sprite->Draw();
	}
}

void Item::Set2DSize(GLint w, GLint h)
{
	m_sprite->Set2DSize(w, h);
	m_tileSizeByPixel = w;
	m_sprite->Set2DPosition(m_tileSizeByPixel * m_position.x, m_tileSizeByPixel * m_position.y);

}
