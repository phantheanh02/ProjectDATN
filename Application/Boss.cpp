#include "stdafx.h"
#include "Boss.h"
#include "b2Utilities.h"

extern int tileSizeByPixel;

Boss::Boss(b2World* world)
{
	m_bossBody			= nullptr;
	m_bossFixture		= nullptr;
	m_skillBossFixture	= nullptr;
	m_isNewTexture		= false;
	m_coolDownSkill1	= 0.0f;
	m_coolDownSkill2	= 0.0f;
	m_coolDownIDLE		= 0.0f;
	m_isAttack			= false;
	m_isDie				= false;
	m_isActive			= true;
	m_currentDirection	= 1.0f;
	m_health			= 15.0f;
	m_damage			= 0;
	m_timeBarDisplay	= 0.0f;
	auto model = ResourcesManager::GetInstance()->GetModel(ModelType::R_RETANGLE_TOPRIGHT);
	auto shader = ResourcesManager::GetInstance()->GetShader(1);
	auto texture = ResourcesManager::GetInstance()->GetTexture(34);
	m_animation = std::make_shared<SpriteAnimation>(0, model, shader, texture, 8, 0.1f);

	SetTexture(TypeBossAction::IDLE_BOSS);

	// Body boss
	b2BodyDef bossBodyDef;
	bossBodyDef.userData.pointer = (uintptr_t)this;
	bossBodyDef.type = b2_dynamicBody;
	bossBodyDef.position.Set(42, 28);
	m_bossBody = world->CreateBody(&bossBodyDef);
	CreateBossBox(world);

	// Body skill boss
	b2BodyDef skillBossBodyDef;
	skillBossBodyDef.userData.pointer = (uintptr_t)this;
	skillBossBodyDef.type = b2_dynamicBody;
	m_skillBossBody = world->CreateBody(&skillBossBodyDef);
	m_skillBossBody->SetGravityScale(0.0f);
	m_skillBossBody->SetEnabled(false);

	// HP bar
	shader = ResourcesManager::GetInstance()->GetShader(0);
	texture = ResourcesManager::GetInstance()->GetTexture(57);
	m_hpBar = std::make_shared<Sprite2D>(0, model, shader, texture);
}

Boss::~Boss()
{
	
}

void Boss::Update(float deltaTime, std::shared_ptr<Player> player)
{
	if (m_timeBarDisplay > 0)
	{
		m_hpBar->Set2DSize(2 * m_tileSizeByPixel * m_health / HP_BOSS_MAX, 5);
		m_hpBar->Set2DPosition((m_bossBody->GetPosition().x - 1) * m_tileSizeByPixel,
			(m_bossBody->GetPosition().y - 1) * m_tileSizeByPixel);

		m_timeBarDisplay -= deltaTime;
	}
	b2Vec2 bossPosition = m_bossBody->GetPosition();
	this->Set2DPositionFromBox2D(bossPosition.x, bossPosition.y);
	if (m_isDie)
	{
		m_bossBody->SetEnabled(false);
		if (!m_animation->IsLastFrame())
		{
			m_animation->Update(deltaTime);
		}
		else
		{
			m_isActive = false;
		}
	}
	else
	{
		AttackUpdate(player->GetBody(), deltaTime);

		if (m_isNewTexture)
		{
			CreateBossBox(player->GetBody()->GetWorld());
			m_isNewTexture = false;
		}
		m_animation->Update(deltaTime);
	}
}

void Boss::Draw()
{
	if (m_isActive)
	{
		m_animation->Draw();
		if (m_timeBarDisplay > 0)
		{
			m_hpBar->Draw();
		}
	}

}

void Boss::TakeDamage(GLint damage)
{
	m_health -= damage;
	if (m_health <= 0)
	{
		Kill();
	}
	m_timeBarDisplay = 2.0f;
}

void Boss::Kill()
{
	m_isDie = true;

	this->SetTexture(TypeBossAction::DEATH_BOSS);
}

void Boss::Set2DPosition(GLint x, GLint y)
{
	m_animation->Set2DPosition(x, y);
}

void Boss::Set2DSize(GLint width, GLint height)
{
	m_animation->Set2DSize(width, height);
}

void Boss::Set2DSizeByTile()
{
	this->Set2DSize(m_imgSize.x * m_tileSizeByPixel, m_imgSize.y * m_tileSizeByPixel);
}

Vector2 Boss::Get2DSize()
{
	return m_animation->GetSize();
}

