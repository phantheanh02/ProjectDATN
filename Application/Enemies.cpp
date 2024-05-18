#include "stdafx.h"
#include "Enemies.h"
#include "ResourcesManager.h"
#include "GSPlay.h"
#include "Globals.h"

Enemies::Enemies(GLint id): m_id(id)
{
	auto model = ResourcesManager::GetInstance()->GetModel(ModelType::R_RETANGLE_TOPRIGHT);
	auto shader = ResourcesManager::GetInstance()->GetShader(1);
	auto texture = ResourcesManager::GetInstance()->GetTexture(12);
	m_animation = std::make_shared<SpriteAnimation>(0, model, shader, texture, 5, 0.15f);

	Intro();
}

Enemies::Enemies(GLint id, GLint idTexture): m_id(id)
{
	auto model = ResourcesManager::GetInstance()->GetModel(ModelType::R_RETANGLE_TOPRIGHT);
	auto shader = ResourcesManager::GetInstance()->GetShader(1);
	auto texture = ResourcesManager::GetInstance()->GetTexture(idTexture);
	m_animation = std::make_shared<SpriteAnimation>(0, model, shader, texture, 5, 0.15f);
	
	Intro();
}

Enemies::~Enemies()
{
}

void Enemies::Intro()
{
	m_fSpeed = 100.0f;
	m_isFire = false;
	m_isDie = false;
	m_isActive = true;
	m_timeDie = 1.5f;
	m_isTakeCoin = false;

	m_enemyBody = nullptr;
	m_enemyFixture = nullptr;

	m_currentDirection = 1;
	m_health = 3.0f;
	m_timeBarDisplay = 0.0f;
	auto model = ResourcesManager::GetInstance()->GetModel(ModelType::R_RETANGLE_TOPRIGHT);
	auto shader = ResourcesManager::GetInstance()->GetShader(0);
	auto texture = ResourcesManager::GetInstance()->GetTexture(57);
	m_hpBar = std::make_shared<Sprite2D>(0, model, shader, texture);

}

void Enemies::Update(float deltaTime, std::shared_ptr<Player> player)
{
	if (m_timeBarDisplay > 0)
	{
		m_hpBar->Set2DSize(1 * m_tileSizeByPixel * m_health / HP_ENEMY_MAX, 3);
		m_hpBar->Set2DPosition((m_enemyBody->GetPosition().x - 0.5) * m_tileSizeByPixel,
			(m_enemyBody->GetPosition().y - 1) * m_tileSizeByPixel);

		m_timeBarDisplay -= deltaTime;
	}

	if (m_isChange)
	{
		CalculateSize();
		m_isChange = false;
	}
	if (m_isActive)
	{
		b2Vec2 enemies_position = m_enemyBody->GetPosition();
		this->Set2DPositionFromBox2D(enemies_position.x, enemies_position.y);
	}
	if (m_isDie)
	{
		m_enemyBody->SetEnabled(false);
		m_timeDie -= deltaTime;
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
		if (typeEnemeis == TypeEnemyAction::RUN )
		{
			RunModUpdate(player);
		} 
		else if (typeEnemeis == TypeEnemyAction::FIND_PLAYER)
		{
			FindPlayerUpdate(player);
		}
		else if (typeEnemeis == TypeEnemyAction::ENEMY_03)
		{
			if (m_isFire)
			{
				m_coolDown -= deltaTime;
			}
			EnemyFlyUpdate(player);
		}
		else
		{
			m_coolDown -= deltaTime;
			AttackUpdate(player);
		}
		m_animation->Update(deltaTime);
	}
}

void Enemies::Draw()
{
	m_animation->Draw();
	if (m_timeBarDisplay > 0)
	{
		m_hpBar->Draw();
	}
}

void Enemies::Move()
{
}


void Enemies::Set2DPosition(GLint x, GLint y)
{
	m_animation->Set2DPosition(x, y);
}

void Enemies::Set2DSize(GLint width, GLint height)
{
	m_animation->Set2DSize(width, height);
}

void Enemies::Set2DSizeScroll()
{
	this->Set2DSize(size2D.x * m_tileSizeByPixel, size2D.y * m_tileSizeByPixel);
}

Vector2 Enemies::Get2DSize()
{
	return m_animation->GetSize();
}


