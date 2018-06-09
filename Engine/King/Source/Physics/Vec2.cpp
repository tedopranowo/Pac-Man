#include "Vec2.h"

Vec2::Vec2(double x, double y)
    : m_x(x)
    , m_y(y)
{
}

void Vec2::Set(double x, double y)
{
    m_x = x;
    m_y = y;
}

Vec2 Vec2::operator+(const Vec2& rhs)
{
    this->m_x += rhs.m_x;
    this->m_y += rhs.m_y;
    return *this;
}

Vec2 Vec2::operator*(const int num)
{
	this->m_x *= num;
	this->m_y *= num;
	return *this;
}

Vec2 Vec2::operator-(const Vec2& rhs)
{
	this->m_x -= rhs.m_x;
	this->m_y -= rhs.m_y;
	return *this;
}

bool Vec2::operator==(const Vec2& rhs) const
{
	return (this->m_x == rhs.m_x) && (this->m_y == rhs.m_y);
}

double Vec2::SqrMagnitude() const
{
	return (this->m_x * this->m_x) + (this->m_y * this->m_y);
}