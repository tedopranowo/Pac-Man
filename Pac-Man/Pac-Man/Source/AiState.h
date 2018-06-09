#pragma once

#include <vector>

#include <Physics\Vec2.h>

#include "GhostAi.h"

class AiStateTransition;

//-------------------------------------------------------------------------------------------------
// AiState
// - Defines where the Ai target location is
//-------------------------------------------------------------------------------------------------
class AiState
{
protected:
	GhostAi* m_pOwner;
	std::vector<AiStateTransition*> m_transitionList;
	bool m_isPaused;

#if _DEBUG
	std::string m_stateName;
#endif

public:
	AiState(GhostAi* pOwner) : m_pOwner{ pOwner } {}
	virtual ~AiState() {}

	void AddStateTransition(AiStateTransition* pTransition) { m_transitionList.push_back(pTransition); }

	virtual Vec2 GetTargetLocation() = 0;

	virtual void Start();
	virtual void Update(double deltaTime);
	virtual void Exit();

	void Pause() { m_isPaused = true; }
	void Resume() { m_isPaused = false; }

#if _DEBUG
	void SetName(const std::string& name) { m_stateName = name; }
	const std::string& GetName() const { return m_stateName; }
#endif
};

class ExactLocationState : public AiState
{
private:
	Vec2 m_targetLocation;

public:
	ExactLocationState(GhostAi* pOwner, const Vec2& targetLocation);

	// Inherited via AiState
	virtual Vec2 GetTargetLocation() override { return m_targetLocation; }
};

class FunctionalLocationState : public AiState
{
private:
	LocationFunc m_targetLocationFunc;

public:
	FunctionalLocationState(GhostAi* pOwner, LocationFunc targetLocationFunc);

	// Inherited via AiState
	virtual Vec2 GetTargetLocation() override;
};

class RandomLocationState : public AiState
{
private:
	Vec2 m_targetLocation;

public:
	RandomLocationState(GhostAi* pOwner) : AiState(pOwner) {}

	virtual void Start() override;

	// Inherited via AiState
	virtual Vec2 GetTargetLocation() override { return m_targetLocation; }
};



//-------------------------------------------------------------------------------------------------
// AiStateTransition
// - Defines when a state should change and what it should change to
//-------------------------------------------------------------------------------------------------
class AiStateTransition
{
private:
	AiState* m_pNextState;
	bool m_isPaused;

public:
	AiStateTransition(AiState* pNextState) : m_pNextState{ pNextState } {}
	virtual ~AiStateTransition() {}

	AiState* GetNextState() { return m_pNextState; }

	virtual void Start() {}
	virtual void Update(double deltaTime) {}
	virtual void Exit() {}

	virtual bool CheckChangeStateCondition() = 0;
};

class STTimeLimitExceeded : public AiStateTransition
{
private:
	double m_timeLimit;
	double m_timeSinceStart;

public:
	STTimeLimitExceeded(AiState* pNextState, double timeLimit);
	virtual ~STTimeLimitExceeded() {}

	virtual void Start() override { m_timeSinceStart = 0.0f; }
	virtual void Update(double deltaTime) override { m_timeSinceStart += deltaTime; }

	// Inherited via AiStateTransition
	virtual bool CheckChangeStateCondition() override { return m_timeSinceStart >= m_timeLimit; }
};