void Enemies::SetTexture(GLint typeAction)
{
	m_isChange = true;
	typeEnemeis = typeAction;

	if (typeAction == TypeEnemyAction::FIND_PLAYER)
	{
		m_animation->SetTexture(ResourcesManager::GetInstance()->GetTexture(17));
		m_boxInfo = SceneManager::GetInstance()->GetBoxEnemy(17);
	}
	else
	{
		m_animation->SetTexture(ResourcesManager::GetInstance()->GetTexture(typeAction));
		m_boxInfo = SceneManager::GetInstance()->GetBoxEnemy(typeAction);
	}

	m_coolDown = 0;
	switch (typeAction)
	{
	case TypeEnemyAction::FIND_PLAYER:
		m_animation->SetNumFrame(8);
		m_animation->SetCurrentFrame(0);
		m_animation->SetTimeBtwFrame(0.1f);
		break;
	case TypeEnemyAction::RUN:
		m_animation->SetNumFrame(8);
		m_animation->SetCurrentFrame(0);
		m_animation->SetTimeBtwFrame(0.1f);
		break;
	case TypeEnemyAction::FIRE_DOWN:
		m_animation->SetNumFrame(3);
		m_animation->SetCurrentFrame(0);
		m_animation->SetTimeBtwFrame(0.1f);
		m_coolDown = m_animation->GetTimeAction();
		break;
	case TypeEnemyAction::FIRE_UP:
		m_animation->SetNumFrame(3);
		m_animation->SetCurrentFrame(0);
		m_animation->SetTimeBtwFrame(0.1f);
		m_coolDown = m_animation->GetTimeAction();
		break;
	case TypeEnemyAction::DIE:
		m_animation->SetNumFrame(4);
		m_animation->SetCurrentFrame(0);
		m_animation->SetTimeBtwFrame(0.1f);
		break;
	case TypeEnemyAction::ARMOD_FIRE:
		m_animation->SetNumFrame(5);
		m_animation->SetCurrentFrame(0);
		m_animation->SetTimeBtwFrame(0.1f);
		m_coolDown = m_animation->GetTimeAction();
		break;
	case TypeEnemyAction::RPGMOD_FIRE:
		m_animation->SetNumFrame(5);
		m_animation->SetCurrentFrame(0);
		m_animation->SetTimeBtwFrame(0.15f);
		break;
	case TypeEnemyAction::SNIPERMOD_FIRE:
		m_animation->SetNumFrame(9);
		m_animation->SetCurrentFrame(0);
		m_animation->SetTimeBtwFrame(0.1f);
		break;
	case TypeEnemyAction::ENEMY_01:
		m_animation->SetNumFrame(5);
		m_animation->SetCurrentFrame(0);
		m_animation->SetTimeBtwFrame(0.1f);
		m_coolDown = 1.0f;
		break;
	case TypeEnemyAction::ENEMY_02:
		m_animation->SetNumFrame(4);
		m_animation->SetCurrentFrame(0);
		m_animation->SetTimeBtwFrame(0.1f);
		m_coolDown = 1.0f;
		break;
	case TypeEnemyAction::ENEMY_03:
		m_animation->SetNumFrame(4);
		m_animation->SetCurrentFrame(0);
		m_animation->SetTimeBtwFrame(0.1f);
		m_coolDown = 1.0f;
		break;
	case TypeEnemyAction::ENEMY_EXPLOSION:
		m_animation->SetNumFrame(7);
		m_animation->SetCurrentFrame(0);
		m_animation->SetTimeBtwFrame(0.1f);
		break;
	default:
		break;
	}
}

void Enemies::Set2DPositionFromBox2D(GLfloat x, GLfloat y)
{
	float posX = x * m_tileSizeByPixel - Get2DSize().x / 2.0f ;
	float posY = y * m_tileSizeByPixel - (Get2DSize().y - sizeBox.y * m_tileSizeByPixel);
	m_animation->Set2DPosition(posX , posY);
}

void Enemies::CreateEnemyBox(std::shared_ptr<b2World> world, float x, float y)
{
	typeAttack = typeEnemeis;

	// Body
	b2BodyDef enemyBodyDef;
	enemyBodyDef.userData.pointer = (uintptr_t)this;
	enemyBodyDef.type = b2_dynamicBody;
	enemyBodyDef.position.Set(x , y);
	m_enemyBody = world->CreateBody(&enemyBodyDef);

	b2PolygonShape enemyShape;
	sizeBox = Vector2(m_boxInfo.boxSize.x / HEIGHT_ENEMY * ENEMY_SIZE, m_boxInfo.boxSize.y / HEIGHT_ENEMY * ENEMY_SIZE);
	enemyShape.SetAsBox(sizeBox.x, sizeBox.y);
	
	b2FixtureDef enemyFixtureDef;
	enemyFixtureDef.shape = &enemyShape;
	enemyFixtureDef.density = 1.0f;
	enemyFixtureDef.friction = 0.5f;
	enemyFixtureDef.filter.categoryBits = FixtureTypes::FIXTURE_ENEMY;
	enemyFixtureDef.filter.maskBits = FixtureTypes::FIXTURE_GROUND | FixtureTypes::FIXTURE_PLAYER_BULLET;
	m_enemyFixture = m_enemyBody->CreateFixture(&enemyFixtureDef);

	size2D.x = 2 * sizeBox.x * m_boxInfo.imgSize.x / m_boxInfo.boxSize.x;
	size2D.y = 2 * sizeBox.y * m_boxInfo.imgSize.y / m_boxInfo.boxSize.y;

	this->Set2DSize(size2D.x * m_tileSizeByPixel, size2D.y * m_tileSizeByPixel);
	if (this->typeEnemeis == TypeEnemyAction::ENEMY_03)
	{
		this->SetTexture(TypeEnemyAction::ENEMY_03);
		m_enemyBody->SetEnabled(false);
	}
	else
	{
		this->SetTexture(TypeEnemyAction::RUN);
	}
}

