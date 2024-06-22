#include "stdafx.h"
#include "Item.h"
#include "ResourcesManager.h"
#include "b2Utilities.h"
#include "SceneManager.h"

extern int tileSizeByPixel;
extern std::shared_ptr<b2World> world;
 
Item::Item(ItemType type, GLfloat posX, GLfloat posY):
	  m_type(type)
	, m_isLoot(false)

{
	switch (type)
	{
	case HEALING:
		m_sprite = std::make_shared<Sprite2D>("Item/item_hp.png");
		break;
	case COIN:
		m_sprite = std::make_shared<Sprite2D>("Item/item_coin.png");
		break;
	case ARMOR:
		m_sprite = std::make_shared<Sprite2D>("Item/item_armor.png");
		break;
	default:
		break;
	}
	m_sprite->Set2DSizeByTile(1, 1);
	m_sprite->Set2DPositionByTile(posX, posY);
	m_sprite->AttachCamera(SceneManager::GetInstance()->GetCamera(CameraType::DYNAMIC_CAMERA));
	m_position = Vector2(posX, posY);

	// Box
	b2BodyDef obsBodyDef;
	b2FixtureDef obsFixDef;

	obsBodyDef.position.Set(posX, posY + 0.5f);
	obsBodyDef.type = b2_staticBody;
	m_body = world->CreateBody(&obsBodyDef);

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
	m_body->CreateFixture(&sensorFixtureDef);
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
		m_sprite->Set2DPositionByTile(m_position.x, m_position.y);
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
	printf("Excute from Item class\n");
}
