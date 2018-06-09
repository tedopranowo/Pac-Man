#include "MovementDirection.h"



MovementDirection::MovementDirection()
	: m_direction{ Type::kNull }
{
}

MovementDirection::MovementDirection(const Vec2 & direction)
	: m_direction { GetType(direction)}
{
}

MovementDirection::MovementDirection(const Type directionType)
	: m_direction{ directionType }
{
}


MovementDirection::~MovementDirection()
{
}

Vec2 MovementDirection::ToVec2() const
{
	switch (m_direction)
	{
	case Type::kUp:
		return Vec2{ 0, -1 };
	case Type::kLeft:
		return Vec2{ -1, 0 };
	case Type::kDown:
		return Vec2{ 0, 1 };
	case Type::kRight:
		return Vec2{ 1, 0 };
	default:
		return Vec2{ 0, 0 };
	}
}

MovementDirection MovementDirection::Opposite() const
{
	switch (m_direction)
	{
	case Type::kUp:
		return MovementDirection(Type::kDown);
	case Type::kDown:
		return MovementDirection(Type::kUp);
	case Type::kRight:
		return MovementDirection(Type::kLeft);
	case Type::kLeft:
		return MovementDirection(Type::kRight);
	}
}

MovementDirection & MovementDirection::operator=(const MovementDirection & other)
{
	m_direction = other.m_direction;
	return *this;
}

MovementDirection::Type MovementDirection::GetType(const Vec2& direction)
{
	if (direction == Vec2{ 0, -1 })
		return Type::kUp;
	if (direction == Vec2{ 0, 1 })
		return Type::kDown;
	if (direction == Vec2{ 1, 0 })
		return Type::kRight;
	if (direction == Vec2{ -1, 0 })
		return Type::kLeft;
	if (direction == Vec2{ 0, 0 })
		return Type::kNull;

	return Type();
}

std::array<MovementDirection, static_cast<size_t>(MovementDirection::Type::kCount)> MovementDirection::GetAll()
{
	// TODO:
	// Subject to optimization, still not sure how -Tedo
	std::array<MovementDirection, static_cast<size_t>(MovementDirection::Type::kCount)> array;
	for (int i = 0; i < static_cast<int>(Type::kCount); ++i)
	{
		array[i].SetDirection(static_cast<Type>(i));
	}

	return array;
}

MovementDirection MovementDirection::Random()
{
	return MovementDirection(static_cast<Type>(rand() % static_cast<int>(Type::kCount)));
}

