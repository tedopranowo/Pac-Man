#pragma once

#include <vector>
#include <Physics\Vec2.h>

class PacmanGame;
class Unit;
class AiState;
class AiStateTransition;

typedef Vec2(*LocationFunc)(const PacmanGame* pGame);

struct AiDescriptor
{
	//Caged
	double m_cagedDuration;

	//Chase
	LocationFunc m_chaseTargetFunction;
	double m_chaseDuration;

	//Scatter
	Vec2 m_scatterLocation;
	double m_scatterDuration;
};

class GhostAi
{
public:
	enum class State
	{
		kCaged,
		kChase,
		kScatter,
		kRunningHome,
		kCount
	};

private:
	AiState* m_pCurState;
	
	Unit* m_pOwner;

	AiState* m_aiStateList[static_cast<int>(State::kCount)];
	std::vector<AiStateTransition*> m_aiTransitionStateList;
	
	bool m_isFrightened;

public:
	GhostAi(Unit* pOwner, const AiDescriptor& aiDescriptor);
	~GhostAi();
	
	// Get Movement choice of Ai
	Vec2 GetMove() const;

	// Ai Owner
	Unit* GetOwner() { return m_pOwner; }

	// State
	void ChangeState(AiState* pNewState);
	void ChangeState(State newState);

	void SetFrightened(bool isFrightened);

	// Update
	void Update(double deltaTime);

	// Find path home
	Vec2 FindPathHome();

private:
	// AiState
	void BuildAiStates(const AiDescriptor& aiDescriptor);
	void RemoveAiStates();

	//Walkable check
	bool IsWalkableByGhost(const Vec2& position) const;

	// Pathfinding
	Vec2 MeasurePreferredPath(const Vec2& targetLocation) const;
	std::vector<Vec2> GetAllNonReversingWalkableDirection() const;
};

