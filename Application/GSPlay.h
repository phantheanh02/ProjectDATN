﻿#pragma once
#include <memory>
#include <vector>
#include "box2d.h"
#include "GameStateBase.h"
#include "ResourcesManager.h"
#include "SceneManager.h"
#include "GameStateMachine.h"
#include "Button.h"
#include "Player.h"
#include "Bullet.h"
#include "Text.h"

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

private:
	// Kry/mouse event
	unsigned int	m_key;
	unsigned int	m_mouse;
	std::vector<std::string> m_keyStack;

	// background
	std::vector<std::shared_ptr<Button>>	m_buttonList;

	// camera
	std::shared_ptr<Camera>					m_dynamicCamera;
	std::shared_ptr<Camera>					m_staticCamera;
	Vector4									m_cameraPositionBoudaries;

	// map
	std::shared_ptr<MapClass> 				m_map;

	// box2d
	std::vector<b2Body*>					m_BodyList;
	ContactListener*						m_contactListener;
	GLfloat									m_timeStep;

	// player
	std::shared_ptr<Player>					m_player;

	// Bullet
	std::vector<std::shared_ptr<Bullet>>	m_bulletList;

	// Enemy
	std::vector<std::shared_ptr<Enemies>>	m_enemiesList;
	
	// Item
	std::vector<std::shared_ptr<Item>>		m_itemList;

	// HUB
	std::shared_ptr <Sprite2D>			m_bulletIcon;
	GLfloat								m_totalTime;

	// Effect
	std::shared_ptr<SpriteAnimation>	m_loadBullet;

	// Text
	std::shared_ptr<Text> m_numberBulletText;
	std::shared_ptr<Text> m_numberEnemiesKilledText;
	std::shared_ptr<Text> m_totalTimeText;
	
	// Popup
	std::vector<std::shared_ptr<Sprite2D>>	m_listPopupSprite;
	bool								m_isShowPopup;
private:
	void CreateBullet(BulletType type, b2Vec2 speed, Vector2 position);
	void RandomEnemies();
	void LoadMap();
	void CreatePopUp();
	void DrawPopup();
	void UpdatePopup(float deltaTime);
	void CreateButton(const char* filename, GLfloat width, GLfloat height, GLfloat posX, GLfloat posY, ButtonType buttonType);

};
	