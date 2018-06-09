#pragma once

#include <vector>

#include <Physics\Vec2.h>

class Map
{
	enum class TileType
	{
		//Walkable
		kEmpty,
		kGhostHome,

		//Non walkable
		kWall,
		kGate
	};

	typedef std::vector<std::vector<TileType>> TileChunks;
	static constexpr int kWalkableMaxIndex = 1;

private:
	// Store the tile data
	// Walkable:
	// - 0 = empty
	// - 1 = ghost home
	// Not walkable:
	// - 2 = wall
	// - 3 = gate
	TileChunks m_tiles;

	Vec2 m_ghostHomePos;

public:
	Map();
	~Map();

	void Read(const std::string& filePath);

	bool IsValid(const Vec2& position) const;
	bool IsValid(const size_t x, const size_t y) const;

	bool IsWalkable(const Vec2& position) const;
	bool IsWalkable(const size_t x, const size_t y) const;

	bool IsIntersection(const Vec2& position) const;
	bool IsIntersection(const size_t x, const size_t y) const;

	bool IsGate(const Vec2& position) const;
	bool IsGate(const size_t x, const size_t y) const;

	size_t GetHeight() const { return m_tiles.size(); }
	size_t GetWidth() const { return m_tiles[0].size(); }

	Vec2 GetHomePosition() const { return m_ghostHomePos; }
};