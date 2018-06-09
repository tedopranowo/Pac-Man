#include "SquareRenderComponent.h"

#include <SDL_image.h>
#include <memory>

#include "TransformComponent.h"
#include "../Physics/Vec2.h"

//--------------------------------------------------------------------------------------
//  This component allows the actor have an image and be rendered.  Note there are 2 ways
//  that a SquareRenderComponent can be added to an Actor:
//      (1) Tiled:  If an object is placed in the map in Tiled, then it is NOT required
//                  for the XML to have a renderable component.  The data will be
//                  retreived from the TMX file.
//      (2) XML:    Like any other component, simply add the component data to the Actor's
//                  XML file (see ActorComponent for further details).
//
//      -m_pSurface:    The image to render
//      -m_pTexture:    An efficient driver-specific representation of pixel data
//      -m_src:         The part of the source image we wish to render
//      -m_dst:         Where in the game window to render the image
//--------------------------------------------------------------------------------------

using namespace std;

#pragma warning(push)
#pragma warning(disable:4100)

//--------------------------------------------------------------------------------------
// This default constructor will be used when an actor is dynamically created through
//  XML.
//--------------------------------------------------------------------------------------
SquareRenderComponent::SquareRenderComponent()
    : m_pSurface(nullptr)
    , m_pTexture(nullptr)
    , m_src(SDL_Rect())
{
}

//--------------------------------------------------------------------------------------
// This constructor is used when an actor is created from a Tiled map
//--------------------------------------------------------------------------------------
SquareRenderComponent::SquareRenderComponent(double x, double y, double w, double h, SDL_Surface* filename)
    : m_pSurface(filename)
{
    m_src.x = (int)x;
    m_src.y = (int)y;
    m_src.w = (int)w;
    m_src.h = (int)h;
    m_dst.w = (int)w;
    m_dst.h = (int)h;
}

//--------------------------------------------------------------------------------------
// Initialize the component from xml data
//--------------------------------------------------------------------------------------
bool SquareRenderComponent::Init(const tinyxml2::XMLElement * pData)
{
    const tinyxml2::XMLElement* xElement = pData->FirstChildElement("Source");

    m_pSurface  = IMG_Load(xElement->Attribute("filename"));
    m_src.x     = xElement->IntAttribute("x");
    m_src.y     = xElement->IntAttribute("y");
    m_src.w     = xElement->IntAttribute("width");
    m_src.h     = xElement->IntAttribute("height");
    m_dst.x     = xElement->IntAttribute("x");
    m_dst.y     = xElement->IntAttribute("y");
    m_dst.w     = xElement->IntAttribute("width");
    m_dst.h     = xElement->IntAttribute("height");

    return true;
}

//--------------------------------------------------------------------------------------
//  See if our owner has a transform and set this renderable to that location.
//  If our owner does not have a transform than we are a static renderable actor.
//--------------------------------------------------------------------------------------
void SquareRenderComponent::Update(double deltaTime)
{
    // Grab this actor's transform component
    shared_ptr<TransformComponent> pTransform = 
        static_pointer_cast<TransformComponent>(m_pOwner->GetComponent(ActorComponent::TransformComponentID));

    // Owner has a transform
    if (pTransform)
    {
        Vec2 posVec = pTransform->GetPosition();
        m_dst.x = (int)posVec.GetX();
        m_dst.y = (int)posVec.GetY();
    }
}

//--------------------------------------------------------------------------------------
// Render the Actor
//--------------------------------------------------------------------------------------
void SquareRenderComponent::Draw(SDL_Renderer* pRen, const Vec2& cameraPosition)
{
    // If this is the first time it's being rendered create the texture
    if (m_pTexture == nullptr)
        m_pTexture = SDL_CreateTextureFromSurface(pRen, m_pSurface);

    // Render the Actor in screen space
    m_dst.x += (int)cameraPosition.GetX();
    m_dst.y += (int)cameraPosition.GetY();
    SDL_RenderCopy(pRen, m_pTexture, &m_src, &m_dst);
}

//--------------------------------------------------------------------------------------
//  Create a clone of this component, this is called by it's owner (Actor) when creating
//  a clone of the owner (Actor).
//      -return: a shared_ptr to the clone of this component
//--------------------------------------------------------------------------------------
StrongActorComponentPtr SquareRenderComponent::Clone()
{
    SquareRenderComponent copy  = SquareRenderComponent();
    copy.m_pSurface             = this->m_pSurface;
    copy.m_pTexture             = this->m_pTexture;
    copy.m_dst                  = this->m_dst;
    copy.m_src                  = this->m_src;

    return StrongActorComponentPtr(std::make_shared<SquareRenderComponent>(copy));
}

#pragma warning(pop)