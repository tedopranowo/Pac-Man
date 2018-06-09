//------------------------- 
//Created by Shane King 
//Wed 05/03/2017 12:37:09.95 
//------------------------- 
#pragma once 

#include "ActorComponent.h"

#include "..\LuaManager\TypedefLua.h"
#include "..\Physics\Vec2.h"
#include "..\Generated\TransformComponent.gen.h"

class TransformComponent : public ActorComponent
{
private: 
    constexpr static ComponentId m_kId{ ActorComponent::TransformComponentID };
    Vec2 m_pos;
    Vec2 m_prevPos;
	int m_width;
	int m_height;

    double m_scale{ 0 };
    const char* m_kFilename;

public: 
	TransformComponent();
    virtual ~TransformComponent() {}
    
    // ActorComponent overrides
    virtual bool Init(const tinyxml2::XMLElement* pData) override;
	virtual void Update(double deltaSeconds) override { m_prevPos = m_pos; deltaSeconds; };
    virtual StrongActorComponentPtr Clone() override;
    virtual const ComponentId GetComponentId() const override { return m_kId; }

    // Accessors
	const int GetWidth() { return m_width; }
	const int GetHeight() { return m_height; }
    Vec2 GetPosition() const { return m_pos; }

	const SDL_Rect GetRect();
    void MoveBack(const SDL_Rect &collider);

    // Lua Functions
    void ExposeToLua(lua_State* pState);

    LuaFunction()
	void SetPosition(double x, double y) { m_pos.Set(x, y); }

    LuaFunction()
    virtual void Move(double x, double y);
};
