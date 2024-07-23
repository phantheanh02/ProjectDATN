#include "stdafx.h"
#include "Enemies.h"
#include "ResourcesManager.h"
#include "GSPlay.h"
#include "Globals.h"

extern int tileSizeByPixel;
extern std::shared_ptr<b2World> world;

Enemies::Enemies(EnemyType type, Vector2 sizeImg, Vector2 sizeBox, GLint hp, GLint damage)
	: m_type(type)
	, m_currentAction(E_IDLE)
	, m_imgSize(sizeImg)
	, m_boxSize(sizeBox)
	, m_isActive(false)
	, m_sprinningDirection(LEFT)
	, m_coolDown(0)
	, m_isReadyAttack(false)
	, m_timeFinding(TIME_FINDING)
	, m_isTakeDamage(false)
	, m_health(hp)
	, m_damage(damage)
	, m_isGetCoin(false)
	, m_contacCount(0)
{
	m_blood = std::make_shared<SpriteAnimation>(111, 18, 0.02f);
}

Enemies::~Enemies()
{
}

void Enemies::Init(GLfloat posX, GLfloat posY)
{
	m_isActive = true;
	m_isDie = false;

	// create body
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(posX, posY);
	bodyDef.userData.pointer = (uintptr_t)this;
	m_body = world->CreateBody(&bodyDef);

	// set body properties
	auto size = Vector2(m_boxSize.x / m_imgSize.x, m_boxSize.y / m_imgSize.y) * SCALE_SIZE / 2;
	b2PolygonShape shape;
	shape.SetAsBox(size.x, size.y); // size box = size img / 2
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shape;
	fixtureDef.density = 1.0f;
	fixtureDef.filter.categoryBits = FixtureTypes::FIXTURE_ENEMY;
	fixtureDef.filter.maskBits = FixtureTypes::FIXTURE_GROUND | FixtureTypes::FIXTURE_PLAYER_BULLET;
	fixtureDef.userData.pointer = (uintptr_t)this;
	m_body->CreateFixture(&fixtureDef);
	m_body->SetFixedRotation(true);

	// Create sensor foot to check in ground
	//b2PolygonShape sensorBox;
	//b2FixtureUserData userData;
	//b2FixtureDef sensorDef;
	//sensorBox.SetAsBox(0.1f, 0.1f, b2Vec2(0, 1), 0); // sensor dưới chân
	//sensorDef.shape = &sensorBox;
	//sensorDef.isSensor = true;
	//sensorDef.filter.categoryBits = FixtureTypes::FIXTURE_PLAYER_FOOT;
	//sensorDef.filter.maskBits = FixtureTypes::FIXTURE_GROUND;
	//sensorDef.userData.pointer = (uintptr_t)this;
	//m_body->CreateFixture(&sensorDef);

	switch (m_type)
	{
	case AR_MOD:
		m_animation = std::make_shared<SpriteAnimation>("Enemy/ARMod_Idle.png", 8, 0.1f);
		m_enemyBulletType = BulletType::AR_BULLET;
		break;
	case RPG_MOD:
		m_animation = std::make_shared<SpriteAnimation>("Enemy/ARMod_Idle.png", 8, 0.1f);
		m_enemyBulletType = BulletType::RPG_BULLET;
		break;
	case Sniper_MOD:
		m_animation = std::make_shared<SpriteAnimation>("Enemy/ARMod_Idle.png", 8, 0.1f);
		m_enemyBulletType = BulletType::SNIPER_BULLET;
		break;
	case PATREON:
		m_animation = std::make_shared<SpriteAnimation>("Enemy/Patreon.png", 5, 0.1f);
		m_body->SetGravityScale(0);
		m_enemyBulletType = BulletType::PATREON_BULLET;
		break;
	case MEGAMAN:
		m_animation = std::make_shared<SpriteAnimation>("Enemy/Megaman.png", 4, 0.1f);
		m_body->SetGravityScale(0);
		m_enemyBulletType = BulletType::MEGAMAN_BULLET;
		break;
	case YUME:
		m_animation = std::make_shared<SpriteAnimation>("Enemy/Yume.png", 4, 0.1f);
		m_body->SetGravityScale(0);
		m_enemyBulletType = BulletType::YUME_BULLET;
		break;
	default:
		break;
	}
	m_animation->Set2DSizeByTile(SCALE_SIZE, SCALE_SIZE);
	m_sizeByTile = Vector2(SCALE_SIZE, SCALE_SIZE);

	// Create bullet pooling
	for (int i = 0; i < 20; i++)
	{
		m_bulletList.push_back(std::make_shared<Bullet>(world.get()));
	}
}

