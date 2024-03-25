#pragma once
#include <stack>
#include <memory>
#include "Singleton.h"
#include "GameStateBase.h"

class GameStateMachine final : public SingletonDclp<GameStateMachine>
{
public:
	void Init();
	void CleanUp();
	void PushState(StateType stateType);
	void PopState();
	void Update();
	void Exit();
	std::shared_ptr<GameStateBase> GetCurrentState();
	inline bool IsRunning() { return m_running; };

private:
	std::stack<std::shared_ptr<GameStateBase>> m_stateStack;
	std::shared_ptr<GameStateBase> m_pActiveState;
	std::shared_ptr<GameStateBase> m_pNextState;
	bool m_running;
};