void Boss::SetTexture(GLint typeAction)
{
	m_isNewTexture = true;
	m_typeAction = typeAction;
	m_animation->SetTexture(ResourcesManager::GetInstance()->GetTexture(typeAction));
	SetBoxInfo(Vector2(96, 38), Vector2(21, 30));

	switch (typeAction)
	{
	case TypeBossAction::DASH_BOSS:
		m_animation->SetNumFrame(12);
		m_animation->SetCurrentFrame(0);
		m_animation->SetTimeBtwFrame(0.05f);
		break;
	case TypeBossAction::DEATH_BOSS:
		m_animation->SetNumFrame(16);
		m_animation->SetCurrentFrame(0);
		m_animation->SetTimeBtwFrame(0.1f);
		break;
	case TypeBossAction::GET_HIT_BOSS:
		m_animation->SetNumFrame(4);
		m_animation->SetCurrentFrame(0);
		m_animation->SetTimeBtwFrame(0.1f);
		break;
	case TypeBossAction::IDLE_BOSS:
		m_animation->SetNumFrame(8);
		m_animation->SetCurrentFrame(0);
		m_animation->SetTimeBtwFrame(0.05f);
		break;
	case TypeBossAction::JUMP_BOSS:
		m_animation->SetNumFrame(12);
		m_animation->SetCurrentFrame(0);
		m_animation->SetTimeBtwFrame(0.1f);
		break;
	case TypeBossAction::RUN_BOSS:
		m_animation->SetNumFrame(8);
		m_animation->SetCurrentFrame(0);
		m_animation->SetTimeBtwFrame(0.05f);
		break;
	case TypeBossAction::SLASH_BOSS:
		m_animation->SetNumFrame(16);
		m_animation->SetCurrentFrame(0);
		m_animation->SetTimeBtwFrame(0.1f);
		SetBoxInfo(Vector2(128, 38), Vector2(21, 30));
		break;
	default:
		break;
	}
}


void Boss::Set2DPositionFromBox2D(GLfloat x, GLfloat y)
{
	float posX = x * m_tileSizeByPixel - Get2DSize().x / 2.0f;
	float posY = y * m_tileSizeByPixel - (Get2DSize().y - m_boxSize.y * m_tileSizeByPixel);
	m_animation->Set2DPosition(posX, posY);
}

void Boss::SetTileSizeScroll(GLint tileSize)
{
	m_tileSizeByPixel = tileSize;
	Set2DSizeByTile();
}

void Boss::CreateBossBox(b2World* world)
{
	this->DestroyFixture(m_bossFixture);
	b2PolygonShape bossShape;
	bossShape.SetAsBox(m_boxSize.x, m_boxSize.y);

	b2FixtureDef bossFixtureDef;
	bossFixtureDef.shape = &bossShape;
	bossFixtureDef.density = 1.0f;
	bossFixtureDef.friction = 0.5f;
	bossFixtureDef.filter.categoryBits = FixtureTypes::FIXTURE_BOSS;
	bossFixtureDef.filter.maskBits = FixtureTypes::FIXTURE_GROUND | FixtureTypes::FIXTURE_PLAYER_BULLET ;
	m_bossFixture = m_bossBody->CreateFixture(&bossFixtureDef);


	this->Set2DSize(m_imgSize.x * m_tileSizeByPixel, m_imgSize.y * m_tileSizeByPixel);
}


void Boss::SetBoxInfo(Vector2 imgSize, Vector2 boxSize)
{
}

