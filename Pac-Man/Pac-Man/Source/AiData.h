#pragma once

#include "GhostAi.h"
#include "Unit.h"
#include "PacmanGame.h"

const AiDescriptor kAiData[static_cast<int>(Unit::Type::kCount)] =
{
	//Pacman
	{
		0.0f,
		nullptr,
		0.0f,
		Vec2(0, 0),
		0.0f
	},
	//Blinky
	{
		//Cage duration
		0.0f,
		//Chase target function
		[](const PacmanGame* pGame) -> Vec2
		{
			// Chase target is Pacman current location
			return pGame->GetUnit(Unit::Type::kPacman)->GetPosition();
		},
		//Chase duration
		20.0f,
		//Scatter location
		Vec2(50, -3),		//top right of map
		//Scatter duration
		7.0f
	},
	//Pinky
	{
		//Cage duration
		2.0f,
		//Chase target function
		[](const PacmanGame* pGame) -> Vec2
		{
			//Chase target is 4 step in front of pacman
			const Unit* pPacman = pGame->GetUnit(Unit::Type::kPacman);
			return pPacman->GetPosition() + pPacman->GetDirection() * 4;
		},
		//Chase duration
		20.0f,
		//Scatter location
		Vec2(2, -3),		//top left of map
		//Scatter duration
		7.0f
	},
	//Inky
	{
		//Cage duration
		15.0f,
		//Chase target function
		[](const PacmanGame* pGame) -> Vec2
		{
			//Chase target is Blinky location + (the vector of Blinky to Pacman multiplied by 2)
			const Unit* pPacman = pGame->GetUnit(Unit::Type::kPacman);
			const Unit* pBlinky = pGame->GetUnit(Unit::Type::kBlinky);

			Vec2 blinkyToPacmanVector = pPacman->GetPosition() - pBlinky->GetPosition();

			return pBlinky->GetPosition() + blinkyToPacmanVector * 2;
		},
		//Chase duration
		20.0f,
		//Scatter location
		Vec2(50, 50),		//bottom right of map
		//Scatter duration
		7.0f
	},
	//Clyde
	{
		//Cage duration
		30.0f,
		//Chase target function
		[](const PacmanGame* pGame) -> Vec2
		{
			// Chase target depends on the two two state
			// 1. If distance to pacman is bigger than 8, Clyde target location is pacman current location
			// 2. If distance to pacman is less than 8, Clyde target location is his scatter location
			const Unit* pPacman = pGame->GetUnit(Unit::Type::kPacman);
			const Unit* pClyde = pGame->GetUnit(Unit::Type::kClyde);

			double sqrDistance = (pClyde->GetPosition() - pPacman->GetPosition()).SqrMagnitude();

			//If distance is bigger than 8
			if (sqrDistance > 64)
				return pPacman->GetPosition();
			else
				return kAiData[static_cast<size_t>(Unit::Type::kClyde)].m_scatterLocation;
		},
		//Chase duration
		20.0f,
		//Scatter location
		Vec2(-50, 50),		//bottom left of map
		//Scatter duration
		7.0f
	}
};