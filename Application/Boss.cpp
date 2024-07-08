#include "stdafx.h"
#include "Boss.h"
#include "b2Utilities.h"

extern int tileSizeByPixel;
extern std::shared_ptr<b2World> world;

Boss::Boss()
	: m_isNewTexture(false)
	, m_coolDownSkill1(0)
	, m_coolDownSkill2(0)
	, m_coolDownIDLE(0)
	, m_isAttack(false)
	, m_isDie(false)
	, m_isActive(true)
	, m_health(HP_BOSS_MAX)
	, m_bossFixture(nullptr)
	, m_skillBossFixture(nullptr)
	, m_currentDirection(1)
{
	m_animation = std::make_shared<SpriteAnimation>("Striker/spr_StrikerIdle_strip.png", 8, 0.1f);
	m_blood = std::make_shared<SpriteAnimation>(111, 18, 0.02f);

	SetAction(BossActionType::IDLE_BOSS);


	// Body boss
	b2BodyDef bossBodyDef;
	bossBodyDef.userData.pointer = (uintptr_t)this;
	bossBodyDef.type = b2_dynamicBody;
	bossBodyDef.position.Set(150, 30);
	m_body = world->CreateBody(&bossBodyDef);
	CreateBossBox();

	// Body skill boss
	b2BodyDef skillBossBodyDef;
	skillBossBodyDef.userData.pointer = (uintptr_t)this;
	skillBossBodyDef.type = b2_dynamicBody;
	m_skillBossBody = world->CreateBody(&skillBossBodyDef);
	m_skillBossBody->SetGravityScale(0.0f);
	m_skillBossBody->SetEnabled(false);

	// HP bar
	m_hpBar = std::make_shared<Sprite2D>("Icons/hp_status.png");
	
}

Boss::~Boss()
{
	
}

void Boss::Update(float deltaTime, std::shared_ptr<Player> player)
{
	m_animation->Set2DPositionByTile(m_body->GetPosition().x, m_body->GetPosition().y);
	m_hpBar->Set2DPositionByTile((m_body->GetPosition().x - 1), (m_body->GetPosition().y - 1.2f));

	if (m_isDie)
	{
		m_body->SetEnabled(false);
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
		if (m_isTakeDamage)
		{
			m_blood->Update(deltaTime);
			if (m_blood->IsLastFrame())
			{
				m_isTakeDamage = false;
			}
		}

		if (m_isNewTexture)
		{
			//CreateBossBox();
			m_isNewTexture = false;
		}
		//LOG(std::to_string(m_body->GetPosition().x) + " " + std::to_string(m_body->GetPosition().y));
		m_animation->Update(deltaTime);
	}
}

void Boss::Draw()
{
	if (m_isActive)
	{
		m_animation->Draw();
		m_hpBar->Draw();
		if (m_isTakeDamage)
		{
			m_blood->Draw();
		}
	}

}

void Boss::TakeDamage(GLint damage)
{
	m_health -= damage;
	m_isTakeDamage = true;

	m_hpBar->Set2DSizeByTile(2.0f * m_health / HP_BOSS_MAX, 0.1f);
	m_blood->SetCurrentFrame(0);
	m_blood->Set2DSizeByTile(3, 3);
	m_blood->Set2DPositionByTile(m_body->GetPosition().x, m_body->GetPosition().y);

	if (m_health <= 0)
	{
		ResourcesManager::GetInstance()->GetSound(13)->Play();
		Kill();
		return;
	}
	ResourcesManager::GetInstance()->GetSound(8)->Play();
}

void Boss::Kill()
{
	m_isDie = true;

	SetAction(BossActionType::DEATH_BOSS);
}

void Boss::Set2DSizeByTile(GLint width, GLint height)
{
	m_animation->Set2DSizeByTile(width, height);
}

Vector2 Boss::Get2DSize()
{
	return m_animation->GetSize();
}

void Boss::SetAction(BossActionType typeAction)
{
	m_isNewTexture = true;
	m_typeAction = typeAction;
	SetSizeInfo(Vector2(96, 38), Vector2(21, 30));

	switch (typeAction)
	{
	case BossActionType::DASH_BOSS:
		m_animation->SetTexture(ResourcesManager::GetInstance()->GetTexture("Striker/spr_StrikerDash_strip.png"));
		m_animation->SetNumFrame(12);
		m_animation->SetTimeBtwFrame(0.05f);
		break;
	case BossActionType::DEATH_BOSS:
		m_animation->SetTexture(ResourcesManager::GetInstance()->GetTexture("Striker/spr_StrikerDeath_strip.png"));
		m_animation->SetNumFrame(16);
		m_animation->SetTimeBtwFrame(0.1f);
		break;
	case BossActionType::GET_HIT_BOSS:
		m_animation->SetTexture(ResourcesManager::GetInstance()->GetTexture("Striker/spr_StrikerGetHit_strip.png"));
		m_animation->SetNumFrame(4);
		m_animation->SetTimeBtwFrame(0.1f);
		break;
	case BossActionType::IDLE_BOSS:
		m_animation->SetTexture(ResourcesManager::GetInstance()->GetTexture("Striker/spr_StrikerIdle_strip.png"));
		m_animation->SetNumFrame(8);
		m_animation->SetTimeBtwFrame(0.05f);
		break;
	case BossActionType::JUMP_BOSS:
		m_animation->SetTexture(ResourcesManager::GetInstance()->GetTexture("Striker/spr_StrikerJump_strip.png"));
		m_animation->SetNumFrame(12);
		m_animation->SetTimeBtwFrame(0.1f);
		break;
	case BossActionType::RUN_BOSS:
		m_animation->SetTexture(ResourcesManager::GetInstance()->GetTexture("Striker/spr_StrikerRun_strip.png"));
		m_animation->SetNumFrame(8);
		m_animation->SetTimeBtwFrame(0.05f);
		break;
	case BossActionType::SLASH_BOSS:
		m_animation->SetTexture(ResourcesManager::GetInstance()->GetTexture("Striker/spr_StrikerSlash_stripWithEffect.png"));
		m_animation->SetNumFrame(16);
		m_animation->SetTimeBtwFrame(0.1f);
		SetSizeInfo(Vector2(128, 38), Vector2(21, 30));
		break;
	default:
		break;
	}
	m_animation->SetCurrentFrame(0);
	m_animation->Set2DSizeByTile(SCALE_SIZE * m_imgSize.x / m_imgSize.y, SCALE_SIZE);
}

