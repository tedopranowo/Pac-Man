#pragma once

#include "BaseEventData.h"

#include <vector>
#include "..\Tiled\Tiled.h"

class MapLoadedEvent : public BaseEventData
{
private:
    std::vector<Tiled::TileType>* m_tileTypes;
    std::vector<std::vector<Tiled::Tile>>* m_map;
public:
    static const EventType s_kEventType = 0xeac9d89b;
    MapLoadedEvent(std::vector<Tiled::TileType>* tileTypes, std::vector<std::vector<Tiled::Tile>>* map)
        : m_tileTypes(tileTypes)
        , m_map(map)
    {}
    ~MapLoadedEvent() {}

    // Inherited via BaseEventData
    virtual const char * GetName() const override { return "MapLoadedEvent"; }
    virtual const EventType & GetEventType(void) const override { return s_kEventType; };
    virtual float GetTimeStamp() const override { return 0.0f; }
    virtual EventDataPtr Copy() const override { return EventDataPtr(std::make_shared<MapLoadedEvent>(m_tileTypes, m_map)); };

    std::vector<Tiled::TileType>* GetTileTypes() const { return m_tileTypes; }
    std::vector<std::vector<Tiled::Tile>>* GetMap() const { return m_map; }
};