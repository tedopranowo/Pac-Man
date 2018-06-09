#pragma once

#include <array>

#include <Physics\Vec2.h>

class MovementDirection
{
	enum class Type
	{
		kUp,
		kLeft,
		kDown,
		kRight,
		kCount,
		kNull
	};

private:
	Type m_direction;

public:
	//Constructor and destructor
	MovementDirection();
	MovementDirection(const Vec2& direction);
	MovementDirection(const Type directionType);
	~MovementDirection();
	
	//Setter
	void SetDirection(const Type directionType) { m_direction = directionType; }

	//Converter
	Vec2 ToVec2() const;
	MovementDirection Opposite() const;

	//Operator overload
	MovementDirection& operator=(const MovementDirection& other);

	bool operator!=(const MovementDirection& other) { return m_direction != other.m_direction; }
	bool operator==(const MovementDirection& other) { return m_direction == other.m_direction; }

	//Static
	static Type GetType(const Vec2& direction);
	static std::array<MovementDirection, static_cast<size_t>(Type::kCount)> GetAll();
	static MovementDirection Random();


};

