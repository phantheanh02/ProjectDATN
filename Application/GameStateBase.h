#pragma once

enum class StateType
{
	STATE_INTRO,
	STATE_MENU,
	STATE_PLAY,
	STATE_SETTING,
	STATE_HIGHSCORE,
	STATE_CREDIT,
	STATE_MAP,
	STATE_CHOOSECHARACTER,
	STATE_INVALID
};

class GameStateBase
{
public:
	GameStateBase();
	GameStateBase(StateType stateType);
	virtual ~GameStateBase();

	virtual void Init() = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void Draw() = 0;
	virtual void Pause() = 0;
	virtual void Resume() = 0;
	virtual void Exit() = 0;

	StateType GetStateType();

	virtual void HandleEvent() = 0;
	virtual void OnKey(unsigned char key, bool pressed) = 0;
	virtual void OnMouseClick(int x, int y, unsigned char key, bool pressed) = 0;
	virtual void OnMouseMove(int x, int y) = 0;
	virtual void OnMouseScroll(int x, int y, short delta) = 0;

private:
	StateType m_stateType = StateType::STATE_INVALID;
};