void Boss::AttackUpdate(b2Body* playerBody, GLfloat deltaTime)
{
	GLfloat distanceToPlayer = b2Distance(m_bossBody->GetPosition(), playerBody->GetPosition());
	if (distanceToPlayer <= DETECTION_DISTANCE_ATTACK && !m_isAttack)
	{
		m_isAttack = true;
		SetTexture(TypeBossAction::RUN_BOSS);
	}
	if (m_isAttack)
	{
		m_coolDownSkill1 -= deltaTime;
		m_coolDownSkill2 -= deltaTime;
		m_coolDownJump -= deltaTime;

		if (m_typeAction == TypeBossAction::DASH_BOSS)
		{
			b2Vec2 posBoss = m_bossBody->GetPosition();
			if (m_animation->GetCurrentFrame() == 7)
			{
				m_skillBossBody->SetTransform(b2Vec2(posBoss.x + 0.5f * m_currentDirection, posBoss.y + 0.5f * m_currentDirection), 0.0f);
				Vector2 boxSkill = Vector2(64, 26);
				CreateSkillFixture(boxSkill);
				m_damage = 2;
			}

			if (m_animation->IsLastFrame())
			{
				SetTexture(TypeBossAction::IDLE_BOSS);
				m_skillBossBody->SetEnabled(false);
				m_coolDownIDLE = 0.5f;
			}
		}
		if (m_typeAction == TypeBossAction::SLASH_BOSS)
		{
			b2Vec2 posBoss = m_bossBody->GetPosition();
			if (m_animation->GetCurrentFrame() == 2)
			{ 
				m_skillBossBody->SetTransform(b2Vec2(posBoss.x + 0.5f * m_currentDirection, posBoss.y + 0.5f * m_currentDirection), 0.0f);
				Vector2 boxSkill = Vector2(64, 26);
				CreateSkillFixture(boxSkill);
				m_damage = 2;
			}

			if (m_animation->GetCurrentFrame() == 13)
			{
				m_skillBossBody->SetTransform(b2Vec2(posBoss.x + 3.0f * m_currentDirection, posBoss.y + 0.5f * m_currentDirection), 0.0f);
				Vector2 boxSkill = Vector2(40, 5);
				CreateSkillFixture(boxSkill);
				m_damage = 3;
			}

			if (m_animation->IsLastFrame())
			{
				SetTexture(TypeBossAction::IDLE_BOSS);
				m_skillBossBody->SetEnabled(false);
				m_coolDownIDLE = 0.5f;
			}
		}
		else if (m_typeAction == TypeBossAction::IDLE_BOSS && m_coolDownIDLE > 0)
		{
			m_coolDownIDLE -= deltaTime;
			if (m_coolDownIDLE <= 0)
			{
				SetTexture(TypeBossAction::RUN_BOSS);
			}
		}
		else 
		{
			if (distanceToPlayer < DETECTION_DISTANCE_SKILL2 && m_coolDownSkill2 <= 0)
			{
				m_skillBossBody->SetEnabled(true);
				SetTexture(TypeBossAction::SLASH_BOSS);
				m_coolDownSkill2 = 8.0f;
				m_damage = 3;
			}
			else if (distanceToPlayer < DETECTION_DISTANCE_SKILL1 && m_coolDownSkill1 <= 0)
			{
				m_skillBossBody->SetEnabled(true);
				SetTexture(TypeBossAction::DASH_BOSS);
				m_coolDownSkill1 = 1.0f;
				m_damage = 2;
			}

			if (m_typeAction == TypeBossAction::RUN_BOSS && distanceToPlayer > 1.0f)
			{
				b2Vec2 currentPosition = m_bossBody->GetPosition();
				float newX = currentPosition.x + m_currentDirection * 0.04f;
				m_bossBody->SetLinearVelocity(b2Vec2(5.0f * m_currentDirection, 0.0f));
				//m_bossBody->SetTransform(b2Vec2(newX, currentPosition.y), 0.0f);

				GLint newCurrentDirection = newX < playerBody->GetPosition().x ? 1 : -1;
				if (newCurrentDirection != m_currentDirection) {
					m_currentDirection = -m_currentDirection;
					m_animation->FlipVertical();
				}
			}
		}
	}
}

void Boss::CreateSkillFixture(Vector2 sizeSkillBox)
{
	// Fixture
	if (m_skillBossFixture != nullptr)
	{
		m_skillBossBody->DestroyFixture(m_skillBossFixture);
	}

	Vector2 sizeBox = Vector2();

	b2PolygonShape sensorShape;
	b2FixtureDef sensorFixtureDef;
	b2FixtureUserData userData;
	sensorShape.SetAsBox(sizeBox.x, sizeBox.y);
	sensorFixtureDef.shape = &sensorShape;
	sensorFixtureDef.isSensor = true;
	sensorFixtureDef.filter.categoryBits = FixtureTypes::FIXTURE_BOSS_BULLET;
	sensorFixtureDef.filter.maskBits = FixtureTypes::FIXTURE_PLAYER;
	m_skillBossFixture = m_skillBossBody->CreateFixture(&sensorFixtureDef);

}

void Boss::DestroyFixture(b2Fixture* fixture)
{
	if (fixture != nullptr)
	{
		m_bossBody->DestroyFixture(fixture);
	}

}
