#pragma once

#include <unordered_map>
#include <Physics\Vec2.h>

#include "Map.h"
#include "GhostAi.h"
#include "Unit.h"

class PacmanGame
{
private:
	Map m_map;

	// Note:
	// I choose this type of container since we don't want to have 2 moving units
	// with the same type because Inky Ai movement is related to Blinky location
	Unit m_units[static_cast<size_t>(Unit::Type::kCount)];

	// Store a map with:
	// - Key = Unit id
	// - Value = Unit index at m_units array
	std::unordered_map<int, int> m_idToIndex;


public:
	PacmanGame();
	~PacmanGame() {}

	void Initialize();

	void LoadMap(const std::string& filePath) { m_map.Read(filePath); }
	const Map& GetMap() const { return m_map; }

	const Unit* GetUnit(Unit::Type unitType) const { return &m_units[static_cast<size_t>(unitType)]; }
	Unit* GetUnitWithId(int id) { return &m_units[m_idToIndex[id]]; }

	void RegisterUnit(int id, Unit::Type type, const Vec2& position);
};

