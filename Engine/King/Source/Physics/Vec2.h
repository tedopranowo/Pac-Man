#pragma once

class Vec2
{
public:
    double m_x, m_y;
    Vec2(double x = 0, double y = 0);

    void Set(double x, double y);
    const double GetX() const { return m_x; }
    const double GetY() const { return m_y; }

    Vec2 operator+(const Vec2& rhs);
	Vec2 operator*(const int num);
	Vec2 operator-(const Vec2& rhs);

	bool operator==(const Vec2& rhs) const;

	double SqrMagnitude() const;
};