void Enemies::PerformRayCasting()
{
	b2RayCastInput rayInput;
	rayInput.p1 = m_enemyBody->GetPosition();
	// LEFT
	rayInput.p2 = rayInput.p1 + 100.0f * b2Vec2(-1.0f, 0.0f);
	m_enemyBody->GetWorld()->RayCast(&m_rayCallback, rayInput.p1, rayInput.p2);
	if (m_rayCallback.m_playerHit)
	{
		SetTexture(ARMOD_FIRE);
		m_isFire = true;
	}
	//RIGHT
	rayInput.p2 = rayInput.p1 + 100.0f * b2Vec2(1.0f, 0.0f);
	m_enemyBody->GetWorld()->RayCast(&m_rayCallback, rayInput.p1, rayInput.p2);
	if (m_rayCallback.m_playerHit)
	{
		SetTexture(ARMOD_FIRE);
		m_isFire = true;
	}
	////TOP
	//rayInput.p2 = rayInput.p1 + 100.0f * b2Vec2(0.0f, -1.0f);
	//m_enemyBody->GetWorld()->RayCast(&m_rayCallback, rayInput.p1, rayInput.p2);
	//if (m_rayCallback.m_playerHit)
	//{
	//	SetTexture(FIRE_UP);
	//	m_isFire = true;
	//}
	////DOWN
	//rayInput.p2 = rayInput.p1 + 100.0f * b2Vec2(0.0f, 1.0f);
	//m_enemyBody->GetWorld()->RayCast(&m_rayCallback, rayInput.p1, rayInput.p2);
	//if (m_rayCallback.m_playerHit)
	//{
	//	
	//	SetTexture(FIRE_DOWN);
	//	m_isFire = true;
	//}
}


b2Body* Enemies::GetEnemyBody()
{
	return m_enemyBody;
}

b2Fixture* Enemies::GetEnemyFixture()
{
	return m_enemyFixture;
}

void Enemies::CalculateSize()
{
	b2PolygonShape enemyShape;
	Vector2 newSizeBox = Vector2(m_boxInfo.boxSize.x / HEIGHT_ENEMY * ENEMY_SIZE, m_boxInfo.boxSize.y / HEIGHT_ENEMY * ENEMY_SIZE);
	Vector2 deltaPos = newSizeBox - sizeBox;
	sizeBox = newSizeBox;
	enemyShape.SetAsBox(sizeBox.x, sizeBox.y);
	m_enemyBody->SetTransform(b2Vec2(m_enemyBody->GetPosition().x - deltaPos.x, m_enemyBody->GetPosition().y - deltaPos.y), 0.0f);

	if (m_enemyFixture != nullptr)
	{
		m_enemyBody->DestroyFixture(m_enemyFixture);
	}

	b2FixtureDef enemyFixtureDef;
	enemyFixtureDef.shape = &enemyShape;
	enemyFixtureDef.density = 1.0f;
	enemyFixtureDef.filter.categoryBits = FIXTURE_ENEMY;
	enemyFixtureDef.filter.maskBits = FIXTURE_GROUND | FIXTURE_PLAYER_BULLET;
	m_enemyFixture = m_enemyBody->CreateFixture(&enemyFixtureDef);
	
	size2D.x = 2 * sizeBox.x * m_boxInfo.imgSize.x / m_boxInfo.boxSize.x;
	size2D.y = 2 * sizeBox.y * m_boxInfo.imgSize.y / m_boxInfo.boxSize.y;

	this->Set2DSize(size2D.x * m_tileSizeByPixel, size2D.y * m_tileSizeByPixel);
}

