// AnimationComponent.h

#include "SquareRenderComponent.h"

class AnimationComponent : public SquareRenderComponent
{
private:
	unsigned int m_frameCount;
	unsigned int m_currFrame;
	unsigned int m_xOffset;
	unsigned int m_yOffset;
	unsigned int m_origX;
	unsigned int m_origY;
	double m_totalTime;
	double m_nextAnimTime;

public:
	AnimationComponent() : SquareRenderComponent() {}
	AnimationComponent(unsigned int frameCount, unsigned int offsetX, unsigned int offsetY, double x, double y, double w, double h, SDL_Surface *filename);

	virtual void Update(double deltaTime) override;

};