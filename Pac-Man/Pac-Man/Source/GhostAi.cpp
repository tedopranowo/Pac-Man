#include "GhostAi.h"

#include <iostream>
#include <assert.h>

#include "PacmanGame.h"
#include "AiState.h"

GhostAi::GhostAi(Unit* pOwner, const AiDescriptor & aiDescriptor)
	: m_pOwner{ pOwner }
	, m_isFrightened{ false }
{
	BuildAiStates(aiDescriptor);
}

GhostAi::~GhostAi()
{
	RemoveAiStates();
}

Vec2 GhostAi::GetMove() const
{
	const Map& map = m_pOwner->GetGame()->GetMap();
	Vec2 position = m_pOwner->GetPosition();

	//If right now the ghost is not at an intersection
	if (!map.IsIntersection(position))
	{
		//If there is a path in the current direction, keep continuing
		Vec2 nextLocation = position + m_pOwner->GetDirection();
		if (IsWalkableByGhost(nextLocation))
			return m_pOwner->GetDirection();

		//Otherwise, check every path and find the one which is walkable and is not the opposite of the current direction
		auto directionList = GetAllNonReversingWalkableDirection();

		// If there is no path other than turn back
		if (directionList.empty())
		{
			//Turn back the direction
			return m_pOwner->GetMovementDirection().Opposite().ToVec2();
		}
		// If there is only 1 path found
		else if (directionList.size() == 1)
		{
			return directionList[0];
		}
	}

	//If the ghost is frightened, choose a random path
	if (m_isFrightened)
		return MovementDirection::Random().ToVec2();

	//Find the target tile
	Vec2 targetTile = m_pCurState->GetTargetLocation();

	// Measure preferred path
	Vec2 move = MeasurePreferredPath(targetTile);

	return move;
}

void GhostAi::ChangeState(AiState * pNewState)
{
	//If the old state exist, call the exit function
	if (m_pCurState)
		m_pCurState->Exit();

	//Set the new state
	m_pCurState = pNewState;

	//If the new state exist, call the start function
	if (m_pCurState)
		m_pCurState->Start();
}

void GhostAi::ChangeState(State newState)
{
	assert( static_cast<int>(newState) < static_cast<int>(State::kCount) );
	ChangeState(m_aiStateList[static_cast<int>(newState)]);
}

void GhostAi::SetFrightened(bool isFrightened)
{
	m_isFrightened = isFrightened;

	//If the ghost is frightened, pause the ai state
	if (m_isFrightened)
		m_pCurState->Pause();
	//If the ghost is set to not frightened, resume the ai state
	else
		m_pCurState->Resume();
}

void GhostAi::Update(double deltaTime)
{
	m_pCurState->Update(deltaTime);
}

Vec2 GhostAi::FindPathHome()
{
	Map map = m_pOwner->GetGame()->GetMap();

	return MeasurePreferredPath(map.GetHomePosition());
}

Vec2 GhostAi::MeasurePreferredPath(const Vec2 & targetLocation) const
{
	// Find all possible path
	auto possiblePath = GetAllNonReversingWalkableDirection();

	// Choose the path with the shortest straight distance
	double shortestSquaredDistance = DBL_MAX;
	Vec2 shortestPath = Vec2(0, 0);

	// Loop for every path
	for (auto direction : possiblePath)
	{
		//Measure the distance
		Vec2 targetTile = m_pOwner->GetPosition() + direction;
		double squaredDistance = (targetTile - targetLocation).SqrMagnitude();

		//If this distance is the new shortest, set it as the new shortest
		if (squaredDistance < shortestSquaredDistance)
		{
			shortestSquaredDistance = squaredDistance;
			shortestPath = direction;
		}
	}

	return shortestPath;
}

