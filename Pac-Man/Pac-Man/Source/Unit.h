#pragma once

#include <Physics/Vec2.h>
#include <utility>

#include "GhostAi.h"
#include "MovementDirection.h"

class PacmanGame;

class Unit
{
	//------------------------------------------------------------------------------------------------
	// Enumerator
	//------------------------------------------------------------------------------------------------
public:
	enum class Type
	{
		kPacman,
		kBlinky,
		kPinky,
		kInky,
		kClyde,
		kCount
	};

	//------------------------------------------------------------------------------------------------
	// Variables
	//------------------------------------------------------------------------------------------------
private:
	Vec2 m_position;	//The position of the unit in the tile
	MovementDirection m_direction;
	GhostAi* m_pGhostAi;

	PacmanGame* m_pGame;

	//------------------------------------------------------------------------------------------------
	// Functions
	//------------------------------------------------------------------------------------------------
public:
	Unit();
	~Unit();

	//Position
	void SetPosition(const Vec2& newPosition) { m_position = newPosition; }
	Vec2 GetPosition() const { return m_position; }
	
	//Direction
	void SetDirection(const Vec2& newDirection) { m_direction = newDirection; }
	Vec2 GetDirection() const { return m_direction.ToVec2(); }

	MovementDirection GetMovementDirection() const { return m_direction; }

	//Ai
	void SetAi(GhostAi* ghostAi);
	GhostAi* GetAi() const { return m_pGhostAi; }

	//Check if this is a null unit
	bool IsNull() { return (m_position.GetX() == -1 && m_position.GetY() == -1); }

	//Setter / Getter for game pointer
	void SetGame(PacmanGame* pGame) { m_pGame = pGame; }
	PacmanGame* GetGame() const { return m_pGame; }

	//Update
	void Update(double deltaTime);

//-------------------------------------------------------------------------------------------------
// This code only exist for debug mode
//-------------------------------------------------------------------------------------------------
#if _DEBUG
private:
	Type m_unitType;

public:
	void SetType(Unit::Type type) { m_unitType = type; }
	Type GetType() const { return m_unitType; }
#endif
};