void Enemies::OnMouseScroll()
{
	auto pos = m_body->GetPosition();
	m_animation->Set2DPositionByTile(pos.x, pos.y);
	m_animation->Set2DSizeByTile(m_sizeByTile.x, m_sizeByTile.y);
	for (auto bullet : m_bulletList)
	{
		if (bullet->IsActive())
		{
			bullet->OnMouseScroll();
		}
	}
}

void Enemies::Update(float deltaTime, b2Vec2 positionPlayer)
{
	for (auto it : m_bulletList)
	{
		it->Update(deltaTime);
	}
	if (m_isDie && m_isActive)
	{
		SetAction(EnemyAction::E_DIE);
		m_body->SetEnabled(false);
		if (m_animation->GetCurrentFrame() == m_animation->GetNumFrames() - 1)
		{
			m_isActive = false;
		}
		else
		{
			m_animation->Set2DPosition(m_animation->GetPosition().x, m_animation->GetPosition().y);
			m_animation->Update(deltaTime);
		}
		return;
	}
	if (m_isActive)
	{
		// ray casting 
		PerformRayCasting(positionPlayer);
		m_animation->Set2DPositionByTile(m_body->GetPosition().x, m_body->GetPosition().y);
		m_animation->Update(deltaTime);

		if (m_currentAction == EnemyAction::E_ATTACK && m_coolDown <= 0)
		{
			m_isReadyAttack = true;
			m_coolDown = COOLDOWN_ENEMY_BULLET;
		}
		else if (m_coolDown > 0 && !m_isReadyAttack)
		{
			m_coolDown -= deltaTime;
		}

		if (m_currentAction == EnemyAction::E_RUN)
		{
			if (m_type == EnemyType::MEGAMAN || m_type == EnemyType::PATREON || m_type == EnemyType::YUME || m_contacCount > 0)
			{
				m_body->SetLinearVelocity(m_speed);
			}
			else 
			{
				//m_body->SetLinearVelocity(m_speed);
			}
			
			m_timeFinding -= deltaTime;
			if (m_timeFinding <= 0)
			{
				SetAction(EnemyAction::E_IDLE);
			}
		}

		if (m_isReadyAttack)
		{
			b2Vec2 speed = b2Vec2_zero;

			switch (m_type)
			{
			case AR_MOD:
			case RPG_MOD:
			case Sniper_MOD:
				speed = m_sprinningDirection == DirectionType::RIGHT ? b2Vec2(-10, 0) : b2Vec2(10, 0);
				break;
			case PATREON:
			case MEGAMAN:
			case YUME:
				speed = positionPlayer - m_body->GetPosition();
				speed.Normalize();
				speed *= 10;
				break;
			default:
				break;
			}
			CreateBullet(speed);
			m_isReadyAttack = false;
		}

		// handle when take damage
		if (m_isTakeDamage)
		{
			m_blood->Update(deltaTime);
			m_blood->Set2DPositionByTile(m_body->GetPosition().x, m_body->GetPosition().y);
			if (m_blood->IsLastFrame())
			{
				m_isTakeDamage = false;
			}
		}
	}
}

void Enemies::Draw()
{
	if (m_isActive)
	{
		for (auto it : m_bulletList)
		{
			it->Draw();
		}
		m_animation->Draw();
		if (m_isTakeDamage)
		{
			m_blood->Draw();
		}
	}
}

void Enemies::Move()
{
}


void Enemies::Set2DPosition(GLint x, GLint y)
{
	//m_animation->Set2DPosition(x, y);
}