std::vector<Vec2> GhostAi::GetAllNonReversingWalkableDirection() const
{
	Map map = m_pOwner->GetGame()->GetMap();
	std::vector<Vec2> nonReversingWalkableDirectionList;

	auto allDirection = MovementDirection::GetAll();
	for (size_t i = 0; i < allDirection.size(); ++i)
	{
		//If it is the opposite direction, skip it
		if (allDirection[i] == m_pOwner->GetMovementDirection().Opposite())
			continue;

		//If the target tile is walkable, push it to the list
		Vec2 targetTile = m_pOwner->GetPosition() + allDirection[i].ToVec2();

		if (IsWalkableByGhost(targetTile))
			nonReversingWalkableDirectionList.push_back(allDirection[i].ToVec2());
	}

	return nonReversingWalkableDirectionList;
}

void GhostAi::BuildAiStates(const AiDescriptor & aiDescriptor)
{
	const Map& map = m_pOwner->GetGame()->GetMap();

	//Clean up old data
	RemoveAiStates();

	//Caged state
	AiState* pCagedState = new ExactLocationState(this, map.GetHomePosition());
	m_aiStateList[static_cast<size_t>(State::kCaged)] = pCagedState;

	//Chase state
	AiState* pChaseState = new FunctionalLocationState(this, aiDescriptor.m_chaseTargetFunction);
	m_aiStateList[static_cast<size_t>(State::kChase)] = pChaseState;

	//Scatter state
	AiState* pScatterState = new ExactLocationState(this, aiDescriptor.m_scatterLocation);
	m_aiStateList[static_cast<size_t>(State::kScatter)] = pScatterState;

	//Running home state
	AiState* pRunningHomeState = new ExactLocationState(this, map.GetHomePosition());
	m_aiStateList[static_cast<size_t>(State::kRunningHome)] = pRunningHomeState;

	//Transition from caged to scatter
	AiStateTransition* pCagedToScatter = new STTimeLimitExceeded(pScatterState, aiDescriptor.m_cagedDuration);
	pCagedState->AddStateTransition(pCagedToScatter);
	m_aiTransitionStateList.push_back(pCagedToScatter);

	//Transition from chase to scatter
	AiStateTransition* pChaseToScatter = new STTimeLimitExceeded(pScatterState, aiDescriptor.m_chaseDuration);
	pChaseState->AddStateTransition(pChaseToScatter);
	m_aiTransitionStateList.push_back(pChaseToScatter);

	//Transition from scatter to chase
	AiStateTransition* pScatterToChase = new STTimeLimitExceeded(pChaseState, aiDescriptor.m_scatterDuration);
	pScatterState->AddStateTransition(pScatterToChase);
	m_aiTransitionStateList.push_back(pScatterToChase);

	//Set the current state as caged
	m_pCurState = pCagedState;

#if _DEBUG
	//Assign name to each state
	pCagedState->SetName("Caged State");
	pChaseState->SetName("Chase State");
	pScatterState->SetName("Scatter State");
	pRunningHomeState->SetName("RunningHome State");
#endif
}

void GhostAi::RemoveAiStates()
{
	//Clean up state list
	for (size_t i = 0; i < static_cast<size_t>(State::kCount); ++i)
	{
		delete m_aiStateList[i];
		m_aiStateList[i] = nullptr;
	}

	//Clean up transition state list
	for (auto pAiTransitionState : m_aiTransitionStateList)
	{
		delete pAiTransitionState;
	}
	m_aiTransitionStateList.clear();

	m_pCurState = nullptr;
}

bool GhostAi::IsWalkableByGhost(const Vec2 & position) const
{
	Map map = m_pOwner->GetGame()->GetMap();

	//Return true if either of this condition is fulfilled:
	//1. The tile is walkable
	//2. The tile is a gate and the ghost is neither in 'Frightened' nor 'Caged' state
	bool isCaged = (m_pCurState == m_aiStateList[static_cast<size_t>(State::kCaged)]);

	return map.IsWalkable(position) || (map.IsGate(position) && !isCaged && !m_isFrightened);
}
