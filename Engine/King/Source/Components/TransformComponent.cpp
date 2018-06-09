#include "TransformComponent.h"

#include "..\CollisionSystem\CollisionSystem.h"
#include "..\EventSystem\EventManager.h"
#include "..\Events\ActorMoveEvent.h"

//--------------------------------------------------------------------------------------
//  This component allows the Actor to move and change size if necessary.
//      -m_pos:     A Vec2 of the (x, y) position of the actor
//		hopefully fixed this IMPORTANT NOTE: m_pos is based on the position from tiled, which is bottom left
//
//      TODO: better way to handle this
//      -m_force:   Currently this represents the force applied to the transform when it
//                  moves.  We store this so we can push the actor in the opposite direction
//                  if it collides with a wall.
//      -m_scale:   The scale of the actor
//
//      TODO: I don't think I actually do anything with this data
//      -m_kFilename: 
//--------------------------------------------------------------------------------------

TransformComponent::TransformComponent()
    : m_pos(Vec2())
{
}

//--------------------------------------------------------------------------------------
// Initialize the component from xml data
//--------------------------------------------------------------------------------------
bool TransformComponent::Init(const tinyxml2::XMLElement* pData)
{
    m_kFilename                             = pData->GetText();
    const tinyxml2::XMLElement* xElement    = pData->FirstChildElement("Transform");
    m_scale                                 = xElement->DoubleAttribute("scale");
	m_width									= xElement->IntAttribute("width");
	m_height								= xElement->IntAttribute("height");
    m_pos.Set(xElement->DoubleAttribute("x"), xElement->DoubleAttribute("y"));
	
    return true;
}

//--------------------------------------------------------------------------------------
//  Create a clone of this component, this is called by it's owner (Actor) when creating
//  a clone of the owner (Actor).
//      -return: a shared_ptr to the clone of this component
//--------------------------------------------------------------------------------------
StrongActorComponentPtr TransformComponent::Clone()
{
    TransformComponent clone = TransformComponent();
    clone.m_kFilename        = this->m_kFilename;
    clone.m_pos              = this->m_pos;
    clone.m_scale            = this->m_scale;
	clone.m_width			 = this->m_width;
	clone.m_height			 = this->m_height;

    return StrongActorComponentPtr(std::make_shared<TransformComponent>(clone));
}

//--------------------------------------------------------------------------------------
//  This moves the transform to the location right before it entered the collision.
//--------------------------------------------------------------------------------------
void TransformComponent::MoveBack(const SDL_Rect &collider)
{
	if (m_pos.m_x - m_prevPos.m_x > 0)
		m_pos.m_x = collider.x - m_width - 1;
	else if (m_pos.m_x - m_prevPos.m_x < 0)
		m_pos.m_x = collider.x + collider.w + 1;
	if (m_pos.m_y - m_prevPos.m_y < 0)
		m_pos.m_y = collider.y + collider.h + 1;
	else if (m_pos.m_y - m_prevPos.m_y > 0)
		m_pos.m_y = collider.y - m_height - 1;
}

//--------------------------------------------------------------------------------------
//  This moves the transform by adding a force vector to the position vector
//      -x: the x of the force vector
//      -y: the y of the force vector
//--------------------------------------------------------------------------------------
void TransformComponent::Move(double x, double y)
{
	Vec2 force = Vec2(x, y);

	m_prevPos = m_pos;

	m_pos = m_pos + force;

	StrongActorPtr toAdd = GetOwner();

	EventDataPtr pActorMoved = std::make_shared<ActorMoveEvent>(toAdd);
	EventManager::Get()->TriggerEvent(pActorMoved);
}

const SDL_Rect TransformComponent::GetRect()
{
	SDL_Rect dimensions;
	dimensions.x = (int)m_pos.m_x;
	dimensions.y = (int)m_pos.m_y;
	dimensions.w = (int)m_width;
	dimensions.h = (int)m_height;
	return dimensions;
}