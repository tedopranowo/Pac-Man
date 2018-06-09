#pragma once

#include "Unit.h"

class AiFactory
{
public:
	//Static class
	AiFactory() = delete;
	AiFactory(const AiFactory& other) = delete;
	~AiFactory() = delete;

	static void AttachAi(Unit* pUnit, Unit::Type unitType);
};