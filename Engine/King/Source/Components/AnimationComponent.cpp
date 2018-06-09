// AnimationComponent.cpp

#include "AnimationComponent.h"

#include <memory>
#include "TransformComponent.h"

AnimationComponent::AnimationComponent(unsigned int frameCount, unsigned int offsetX, unsigned int offsetY, double x, double y, double w, double h, SDL_Surface *filename)
	: SquareRenderComponent(x, y, w, h, filename)
	, m_frameCount(frameCount)
	, m_currFrame(0)
	, m_origX((unsigned int) x)
	, m_origY((unsigned int) y)
	, m_xOffset(offsetX)
	, m_yOffset(offsetY)
	, m_totalTime(0.0)
	, m_nextAnimTime(1.0)
{
}

void AnimationComponent::Update(double deltaTime)
{
	m_totalTime += deltaTime;

	// If enough time has passes since our last frame switch, performa the next frame in the animation
	if (m_totalTime >= m_nextAnimTime)
	{
		if (m_currFrame == m_frameCount - 1)
		{
			m_dst.x = m_origX;
			m_dst.y = m_origY;
		}
		else
		{
			m_dst.x += m_xOffset;
			m_dst.y += m_yOffset;
			++m_currFrame;
		}
		m_totalTime = 0.0;
	}

	// Grab this actor's transform component
	std::shared_ptr<TransformComponent> pTransform =
		std::static_pointer_cast<TransformComponent>(m_pOwner->GetComponent(ActorComponent::TransformComponentID));

	// Owner has a transform
	if (pTransform)
	{
		Vec2 posVec = pTransform->GetPosition();
		m_dst.x = (int)posVec.GetX();
		m_dst.y = (int)posVec.GetY();
	}
}