void Enemies::Set2DSize(GLint width, GLint height)
{
	m_animation->Set2DSize(width, height);
}

void Enemies::Set2DSizeByTile(GLfloat x, GLfloat y)
{
	m_animation->Set2DSizeByTile(x, y);
}

Vector2 Enemies::Get2DSize()
{
	return m_animation->GetSize();
}


void Enemies::SetTexture(GLint typeAction)
{

}

void Enemies::SetAction(EnemyAction action)
{
	if (action != EnemyAction::E_NONE && m_currentAction != action)
	{
		if (action == EnemyAction::E_IDLE || action == EnemyAction::E_ATTACK)
		{
			m_body->SetLinearVelocity(b2Vec2_zero);
		}

		m_currentAction = action;
		int check = m_type * 10 + m_currentAction;
		auto pos = m_animation->GetPosition();
		auto size = m_animation->GetSize();
		auto model = m_animation->GetModel();
		switch (check)
		{
		case 00:
		case 10:
		case 20:
			m_animation = std::make_shared<SpriteAnimation>("Enemy/ARMod_Idle.png", 8, 0.1f);
			m_animation->SetCurrentFrame(0);
			m_animation->Set2DSizeByTile(SCALE_SIZE, SCALE_SIZE);
			break;
		case 01: 
		case 11:
		case 21:
			m_animation = std::make_shared<SpriteAnimation>("Enemy/ARMod_Idle.png", 8, 0.1f);
			//m_animation = std::make_shared<SpriteAnimation>("Enemy/Enemy_run.png", 8, 0.15f);
			m_animation->Set2DSizeByTile(SCALE_SIZE, SCALE_SIZE);
			break;
		case 02:
			m_animation = std::make_shared<SpriteAnimation>("Enemy/ARMob_fire.png", 5, 0.1f);
			m_animation->Set2DSizeByTile(SCALE_SIZE, SCALE_SIZE);
			break;
		case 03:
		case 13:
		case 23:
			m_animation = std::make_shared<SpriteAnimation>("Enemy/ARMob_die.png", 4, 0.1f);
			m_animation->Set2DSizeByTile(SCALE_SIZE, SCALE_SIZE);
			break;
		case 12:
			m_animation = std::make_shared<SpriteAnimation>("Enemy/RPGMod_fire.png", 5, 0.1f);
			m_animation->Set2DSizeByTile(SCALE_SIZE * m_imgSize.x / m_imgSize.y, SCALE_SIZE );
			break;
		case 22:
			m_animation = std::make_shared<SpriteAnimation>("Enemy/SniperMob_fire.png", 9, 0.1f);
			m_animation->Set2DSizeByTile(SCALE_SIZE * m_imgSize.x / m_imgSize.y, SCALE_SIZE );
			break;
		case 30:
		case 31:
		case 32:
			m_animation = std::make_shared<SpriteAnimation>("Enemy/Patreon.png", 5, 0.1f);
			m_animation->Set2DSizeByTile(SCALE_SIZE, SCALE_SIZE);
			break;
		case 40:
		case 41:
		case 42:
			m_animation = std::make_shared<SpriteAnimation>("Enemy/Megaman.png", 4, 0.1f);
			m_animation->Set2DSizeByTile(SCALE_SIZE, SCALE_SIZE);
			break;
		case 50:
		case 51:
		case 52:
			m_animation = std::make_shared<SpriteAnimation>("Enemy/Yume.png", 4, 0.1f);
			m_animation->Set2DSizeByTile(SCALE_SIZE, SCALE_SIZE);
			break;
		case 33:
		case 43:
		case 53:
			m_animation = std::make_shared<SpriteAnimation>("Enemy/enemy_explosion.png", 7, 0.1f);
			m_animation->SetCurrentFrame(0);
			m_animation->Set2DSizeByTile(SCALE_SIZE, SCALE_SIZE);
			break;
		default:
			break;
		}
		m_sizeByTile = m_animation->GetSize() / tileSizeByPixel;
		m_animation->Set2DPosition(pos.x, pos.y);
		//m_animation->Set2DSize(size.x, size.y);
		m_animation->SetModel(model);
	}
}

