#pragma once

#include "BaseEventData.h"

#define NORTH PanCameraEvent::Direction::North
#define SOUTH PanCameraEvent::Direction::South
#define WEST PanCameraEvent::Direction::West
#define EAST PanCameraEvent::Direction::East

class PanCameraEvent : public BaseEventData
{
public:
    enum Direction
    {
        North,
        South,
        West,
        East,

        None
    };

    Direction m_direction;

    PanCameraEvent(Direction direction) : m_direction(direction) {}
    ~PanCameraEvent() {}


    static const EventType s_kEventType = 0xab47d126;

    virtual const char* GetName() const override { return "PanCameraEvent"; }
    virtual const EventType& GetEventType(void) const override { return s_kEventType; }
    virtual EventDataPtr Copy() const override { return std::make_shared<PanCameraEvent>(Direction::None); }
    const int GetDirection() const { return m_direction; }
};