void Enemies::SetBoundaryMove(GLfloat x, GLfloat y)
{
	m_boundaryMove = Vector2(x, y);
}

void Enemies::TakeDamage(GLint damage)
{
	m_health -= damage;
	if (m_health <= 0)
	{
		Kill();
	}
	m_timeBarDisplay = 2.0f;
}

void Enemies::Kill()
{
	m_isDie = true;
	m_isChange = true;
	if (typeEnemeis == TypeEnemyAction::ENEMY_03)
	{
		this->SetTexture(TypeEnemyAction::ENEMY_EXPLOSION);
	}
	else
	{
		this->SetTexture(DIE);
	}
}

void Enemies::RunModUpdate(std::shared_ptr<Player> player)
{
	b2Vec2 currentPosition = m_enemyBody->GetPosition();
	float newX = currentPosition.x + m_currentDirection * 0.02f;

	if (newX <= m_boundaryMove.x || newX >= m_boundaryMove.y) {
		m_currentDirection = -m_currentDirection;
		m_animation->FlipVertical();
	}

	m_enemyBody->SetTransform(b2Vec2(newX, currentPosition.y), 0.0f);
	// Distance Attack
	GLfloat distanceToPlayer = b2Distance(m_enemyBody->GetPosition(), player->GetPlayerBody()->GetPosition());
	if (distanceToPlayer <= DETECTION_DISTANCE)
	{
		SetTexture(typeAttack);
		CalculateSize();
		if (m_enemyBody->GetPosition().x > player->GetPlayerBody()->GetPosition().x && m_currentDirection == 1 ||
			m_enemyBody->GetPosition().x < player->GetPlayerBody()->GetPosition().x && m_currentDirection == -1)
		{
			m_animation->FlipVertical();
			m_currentDirection = -m_currentDirection;
		}
	}
}

void Enemies::FindPlayerUpdate(std::shared_ptr<Player> player)
{
	b2Vec2 currentPosition = m_enemyBody->GetPosition();
	float newX = currentPosition.x + m_currentDirection * 0.02f;

	if (newX < m_boundaryMove.x || newX > m_boundaryMove.y) {
		m_currentDirection = -m_currentDirection;
		m_animation->FlipVertical();
	}

	m_enemyBody->SetTransform(b2Vec2(newX, currentPosition.y), 0.0f);
	GLfloat distanceToPlayer = b2Distance(m_enemyBody->GetPosition(), player->GetPlayerBody()->GetPosition());
	if (distanceToPlayer <= DETECTION_DISTANCE)
	{
		PerformRayCasting();
	}
}

void Enemies::AttackUpdate(std::shared_ptr<Player> player)
{
	if (typeEnemeis == TypeEnemyAction::ARMOD_FIRE || typeEnemeis == TypeEnemyAction::FIRE_UP || typeEnemeis == TypeEnemyAction::FIRE_DOWN)
	{
		GLfloat distanceToPlayer = b2Distance(m_enemyBody->GetPosition(), player->GetPlayerBody()->GetPosition());
		if (distanceToPlayer > DETECTION_DISTANCE)
		{
			SetTexture(TypeEnemyAction::FIND_PLAYER);
			m_isFire = false;
		}
	}

	if (m_enemyBody->GetPosition().x > player->GetPlayerBody()->GetPosition().x && m_currentDirection == 1 ||
		m_enemyBody->GetPosition().x < player->GetPlayerBody()->GetPosition().x && m_currentDirection == -1)
	{
		m_animation->FlipVertical();
		m_currentDirection = -m_currentDirection;
	}
}

void Enemies::EnemyFlyUpdate(std::shared_ptr<Player> player)
{
	m_enemyBody->SetGravityScale(0.0f);
	GLfloat distanceToPlayer = b2Distance(m_enemyBody->GetPosition(), player->GetPlayerBody()->GetPosition());
	if (distanceToPlayer < DETECTION_DISTANCE)
	{
		m_isFire = true;
		m_enemyBody->SetLinearVelocity(b2Vec2_zero);
		m_enemyBody->SetEnabled(true);
	}
	else 
	{
		if (m_isFire)
		{
			b2Vec2 speed = player->GetPlayerBody()->GetPosition() - m_enemyBody->GetPosition();
			speed.Normalize();
			speed.x *= 2.0f;
			speed.y *= 2.0f;
			if (player->GetPlayerBody()->GetPosition().y - m_enemyBody->GetPosition().y > 4.0f)
			{
				m_enemyBody->SetLinearVelocity(speed);
			}
			else
			{
				m_enemyBody->SetLinearVelocity(b2Vec2(speed.x, 0.0f));
			}
		}
	}
}