void Boss::OnMouseScroll()
{
}

void Boss::CreateBossBox()
{
	if (m_bossFixture)
	{
		m_body->DestroyFixture(m_bossFixture);
	}
	b2PolygonShape bossShape;
	m_sizeByTile = Vector2(m_boxSize.x / m_imgSize.x, m_boxSize.y / m_imgSize.y) * SCALE_SIZE / 2;
	bossShape.SetAsBox(m_sizeByTile.x, m_sizeByTile.y);;

	b2FixtureDef bossFixtureDef;
	bossFixtureDef.shape = &bossShape;
	bossFixtureDef.density = 1.0f;
	bossFixtureDef.filter.categoryBits = FixtureTypes::FIXTURE_BOSS;
	bossFixtureDef.filter.maskBits = FixtureTypes::FIXTURE_GROUND | FixtureTypes::FIXTURE_PLAYER_BULLET ;
	m_bossFixture = m_body->CreateFixture(&bossFixtureDef);
}


void Boss::SetSizeInfo(Vector2 imgSize, Vector2 boxSize)
{
	m_imgSize = imgSize;
	m_boxSize = boxSize;
	m_sizeByTile = Vector2(boxSize.x / imgSize.x, boxSize.y / imgSize.y) * SCALE_SIZE / 2;
}

void Boss::AttackUpdate(b2Body* playerBody, GLfloat deltaTime)
{
	GLfloat distanceToPlayer = b2Distance(m_body->GetPosition(), playerBody->GetPosition());
	if (distanceToPlayer <= DETECTION_DISTANCE_ATTACK && !m_isAttack)
	{
		m_isAttack = true;
		SetAction(BossActionType::RUN_BOSS);
	}
	if (m_isAttack)
	{
		m_coolDownSkill1 -= deltaTime;
		m_coolDownSkill2 -= deltaTime;
		m_coolDownJump -= deltaTime;

		if (m_typeAction == BossActionType::DASH_BOSS)
		{
			b2Vec2 posBoss = m_body->GetPosition();
			if (m_animation->GetCurrentFrame() == 7)
			{
				m_skillBossBody->SetTransform(b2Vec2(posBoss.x + 0.5f * m_currentDirection, posBoss.y + 0.5f * m_currentDirection), 0.0f);
				Vector2 boxSkill = Vector2(64, 26);
				CreateSkillFixture(boxSkill);
				m_damage = 2;
			}

			if (m_animation->IsLastFrame())
			{
				SetAction(BossActionType::IDLE_BOSS);
				m_skillBossBody->SetEnabled(false);
				m_coolDownIDLE = 0.5f;
			}
		}
		if (m_typeAction == BossActionType::SLASH_BOSS)
		{
			b2Vec2 posBoss = m_body->GetPosition();
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
				SetAction(BossActionType::IDLE_BOSS);
				m_skillBossBody->SetEnabled(false);
				m_coolDownIDLE = 0.5f;
			}
		}
		else if (m_typeAction == BossActionType::IDLE_BOSS && m_coolDownIDLE > 0)
		{
			m_coolDownIDLE -= deltaTime;
			if (m_coolDownIDLE <= 0)
			{
				SetAction(BossActionType::RUN_BOSS);
			}
		}
		else 
		{
			if (distanceToPlayer < DETECTION_DISTANCE_SKILL2 && m_coolDownSkill2 <= 0)
			{
				m_skillBossBody->SetEnabled(true);
				SetAction(BossActionType::SLASH_BOSS);
				m_coolDownSkill2 = 8.0f;
				m_damage = 3;
			}
			else if (distanceToPlayer < DETECTION_DISTANCE_SKILL1 && m_coolDownSkill1 <= 0)
			{
				m_skillBossBody->SetEnabled(true);
				SetAction(BossActionType::DASH_BOSS);
				m_coolDownSkill1 = 1.0f;
				m_damage = 2;
			}

			if (m_typeAction == BossActionType::RUN_BOSS && distanceToPlayer > 1.0f)
			{
				b2Vec2 currentPosition = m_body->GetPosition();
				float newX = currentPosition.x + m_currentDirection * 0.1f;
				//m_body->SetLinearVelocity(b2Vec2(5.0f * m_currentDirection, 0.0f));
				m_body->SetTransform(b2Vec2(newX, currentPosition.y), 0.0f);
				//m_body->ApplyLinearImpulse(b2Vec2(2.0f * m_currentDirection, 0), m_body->GetLocalCenter(), true);
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

	Vector2 sizeBox = Vector2(m_sizeByTile.x * sizeSkillBox.x / m_boxSize.x,
		m_sizeByTile.y * sizeSkillBox.y / m_boxSize.y);

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
