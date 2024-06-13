#pragma once
#include <memory>
#include "GameStateBase.h"
#include "Texture.h"
#include "Camera.h"
#include "Shaders.h"
#include "Model.h"
#include "Sprite2D.h"
#include "Text.h"
#include "Player.h"
#include "MapClass.h"
#include "Bullet.h"

class GSSolo final : public GameStateBase
{
public:
	using GameStateBase::GameStateBase;
	~GSSolo() override;

	void Init() override;
	void Update(float deltaTime) override;
	void Draw() override;
	void Pause() override;
	void Resume() override;
	void Exit() override;

	void HandleEvent() override;
	void OnKey(unsigned char key, bool pressed) override;
	void OnMouseClick(int x, int y, unsigned char key, bool pressed) override;
	void OnMouseMove(int x, int y) override;
	void OnMouseScroll(int x, int y, short delta) override;

	void SendRequest();
private:
	std::shared_ptr<Sprite2D>	m_background;

	std::shared_ptr<Player>		m_ownerPlayer;
	std::shared_ptr<Player>		m_opponentPlayer;

	std::shared_ptr<MapClass> 	m_map;

	// Bullet
	std::vector<std::shared_ptr<Bullet>>	m_bulletList;

	// Key event
	unsigned int	m_key;
	std::vector<std::string> m_keyStack;

	unsigned int	m_opponentKey;
	std::vector<std::string> m_opponentKeyStack;

	// button
	std::vector<std::shared_ptr<Button>>	m_buttonList;

	// camera
	std::shared_ptr<Camera>					m_dynamicCamera;
	std::shared_ptr<Camera>					m_staticCamera;
	Vector4									m_cameraPositionBoudaries;

	// box2d
	std::vector<b2Body*>					m_BodyList;
	ContactListener*						m_contactListener;
	GLfloat									m_timeStep;

	// other
	bool									m_isReadyState;
private:
	void LoadMap();
	void HandleRequest();
	void CreateBullet(BulletType type, b2Vec2 speed, Vector2 position);
};