void Enemies::SetActive(bool isActive)
{
	m_isActive = isActive;
	m_body->SetEnabled(isActive);
}

void Enemies::CreateEnemyBox(std::shared_ptr<b2World> world, float x, float y)
{
}

void Enemies::PerformRayCasting(b2Vec2 positionPlayer)
{
	EnemyAction action = EnemyAction::E_NONE;
	b2RayCastInput rayInput;

	rayInput.p1 = m_body->GetPosition();
	b2Vec2 p2 = positionPlayer - rayInput.p1;
	p2.Normalize();
	p2.x *= MAX_DISTANCE;
	p2.y *= MAX_DISTANCE;
	rayInput.p2 = rayInput.p1 + p2;
	world->RayCast(&m_rayCallback, rayInput.p1, rayInput.p2);

	if (m_rayCallback.m_isDetectObject)
	{
		action = EnemyAction::E_ATTACK;

		if (rayInput.p1.x < rayInput.p2.x && m_sprinningDirection == DirectionType::RIGHT ||
			(rayInput.p1.x > rayInput.p2.x && m_sprinningDirection == DirectionType::LEFT))
		{
			m_animation->FlipVertical();
			m_sprinningDirection = m_sprinningDirection == DirectionType::RIGHT ? LEFT : RIGHT;
		}
	}
	else 
	{
		if (m_currentAction == E_ATTACK)
		{
			action = E_RUN;
			b2Vec2 speed = rayInput.p2 - rayInput.p1;
			if (m_type == EnemyType::AR_MOD || m_type == EnemyType::RPG_MOD || m_type == EnemyType::Sniper_MOD)
			{
				speed.x = speed.x > 0 ? 3 : -3;
				speed.y = 0;
			}
			else
			{
				speed.Normalize();
				speed *= 3;
			}
			m_speed = speed;
			m_timeFinding = TIME_FINDING;
		}
	}

	SetAction(action);
}


b2Body* Enemies::GetBody()
{
	return m_body;
}


void Enemies::CalculateSize()
{
	
}

void Enemies::TakeDamage(GLint damage)
{
	m_isTakeDamage = true;
	m_blood->SetCurrentFrame(0);
	m_blood->Set2DSizeByTile(3, 3);
	m_blood->Set2DPositionByTile(m_body->GetPosition().x, m_body->GetPosition().y);

	m_health -= damage;

	if (m_health <= 0)
	{
		m_isDie = true;
		m_isTakeDamage = false;
		switch (m_type)
		{
		case AR_MOD:
		case RPG_MOD:
		case Sniper_MOD:
			ResourcesManager::GetInstance()->GetSound(12)->Play();
			break;
		case PATREON:
		case MEGAMAN:
		case YUME:
			ResourcesManager::GetInstance()->GetSound(2)->Play();
			break;
		default:
			break;
		}
	}
	m_timeBarDisplay = 2.0f;
}


void Enemies::Set2DPositionByTile(GLfloat x, GLfloat y)
{
	m_animation->Set2DPositionByTile(x, y);
}

std::shared_ptr<Enemies> Enemies::Clone()
{
	return std::make_shared<Enemies>(m_type, m_imgSize, m_boxSize, m_health, m_damage);
}

void Enemies::RunModUpdate(std::shared_ptr<Player> player)
{
}

void Enemies::FindPlayerUpdate(std::shared_ptr<Player> player)
{
	
}

void Enemies::AttackUpdate(std::shared_ptr<Player> player)
{
	
}

void Enemies::EnemyFlyUpdate(std::shared_ptr<Player> player)
{
	
}

void Enemies::CreateBullet(b2Vec2 speed)
{
	for (auto bullet : m_bulletList)
	{
		if (!bullet->IsActive())
		{
			Vector2 pos = Vector2(m_body->GetPosition().x, m_body->GetPosition().y);
			bullet->CreateNewBullet(m_enemyBulletType, speed, pos, m_damage);
			break;
		}
	}
}

