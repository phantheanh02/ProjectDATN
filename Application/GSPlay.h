#pragma once
#include <memory>
#include <vector>
#include "box2d.h"
#include "GameStateBase.h"
#include "ResourcesManager.h"
#include "SceneManager.h"
#include "GameStateMachine.h"
#include "Button.h"
#include "Player.h"
#include "ConfigClass.h"

constexpr auto VELOCITY_ITERATION = 8;
constexpr auto POSITION_ITERATION = 3;
constexpr auto MOVEMENT_SPEED = 7.0f;

class GSPlay final : public GameStateBase
{
public:
	using GameStateBase::GameStateBase;
	~GSPlay() override;

	void Init() override;
	void Update(float deltaTime) override;
	void Draw() override;
	void Pause() override;
	void Resume() override;
	void Exit() override;

	void HandleEvent();
	void OnKey(unsigned char key, bool pressed) override;
	void OnMouseClick(int x, int y, unsigned char key, bool pressed) override;
	void OnMouseMove(int x, int y) override;
	void OnMouseScroll(int x, int y, short delta) override;

	void Update2DDrawPosition();
	void LoadMap();
private:
	unsigned int	m_key;
	unsigned int	m_mouse;

	std::vector<std::shared_ptr<Button>>	m_listButton;
	std::shared_ptr<Sprite2D>				m_background;

	std::shared_ptr<Camera>					m_dynamicCamera;
	std::shared_ptr<Camera>					m_staticCamera;
	Vector4									m_cameraPositionBoudaries;

	std::shared_ptr<Player>					m_player;
	GLint									m_lastPlayerHorizontalDirection;
	GLfloat									m_lastJumpTime;
	MapInfo									m_map;

	// box2d
	std::shared_ptr<b2World>				m_world;
	std::vector<b2Body*>					m_BodyList;
	ContactListener*						m_contactListener;
	b2Vec2									m_gravity;
	GLfloat									m_timeStep;
};
	